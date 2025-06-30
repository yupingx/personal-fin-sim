/* ============================================================================
 * modelRecession.h
 *
 *  Description:
 *  ---------------------------------------------------------------------------
 *  Constants used in modeling recession and recovery. 
 *  To be included by modelRecession.cpp
 *
 *  Created: 	June 2025
 *  Author:     Yuping X
 * ============================================================================
 */


#ifndef RECESSIONMODEL_H_
#define RECESSIONMODEL_H_

#include <array>

enum class ModelOption : int {
	/* Constant growth model*/
	CONSTANT = 0,

	/* A predefined worst-case scenario growth model */
	PREDEFINED_YEAR0_LOSS = 1,

	/* A randomized growth model based on recession assumptions */
	RECESSION_RANDOMIZED = 2,
	
	MIN = CONSTANT,
	MAX = RECESSION_RANDOMIZED
};

/* The average yearly return of the S&P 500 is 9% over the last 30 years,
 * 11.3% over the last 10 years.
 * This assumes dividends are reinvested.
 * Further assumptions:
 * - Recession occurs every 8 - 11 years after the last recovery, with uniform
 * 		distribution
 * - In the recession year, growth is negative and between -0.15 to 0.45 with a
 * 		uniform distribution
 * - Recession recovery is between 1 - 4 years, with uniform distribution
 * - In the recovery year and next year, the growth is the mirror of the recession
 * 		year divided by 2
 */
const float STOCK_GROWTH_AVG = 0.113;
const float STOCK_AVG_SPAN = 0.2;
const float STOCK_AVG_SPAN_HALF	= STOCK_AVG_SPAN / 2;
const float STOCK_GROWTH_AVG_MIN = STOCK_GROWTH_AVG - STOCK_AVG_SPAN / 2;
const float STOCK_GROWTH_AVG_MAX = STOCK_GROWTH_AVG + STOCK_AVG_SPAN / 2;
const float RECESSION_MIN = -0.45;
const float RECESSION_MAX = -0.15;

const unsigned int RECESSION_START_MOD = 1;
const unsigned int RECESSION_INT_MIN = 8;
const unsigned int RECESSION_INT_MAX = 11;
const unsigned int RECOVERY_INT_MIN = 1;
const unsigned int RECOVERY_INT_MAX = 4;

const std::array<float, MAX_YEARS> RECESSION_YEAR0_LOSS = \
                        {-0.426, 0.1187, 0.213, 0.213, 0.0906842,               \
                         0.232684, 0.242684, 0.132684, 0.146684, 0.114684,      \
                         0.0826842, 0.110684, -0.237, 0.1185, 0.1185,           \
                         0.0926842, 0.176684, 0.138684, 0.200684, 0.218684,     \
                         0.140684, 0.122684, 0.0726842, 0.0706842, 0.220684,    \
                         0.190684, -0.312, 0.156, 0.156, 0.144684,              \
                         0.0786842, 0.150684, 0.148684, 0.136684, 0.218684,     \
                         0.0526842, 0.236684, -0.321, 0.136684, 0.1605,         \
                         0.1605, 0.246684, 0.232684, 0.240684, 0.124684,        \
                         0.0926842, 0.242684, 0.236684, 0.130684, 0.216684};

/* constants for random number generation */
const unsigned int RANDOM_NUM_MIN = 1;
const unsigned int RANDOM_NUM_MAX = 100;

#endif /* RECESSIONMODEL_H_ */
