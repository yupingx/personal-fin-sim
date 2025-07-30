/* ============================================================================
 * Asset.h
 *
 *  Description:
 *  ---------------------------------------------------------------------------
 *  Declares the Asset class, which models the behavior of a portfolio of investment
 *  accounts over time. Each Asset tracks growth, expenses, inflation, distribution,
 *  and cash reserve logic used to estimate fund longevity in personal retirement
 *  planning or financial simulation.
 *
 *  Core Responsibilities:
 *    - Maintain account-level value, availability, and growth data.
 *    - Simulate cash reserve replenishment or depletion.
 *    - Compute annual distributions to meet modeled expenses.
 *    - Project how long available funds can sustain the target expenses.
 *    - Populate growth curves based on predefined or randomized scenarios.
 *
 *  Public Interface Highlights:
 *    - calculateN(): Main driver that simulates fund longevity.
 *    - populateGrowthCurves(): Builds per-asset growth rate projections.
 *    - addCashReserve(), clearCashReserve(): Manage cash buffer logic.
 *    - getFundLongevity(): Returns number of years the fund can last.
 *
 *  Dependency:
 * 	  - UserData (simple struct defined in userDataLoading.h)
 * 
 *  Related Files:
 *    - Asset.cpp
 *    - constants.h
 *    - modelRecession.h / modelRecession.cpp
 *
 *  Created: 	June 2025
 *  Author:     Yuping X
 * ============================================================================
 */

#ifndef ASSET_H_
#define ASSET_H_

#include <string>
#include <array>
#include "constants.h"
#include "modelRecession.h"
#include "userDataLoading.h"

/**
 * @brief Represents and simulates the evolution of financial assets over time.
 *
 * The Asset class models asset value growth, cash reserves, and distributions
 * based on user data and different market scenarios.
 */
class Asset {
private:
	/* Number of years the funds would last. */
	int fundLongevity_;

	/* Amount of active cash reserve. */
	int cashReserve_;

	/* 2D array for the dynamic growth rate for each year and each account. */
	std::array<std::array<float, MAX_YEARS>, MAX_ACCOUNTS> growthRate_;

public:
	/* Account names */
	std::array<std::string, MAX_ACCOUNTS> name_;

	/* Number of years before reaching retirement (i.e. job income stops)*/
	int yearsTillRetirement_;

	/* Number of years before withdrawing from tax-deferred investments */
	int yearsTillWithdrawal_;

	/* Number of years before pension income starts */
	int yearsTillPension_;

	/* Current year take-home job income after tax and roth/ira/401k contributions. */
	long int takehomeIncome_;

	/* Current year contribution to Roth */
	long int contributionRoth_;
	
	/* Current year contribution to IRA */
	long int contributionIra_;

	/* Current year contribution to 401k */
	long int contributionR401k_;

	/* Initial estimate of pension income in today's value */
	int pensionEstimate_;

	/* 2D array of pre-distribution value for each year and each account */
	std::array<std::array<long int, MAX_YEARS>, MAX_ACCOUNTS> value_;

	/* 2D array to indicate avaibility for funds in each year */
	std::array<std::array<bool, MAX_YEARS>, MAX_ACCOUNTS> availability_;

	/* Expense vector by year */
	std::array<long int, MAX_YEARS> expense_;

	/* 2D array of distribution amount for each year and each account */
	std::array<std::array<long int, MAX_YEARS>, MAX_ACCOUNTS> distribution_;

	/* Average growth rate vector by account */
	std::array<float, MAX_ACCOUNTS> growthRateAvg_;

	/* Inflation rate vector by year */
	std::array<float, MAX_YEARS> inflation_;

private:
    /**
     * @brief Applies a predefined "year-0 loss" scenario to the growth curve.
     * 
     * @param growth Output array for the scenario growth curve.
     */
	void scenarioPredefinedYear0Loss(std::array<float, MAX_YEARS>& growth);

	/**
     * @brief Applies a randomized recession scenario to the growth curve.
     * 
     * @param growth Output array for the scenario growth curve.
     */
	void scenarioRecessionRandomized(std::array<float, MAX_YEARS>& growth);

public:
    /**
     * @brief Gets the number of years the funds last.
     * 
     * @return Fund longevity in number of years.
     */
	int getFundLongevity();

	/**
     * @brief Gets the current cash reserve.
     * 
     * @return The cash reserve amount.
     */
	int getCashReserve();

	/**
     * @brief Adds a cash reserve by taking fund out of a given investment.
     * 
     * @param cash Amount to withdraw.
     * @param asset_num Index of the asset to draw from.
     * @param n_year Target year of withdrawal.
     * @return true if the reserve was added successfully.
     */
	bool addCashReserve(int cash, int asset_num, int n_year);

	/**
     * @brief Clears the cash reserve value.
     */
	void clearCashReserve();

	/**
     * @brief Populates growth curves based on the selected model option.
     * 
     * @param option Simulation model option to use.
     */
	void populateGrowthCurves(const ModelOption option);

	/**
     * @brief Calculates fund longevity and simulates asset behavior.
     */
	void calculateN();

	/**
	 * @brief Initializes Asset data members using user financial input.
	 *
	 * Populates internal arrays such as value_, growthRateAvg_, and inflation_
	 * based on the UserData struct provided.
	 *
	 * @param user Reference to a UserData object containing user financial parameters.
	 */
	void initializeFromUserData(const UserData& user);

	/**
     * @brief Default constructor.
     */
	Asset();

	/**
     * @brief Default destructor.
     */
	~Asset();

	/**
     * @brief Copy constructor (disallowed).
     */
	Asset(const Asset&) = delete;

	/**
     * @brief Copy assignment operator (disallowed).
     */
	Asset& operator=(const Asset&) = delete;

	/**
     * @brief Move constructor (disallowed).
     */
	Asset(Asset&&) = delete;

	/**
     * @brief Move assignment operator (disallowed).
     */
	Asset& operator=(Asset&&) = delete;
};

#endif /* ASSET_H_ */