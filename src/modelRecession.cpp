/* ============================================================================
 * modelRecession.cpp
 *
 *  Description:
 *  ---------------------------------------------------------------------------
 *  Implements recession scenario models used in personal finance simulations.
 *  Includes both predefined and randomized recession profiles, which
 *  populate growth rate curves applied to investment assets.
 *
 *  Functions:
 *    - scenarioPredefinedYear0Loss: Hardcoded recession scenario where year-0
 * 		is a recession year.
 *    - scenarioRandom1: Randomly inserts recessions and recoveries with randomness
 *      in timing and severity.
 *    - populateGrowthCurves: Fills out asset-specific growth curves based on
 *      selected profile and average expected return.
 *
 *  Dependencies:
 *    - asset.h
 *    - modelRecession.h
 *    - constants.h
 *
 *  Created: 	June 2025
 *  Author:     Yuping X
 * ============================================================================
 */

#include <cstdlib>
#include <iostream>
#include <random>
#include <chrono>
#include "../include/asset.h"
#include "../include/modelRecession.h"
#include "../include/constants.h"

/* =========================================================================
 * Scenario Definition: A Predefined Worst-case
 * ========================================================================= */
void Asset::scenarioPredefinedYear0Loss(std::array<float, MAX_YEARS>& growth_common) {
	for (int n = 0; n < growth_common.size(); n++) {
		growth_common[n] = RECESSION_YEAR0_LOSS[n];
	}
}

/* =========================================================================
 * Scenario Definition: Randomized based on Predefined Recession Assumptions
 * ========================================================================= */
void Asset::scenarioRecessionRandomized(std::array<float, MAX_YEARS>& growth_common) {
	/* In this profile we assume the first recession year happens in the
	 * next RECESSION_START_MOD years;
	 * Initialize the temporary growth curve to 0's.
	 */

	int rr_years_sum = 0;
	int recession_y, recovery_y;
	int n;

	/* Initialize growth to zero across all years */
	for (int n = 0; n < growth_common.size(); n++) {
		growth_common[n] = 0;
	}

    /* Get a time-based seed using high-resolution clock,
	 * then create, seed, and use the random number generator
	 * for uniformely distributed random integers between 1 and 100
	 */
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();

    std::mt19937 generator(seed);

    std::uniform_int_distribution<int> distribution(RANDOM_NUM_MIN, RANDOM_NUM_MAX);

	/* Generate the recession start year */
	recession_y =  distribution(generator) % RECESSION_START_MOD;
	n = recession_y;

	/* First fill out the recession and recovery years. */
	while (n < growth_common.size()) {
		/* Generate the negative growth rate (recession_rate) for the recession year */
		float recession_range = RECESSION_MAX - RECESSION_MIN;
		float rand_normalized = float(distribution(generator)) / RANDOM_NUM_MAX;
		float recession_rate = RECESSION_MIN + rand_normalized * recession_range;

		/* Calculate the recovery half rebound */
		float half_rebound = - recession_rate/2;
	
		if (DEBUG_PRINT) {
			std::cout << "DEBUG: recession rate: " << recession_rate << std::endl;
		}
		growth_common[n] = recession_rate;
		rr_years_sum++;

		int recovery_y_range = RECOVERY_INT_MAX - RECOVERY_INT_MIN;
		/* Generate a random recovery year */
		recovery_y = RECOVERY_INT_MIN + distribution(generator) % recovery_y_range;
		if ((n + recovery_y) >= MAX_YEARS) {
			break;
		}
		n += recovery_y;
		growth_common[n] = half_rebound;
		rr_years_sum++;

		if ((n + 1) >= MAX_YEARS) {
			break;
		}
		n++;
		growth_common[n] = half_rebound;
		rr_years_sum++;

		/* Generate the next random recession year */
		int recession_y_range = RECESSION_INT_MAX - RECESSION_INT_MIN;
		recession_y = RECESSION_INT_MIN + distribution(generator) % recession_y_range;
		n += recession_y;

	}

	/* Next we fill out the remaining (non-recession, non-recovery)
	 * years while maintaining the correct average growth.
	 * Basically the remaining years have a higher average to compensate
	 * for the recessions.
	 */
	float remaining_growth_avg = (STOCK_GROWTH_AVG * MAX_YEARS) / (MAX_YEARS - rr_years_sum);

	for (n = 0; n < growth_common.size(); n++) {

		if (growth_common[n] == 0) {
			growth_common[n] = (remaining_growth_avg - STOCK_AVG_SPAN_HALF) + \
							 float(distribution(generator)) / RANDOM_NUM_MAX * STOCK_AVG_SPAN;
		}
		if (DEBUG_PRINT) {
			int year = CURRENT_YEAR + n;
			float growth = growth_common[n];
			std::cout << "Year " << year << "  growth: " << growth << std::endl;
		}
	}
}

/* =========================================================================
 * Populate Growth Curves Based on Growth Profile
 * ========================================================================= */
void Asset::populateGrowthCurves(const ModelOption option) {

	std::array<float, MAX_YEARS> growth_common{};

	switch (option) {
		case ModelOption::CONSTANT:
			for (int c = 0; c < MAX_ACCOUNTS; c++) {
				for (int n = 0; n < MAX_YEARS; n++) {
					Asset::growthRate_[c][n] = Asset::growthRateAvg_[c];
				}
			}
			break;

		case ModelOption::PREDEFINED_YEAR0_LOSS:
			Asset::scenarioPredefinedYear0Loss(growth_common);
			break;

		case ModelOption::RECESSION_RANDOMIZED:
			Asset::scenarioRecessionRandomized(growth_common);
			break;

		default:
			std::cerr << "ERROR: Fund growth moodel option not recognized!" << std::endl;

	}
	if (option != ModelOption::CONSTANT) {
		/* Unless using the "constant" model, we have a last step:
		 * We populate the growth curves for each investment item.
		 * We guess a ratio of the stock in the investment, and multiply the generic growth curve
		 * generated above with this approximate ratio.
		 */
		for (int c = 0; c < MAX_ACCOUNTS; c++) {
			float guessed_stock_ratio = Asset::growthRateAvg_[c] / STOCK_GROWTH_AVG;
			/* Ensure the ratio is 1.0 at maximum */
			guessed_stock_ratio = (guessed_stock_ratio <= 1.0)? guessed_stock_ratio : 1.0;
			for (int n = 0; n < growth_common.size(); n++) {
				Asset::growthRate_[c][n] = growth_common[n] * guessed_stock_ratio;
			}
		}
	}
}
