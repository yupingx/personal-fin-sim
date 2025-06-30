/* ============================================================================
 * Asset.cpp
 *
 *  Description:
 *  ---------------------------------------------------------------------------
 *  Implementation of the Asset class.
 *
 *  Dependencies:
 *    - asset.h
 *    - constants.h
 *
 *  Related Files:
 *    - modelRecession.h / modelRecession.cpp
 *
 *  Created: 	June 2025
 *  Author:     Yuping X
 * ============================================================================
 */

#include <iostream>
#include <algorithm>
#include "../include/asset.h"
#include "../include/constants.h"

int Asset::getFundLongevity()
{
	return fundLongevity_;
}

int Asset::getCashReserve()
{
	return cashReserve_;
}

/* We use available funds in the asset with index asset_num to replenish cash
 * reserve.
 */
bool Asset::addCashReserve(int cash, int asset_num, int n_year)
{
	if (value_[asset_num][n_year] > cash)
	{
		cashReserve_ += cash;
		value_[asset_num][n_year] -= cash;
		return true;
	}
	else
	{
		return false;
	}
}

void Asset::clearCashReserve()
{
	cashReserve_ = 0;
}

void Asset::calculateN()
{
	long int distributable_total;
	float distribution_percentage;
	int i;
	bool cashAdded;

	for (i = 0; i < MAX_YEARS; i++)
	{		
		/* Once retirement year is reached, deactivate recurring income and
		 * contributions */
		if (i == yearsTillRetirement_) {
			if (DEBUG_PRINT) {
				std::cout << "DEBUG: This is the year of retirement. \n " << '\n';
			}
			takehomeIncome_ = 0;
			contributionRoth_ = 0;
			contributionIra_ = 0;
			contributionR401k_ = 0;
		}

		/* Decide if we should use cash reserve
		 * If growth within the past year is negative, use cash reserve (if available)
		 * If growth within the past year is above a threshold and cash reserve is depleted, replenish it
		 * For now we assume cash reserve is always less than one year's expense and we take it as a whole
		 * TODO: add possibility to use partial cash reserve
		 * TODO: refine cash reserve logic
		 */

		if ((i > 0) && (growthRate_[INDIVIDUAL_INDEX][i - 1] < 0) && (cashReserve_ > 0))
		{
			if (DEBUG_PRINT)
			{
				std::cout << "DEBUG: year " << CURRENT_YEAR + i \
				          << " cash reserve " << cashReserve_ << " used up " << '\n';
			}
			expense_[i] -= Asset::cashReserve_;
			clearCashReserve();
		}
		else if ((i > 0) && (growthRate_[INDIVIDUAL_INDEX][i - 1] > 0) && (cashReserve_ == 0))
		{
			cashAdded = Asset::addCashReserve(CASH_RESERVE, INDIVIDUAL_INDEX, i);
			if (DEBUG_PRINT)
			{
				if (cashAdded)
				{
					std::cout << "DEBUG: year " << CURRENT_YEAR + i \
					          << " cash reserve replenished." << '\n';
				}
				else
				{
					std::cout << "DEBUG: year " << CURRENT_YEAR + i \
					          << " unable to replenish cash reserve." << '\n';
				}
			}
		}

		/* Then calculate current year distribution */
		distributable_total = 0;
		for (int c = 0; c < MAX_ACCOUNTS; c++)
		{
			if (availability_[c][i])
			{
				distributable_total += value_[c][i];
			}
		}

		/* Calculate current year actual expense and potential surplus for
		 * investing considering this year's takehome income.
		 * */
		long int net_expense = std::max(expense_[i] - takehomeIncome_, (long int) 0);
		long int contribution_individual = std::max(takehomeIncome_ - expense_[i], (long int) 0);

		if (net_expense > distributable_total)
		{
			if (net_expense < distributable_total + cashReserve_)
			{
				/* This is a corner case when expense is less than investments
				 * but can be covered by the cash reserve */
				if (DEBUG_PRINT)
				{
					std::cout << "DEBUG: year " << CURRENT_YEAR + i \
					          << " cash reserve " << cashReserve_ \
							  << " used up " << '\n';
				}
				expense_[i] -= cashReserve_;
				clearCashReserve();
			}
			else
			{
				break;
			}
		}

		distribution_percentage = float(net_expense) / float(distributable_total);

		if (DEBUG_PRINT)
		{
			std::cout << "DEBUG: Takehome income: " \
			          << takehomeIncome_ << '\n';
			std::cout << "DEBUG: Net expense (after considering income): " \
			          << net_expense << '\n';
			std::cout << "DEBUG: year " << CURRENT_YEAR + i \
			          << " distributable percentage: " << (distribution_percentage * 100) << "%" \
					  << '\n';
		}

		for (int c = 0; c < MAX_ACCOUNTS; c++)
		{
			if (availability_[c][i])
			{
				/* Next, take out distribution */ 
				distribution_[c][i] = value_[c][i] * distribution_percentage;
				if (DEBUG_PRINT)
				{
					std::cout << "DEBUG: Asset #" << c \
					          << " distribution: " << distribution_[c][i] \
							  << '\n';
				}
			}
			else
			{
				Asset::distribution_[c][i] = 0;
			}
			/* Next, if we haven't reached MAX_YEARS, calculate the remaining
			 * value plus growth, which will be next year's pre-distribution value
			 */
			if (i + 1 < MAX_YEARS) {
				value_[c][i + 1] = (value_[c][i] - distribution_[c][i]) * (
					1 + growthRate_[c][i]);
			}
				
			if (DEBUG_PRINT)
			{
				std::cout << "DEBUG: This year's growth: " << Asset::growthRate_[c][i] << '\n';
				std::cout << "DEBUG: Asset #" << c \
				          << " next year's starting value: " \
						  << value_[c][i + 1] << '\n';
			}
		}

		/* Build each investment account by current year contribution if before
		 * retirement */
		if ((i < yearsTillRetirement_) && (i + 1 < MAX_YEARS)) {
			value_[INDIVIDUAL_INDEX][i + 1] += contribution_individual;
			value_[ROTH_INDEX][i + 1] += contributionRoth_;
			value_[IRA_INDEX][i + 1] += contributionIra_;
			value_[R401K_INDEX][i + 1] += contributionR401k_;

		    /* Update roth, ira and 401k contributions by the rate of inflation.
			 * Since we don't know future contribution limits, this is a reasonable
			 * estimate.
			 * */
			contributionRoth_ = contributionRoth_ * (1 + inflation_[i]);
			contributionIra_ = contributionIra_ * (1 + inflation_[i]);
			contributionR401k_ = contributionR401k_ * (1 + inflation_[i]);

			/* Grow user's next year takehome income by the rate of inflation */
			takehomeIncome_ = takehomeIncome_ * (1 + inflation_[i]);

		}

		/* Update next year's projected expense: */
		if (i + 1 < MAX_YEARS) {
			expense_[i + 1] = expense_[i] * (1 + inflation_[i]);
		}

		/* Finally, grow cash reserve (if available) by the rate of inflation */

		if (cashReserve_ > 0)
		{
			cashReserve_ = cashReserve_ * (1 + inflation_[i]);
		}
	}

	fundLongevity_ = i;

	if (DEBUG_PRINT)
	{
		std::cout << "DEBUG: CONCLUSION: Fund will last " \
		          << fundLongevity_ << " years. " << '\n';
	}
}

void Asset::initializeFromUserData(const UserData& user) {

	fundLongevity_ = 0;
	cashReserve_ = 0;

	for (int c = 0; c < MAX_ACCOUNTS; c++) {
		name_[c] = user.name[c];
		value_[c][0] = user.value[c];
		growthRateAvg_[c] = user.rate[c];
		distribution_[c][0] = 0;
	}
	expense_[0] = user.initialExpense;
	takehomeIncome_ = user.takehomeIncome;
	contributionRoth_ = user.contributionRoth;
	contributionIra_ = user.contributionIra;
	contributionR401k_ = user.contributionR401k;
	yearsTillRetirement_ = user.yearsTillRetirement;
	inflation_[0] = user.initialInflation;
	availability_[INDIVIDUAL_INDEX][0] = true;
	availability_[ROTH_INDEX][0] = false;
	availability_[IRA_INDEX][0] = false;
	availability_[R401K_INDEX][0] = false;
	clearCashReserve();
	addCashReserve(CASH_RESERVE, INDIVIDUAL_INDEX, 0);

	for (int i = 1; i < MAX_YEARS; i++) {
		for (int c = 0; c < MAX_ACCOUNTS; c++) {
			value_[c][i] = value_[c][0];
			distribution_[c][i] = distribution_[c][0];
			availability_[INDIVIDUAL_INDEX][i] = availability_[0][0];

			if (i < user.yearsTillRetirement) {
				availability_[ROTH_INDEX][i] = availability_[ROTH_INDEX][0];
				availability_[IRA_INDEX][i] = availability_[IRA_INDEX][0];
				availability_[R401K_INDEX][i] = availability_[R401K_INDEX][0];
			} else {
				availability_[ROTH_INDEX][i] = true;
				availability_[IRA_INDEX][i] = true;
				availability_[R401K_INDEX][i] = true;
			}
		}
		expense_[i] = expense_[0];
		inflation_[i] = inflation_[0];
	}
}

Asset::Asset()
{
	fundLongevity_ = 0;
	cashReserve_ = 0;
	yearsTillRetirement_ = 0;
	takehomeIncome_ = 0;
	contributionRoth_ = 0;
	contributionIra_ = 0;
	contributionR401k_ = 0;

	for (int c = 0; c < MAX_ACCOUNTS; c++)
	{
		name_[c] = "";

		for (int y = 0; y < MAX_YEARS; y++)
		{
			value_[c][y] = 0;
			distribution_[c][y] = 0;
			availability_[c][y] = false;
			growthRate_[c][y] = 0.0f;
		}

		growthRateAvg_[c] = 0.0f;
	}

	for (int y = 0; y < MAX_YEARS; y++)
	{
		expense_[y] = 0;
		inflation_[y] = 0.0f;
	}

	if (DEBUG_PRINT)
	{
		std::cout << "Asset fully initialized to zero." << '\n';
	}
}

Asset::~Asset() {
}
