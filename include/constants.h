/* ============================================================================
 * constants.h
 *
 *  Description:
 *  ---------------------------------------------------------------------------
 *  Common constants for user data, account types, and simulation parameters
 *
 *  Created: 	June 2025
 *  Author:     Yuping X
 * ============================================================================
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

/* Compile-time option for enabling debug prints or not */
#define DEBUG_PRINT			0

/**
 * @brief Number of total interations run for randomized model simulation.
 */
const unsigned int ITERATIONS = 1000;

/**
 * @brief Bin width (in number of years).
 * 
 * This is used for grouping results from randomized model simulation.
 */
const unsigned int RESULT_BINS_WIDTH = 10;

/**
 * @brief Max number of years over which the simulator runs projections.
 */
const unsigned int MAX_YEARS = 50;

/**
 * @brief The years from which the simulator starts running projections.
 */
const unsigned int CURRENT_YEAR = 2025;

/**
 * @brief MAX_ROTH_CONTRIBUTION for the starting year (year 0).
 */
const unsigned int MAX_ROTH_CONTRIBUTION = 100000;

/**
 * @brief MAX_IRA_CONTRIBUTION for the starting year (year 0).
 */
const unsigned int MAX_IRA_CONTRIBUTION = 100000;

/**
 * @brief MAX_R401K_CONTRIBUTION for the starting year (year 0).
 */
const unsigned int MAX_R401K_CONTRIBUTION = 100000;

/**
 * @brief Maximum average inflation rate (over all future years).
 * 
 * This is the maximum rate the user could specify.
 */
const float MAX_AVG_INFLATION = 0.3;

/**
 * @brief Maximum average growth rate of an investment (over all future years).
 * 
 * This is the maximum rate the user could specify.
 */
const float MAX_AVG_GROWTH = 0.3;

/**
 * @brief Max number of Asset account types.
 *
 * As of v0.1, this simulator only supports 4 Asset account types. Please
 * DO NOT EDIT.
 */
const unsigned int MAX_ACCOUNTS = 4;

/**
 * There is some starting code for the cash reserve logic but it's not fully
 * implemented yet in v0.1. `CASH_RESERVE` is set to be `0` and please
 * DO NOT EDIT.
 */
const unsigned int CASH_RESERVE = 0;

/**
 * @brief enum for Asset account types
 * 
 * These are also used for indexes into Asset data member arrays, so they must
 * start from 0 and be consecutive.
 */
enum class AccountType {
	INDIVIDUAL = 0,
	ROTH = 1,
	IRA = 2,
	R401K = 3
};

/**
 * @brief Aliases for Asset account types (used as indexes into data arrays)
 */
constexpr int INDIVIDUAL_INDEX = static_cast<int>(AccountType::INDIVIDUAL);
constexpr int ROTH_INDEX = static_cast<int>(AccountType::ROTH);
constexpr int IRA_INDEX = static_cast<int>(AccountType::IRA);
constexpr int R401K_INDEX = static_cast<int>(AccountType::R401K);

#endif /* CONSTANTS_H_ */