/* ============================================================================
 * test_asset.cpp
 *
 *  Description:
 *  ---------------------------------------------------------------------------
 *  Minimum unit tests for the Asset class. 
 *
 *  Created: 	June 2025
 *  Maintainer: Yuping X
 * ============================================================================
 */
#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include "asset.h"
#include "modelRecession.h"

TEST(AssetTest, StandardConstructor) {
    // The standard constructor sets all numerical data members to zeros
    Asset myAsset;
    EXPECT_EQ(myAsset.getFundLongevity(), 0);
    EXPECT_EQ(myAsset.getCashReserve(), 0);

    for (int c = 0; c < MAX_ACCOUNTS; c++) {
        EXPECT_EQ(myAsset.growthRateAvg_[c], 0);
        for (int y = 0; y < MAX_YEARS; y++) {
            EXPECT_EQ(myAsset.value_[c][y], 0);
            EXPECT_EQ(myAsset.distribution_[c][y], 0);
            EXPECT_FALSE(myAsset.availability_[c][y]);
        }
    }
    for (int y = 0; y < MAX_YEARS; y++) {
        EXPECT_EQ(myAsset.expense_[y], 0);
        EXPECT_EQ(myAsset.inflation_[y], 0);
    }
}

TEST(AssetTest, AddCashReserveSuccess) {
    Asset myAsset;
    myAsset.value_[INDIVIDUAL_INDEX][0] = 501;
    // Assume asset INDIVIDUAL_INDEX, year 0 is sufficient
    bool added = myAsset.addCashReserve(500, INDIVIDUAL_INDEX, 0);  
    EXPECT_TRUE(added);
    EXPECT_EQ(myAsset.getCashReserve(), 500);
}

TEST(AssetTest, AddCashReserveFail) {
    Asset myAsset;
    myAsset.value_[INDIVIDUAL_INDEX][0] = 400;
    // Assume asset INDIVIDUAL_INDEX, year 0 is insufficient
    bool added = myAsset.addCashReserve(500, INDIVIDUAL_INDEX, 0);  
    EXPECT_FALSE(added);
}

TEST(AssetTest, ClearCashReserve) {
    Asset myAsset;
    myAsset.value_[INDIVIDUAL_INDEX][0] = 501;
    myAsset.addCashReserve(500, INDIVIDUAL_INDEX, 0);  
    myAsset.clearCashReserve();
    EXPECT_EQ(myAsset.getCashReserve(), 0);
}

TEST(AssetTest, ConstantNoGrowthModel) {
    /* This test assumes no growth, no recession, no inflation, no income */
    Asset myAsset;
    const unsigned int V = 50000;
    const unsigned int EACH_V = V / MAX_ACCOUNTS;
    const unsigned int VALUE[MAX_ACCOUNTS] = {EACH_V, EACH_V, EACH_V, EACH_V};

    myAsset.expense_[0] = 1000;
    const int L_EXPECTED = static_cast<int> (std::min(V / myAsset.expense_[0], (long int) MAX_YEARS));

    for (int c = 0; c < MAX_ACCOUNTS; c++) {
        myAsset.value_[c][0] = VALUE[c];
        myAsset.growthRateAvg_[c] = 0;
        for (int y = 0; y < MAX_YEARS; y++) {
            myAsset.availability_[c][y] = true;
        }
    }
    for (int year = 0; year < MAX_YEARS; year++) {
        myAsset.inflation_[year] = 0;
    }

    myAsset.populateGrowthCurves(ModelOption::CONSTANT);
    myAsset.calculateN();

    EXPECT_EQ(myAsset.getFundLongevity(), L_EXPECTED);
}

TEST(AssetTest, OneYearIncomeNoGrowthModel) {
    /* This test has a simple income, 1 year till retirement,
     * and assumes no growth, no recession */
    Asset myAsset;
    const unsigned int V = 45000;
    const unsigned int EACH_V = V / MAX_ACCOUNTS;
    const unsigned int VALUE[MAX_ACCOUNTS] = {EACH_V, EACH_V, EACH_V, EACH_V};
    const unsigned int X = 5000;
    const unsigned int Y = 1000;
    const unsigned int R = 1;
    myAsset.takehomeIncome_ = X;
    myAsset.yearsTillRetirement_ = R;
    myAsset.expense_[0] = Y;

    const int L_EXPECTED = std::min((V + X * R) / Y, MAX_YEARS);

    int year;
    myAsset.availability_[INDIVIDUAL_INDEX][0] = true;
    myAsset.availability_[ROTH_INDEX][0] = false;
    myAsset.availability_[IRA_INDEX][0] = false;
    myAsset.availability_[R401K_INDEX][0] = false;

    for (int c = 0; c < MAX_ACCOUNTS; c++) {
        myAsset.value_[c][0] = VALUE[c];
        myAsset.growthRateAvg_[c] = 0;
        for (year = R; year < MAX_YEARS; year++) {
            myAsset.availability_[c][year] = true;
        }
    }
    for (int year = 0; year < MAX_YEARS; year++) {
        myAsset.inflation_[year] = 0;
    }

    myAsset.populateGrowthCurves(ModelOption::CONSTANT);
    myAsset.calculateN();

    EXPECT_EQ(myAsset.getFundLongevity(), L_EXPECTED);
}

TEST(AssetTest, MultipleYearIncomeNoGrowthModel) {
    /* This test has a simple income, multiple years till retirement,
     * and assumes no growth, no recession */
    Asset myAsset;
    const unsigned int V = 40000;
    const unsigned int EACH_V = V / MAX_ACCOUNTS;
    const unsigned int VALUE[MAX_ACCOUNTS] = {EACH_V, EACH_V, EACH_V, EACH_V};
    const unsigned int X = 1100;
    const unsigned int Y = 1000;
    const unsigned int R = 4;
    myAsset.takehomeIncome_ = X;
    myAsset.yearsTillRetirement_ = R;
    myAsset.expense_[0] = Y;

    const int L_EXPECTED = std::min((V + X * R) / Y, MAX_YEARS);

    int year;
    for (year = 0; year < R; year++) {
        myAsset.availability_[INDIVIDUAL_INDEX][year] = true;
        myAsset.availability_[ROTH_INDEX][year] = false;
        myAsset.availability_[IRA_INDEX][year] = false;
        myAsset.availability_[R401K_INDEX][year] = false;
    }
    for (int c = 0; c < MAX_ACCOUNTS; c++) {
        myAsset.value_[c][0] = VALUE[c];
        myAsset.growthRateAvg_[c] = 0;
        for (year = R; year < MAX_YEARS; year++) {
            myAsset.availability_[c][year] = true;
        }
    }
    for (year = 0; year < MAX_YEARS; year++) {
        myAsset.inflation_[year] = 0;
    }

    myAsset.populateGrowthCurves(ModelOption::CONSTANT);
    myAsset.calculateN();

    EXPECT_EQ(myAsset.getFundLongevity(), L_EXPECTED);
}

TEST(AssetTest, SimpleInflationNoGrowthModel) {
    /* This test has a simple income, expense, multiple years till
     * retirement, fixed inflation, and assumes no growth, no recession.
     */ 
    Asset myAsset;
    const unsigned int V = 40000;
    const unsigned int VALUE[MAX_ACCOUNTS] = {V, 0, 0, 0};
    const float F = 0.05;
    const unsigned int X = 1100;
    const unsigned int Y = 1000;
    const unsigned int R = 4;

    myAsset.takehomeIncome_ = X;
    myAsset.yearsTillRetirement_ = R;
    myAsset.expense_[0] = Y;
    myAsset.inflation_[0] = F;


    /* The following calculations are based on the formula of finite
     * geometric series: 
     *      sum(x, x(1+f), x(1+f)^2, ..., x(1+f)^(r-1)) = x * [(1+f)^r - 1] / f
     *      where f is inflation, 0 < f < 1 
     *      and r is the number of years to sum over
     * Consider x to be the starting income, r to be number of years earning income,
     * and we get the sum of income.
     * Let T be the total sum of assets and income over the lifetime of the model.
     * Consider y to be the starting expense, L to be the expected longevity, then
     * L is the largest integer that satisfies:
     *      y * [(1+f)^L - 1] / f <= T
     * We then get: L = the integer portion of log[(f * T / y) + 1] / log(1 + f) 
     * */
    const unsigned int T = V + static_cast<int> (X * (std::pow(1 + F, R) - 1) / F);
    unsigned int expectedLongevity = static_cast<unsigned int> (std::log(F * T / Y + 1) / std::log(1+F));
    const unsigned int L_EXPECTED = std::min(expectedLongevity, MAX_YEARS);

    std::cout << "Expected longevity is: " << expectedLongevity << '\n';

    int year;
    for (year = 0; year < R; year++) {
        myAsset.availability_[INDIVIDUAL_INDEX][year] = true;
        myAsset.availability_[ROTH_INDEX][year] = false;
        myAsset.availability_[IRA_INDEX][year] = false;
        myAsset.availability_[R401K_INDEX][year] = false;
    }
    for (int c = 0; c < MAX_ACCOUNTS; c++) {
        myAsset.value_[c][0] = VALUE[c];
        myAsset.growthRateAvg_[c] = 0;
        for (year = R; year < MAX_YEARS; year++) {
            myAsset.availability_[c][year] = true;
        }
    }
    for (int year = 1; year < MAX_YEARS; year++) {
        myAsset.inflation_[year] =  myAsset.inflation_[0];
    }

    myAsset.populateGrowthCurves(ModelOption::CONSTANT);
    myAsset.calculateN();

    EXPECT_EQ(myAsset.getFundLongevity(), L_EXPECTED);
}

TEST(AssetTest, SimpleGrowthNoInflationModel) {
    /* This test has a simple income, expense, multiple years till
     * retirement, simple constant growth, and assumes no inflation,
     * no recession.
     */ 
    Asset myAsset;
    const unsigned int V = 40000;
    const unsigned int value[MAX_ACCOUNTS] = {0, 0, 0, V};
    const float G = 0.1;
    const unsigned int X = 1100;
    const unsigned int Y = 1000;
    const unsigned int R = 4;

    myAsset.takehomeIncome_ = X;
    myAsset.yearsTillRetirement_ = R;
    myAsset.expense_[0] = Y;

    /* The following calculations are based on the formula of finite
     * geometric series: 
     *      sum(v, v(1+g), v(1+g)^2, ..., v(1+g)^(L-1)) = v * [(1+g)^L - 1] / g
     *      where g is a growth rate, 0 < g < 1 
     * Let  x be: annual takehome income, 
     *      y be: annual expense,
     *      r be: # of years till retirement,
     *      v be: starting asset value, 
     *      L be: expected longevity, 
     *      T be: the total sum of assets and income over the lifetime of the model.
     * Since there is no inflation (and no increase to x or y),
     *      T = v * [(1+g)^L - 1] / g + x * r
     * 
     * L is the largest integer under MAX_YEARS that satisfies:
     *      y * L <= T 
     * There is no closed-form solution for L so we find it numerically.
     * */

    int year, total;
    for (year = 0; year < MAX_YEARS + 1; year++) {
        total = static_cast<int> (V * (std::pow(1 + G, year) - 1) / G + X * R);
        if (Y * year > total) {
            break;
        }
    }
    const unsigned int L_EXPECTED = year - 1;

    std::cout << "Expected longevity is: " << L_EXPECTED << '\n';

    for (year = 0; year < R; year++) {
        myAsset.availability_[INDIVIDUAL_INDEX][year] = true;
        myAsset.availability_[ROTH_INDEX][year] = false;
        myAsset.availability_[IRA_INDEX][year] = false;
        myAsset.availability_[R401K_INDEX][year] = false;
    }
    for (int c = 0; c < MAX_ACCOUNTS; c++) {
        myAsset.value_[c][0] = value[c];
        myAsset.growthRateAvg_[c] = G;
        for (year = R; year < MAX_YEARS; year++) {
            myAsset.availability_[c][year] = true;
        }
    }
    for (year = 1; year < MAX_YEARS; year++) {
        myAsset.inflation_[year] =  myAsset.inflation_[0];
    }

    myAsset.populateGrowthCurves(ModelOption::CONSTANT);
    myAsset.calculateN();

    EXPECT_EQ(myAsset.getFundLongevity(), L_EXPECTED);
}

TEST(AssetTest, SimpleGrowthNoInflationModelMaxRange) {
    /* This test has a max-range income, max years till
     * retirement, max constant growth, and assumes no inflation,
     * no recession.
     * Starting asset value and growth rate are also maximum.
     * Expense is a small integer. 
     */ 
    Asset myAsset;
    const unsigned int V = INT_MAX;
    const unsigned int value[MAX_ACCOUNTS] = {0, 0, 0, V};
    const float G = MAX_AVG_GROWTH;
    const unsigned int X = INT_MAX;
    const unsigned int Y = 1;
    const unsigned int R = MAX_YEARS;

    myAsset.takehomeIncome_ = X;
    myAsset.yearsTillRetirement_ = R;
    myAsset.expense_[0] = Y;

    /* The following calculations are based on the formula of finite
     * geometric series: 
     *      sum(v, v(1+g), v(1+g)^2, ..., v(1+g)^(L-1)) = v * [(1+g)^L - 1] / g
     *      where g is a growth rate, 0 < g < 1 
     * Let  x be: annual takehome income, 
     *      y be: annual expense,
     *      r be: # of years till retirement,
     *      v be: starting asset value, 
     *      L be: expected longevity, 
     *      T be: the total sum of assets and income over the lifetime of the model.
     * Since there is no inflation (and no increase to x or y),
     *      T = v * [(1+g)^L - 1] / g + x * r
     * 
     * L is the largest integer under MAX_YEARS that satisfies:
     *      y * L <= T 
     * There is no closed-form solution for L so we find it numerically.
     * */

    int year;
    long int total;
    for (year = 0; year < MAX_YEARS + 1; year++) {
        total = static_cast<long> (V * (std::pow(1 + G, year) - 1) / G + X * R);
        if (static_cast<long>(Y) * year > total) {
            break;
        }
    }
    const unsigned int L_EXPECTED = year - 1;

    std::cout << "Expected longevity is: " << L_EXPECTED << '\n';

    for (year = 0; year < R; year++) {
        myAsset.availability_[INDIVIDUAL_INDEX][year] = true;
        myAsset.availability_[ROTH_INDEX][year] = false;
        myAsset.availability_[IRA_INDEX][year] = false;
        myAsset.availability_[R401K_INDEX][year] = false;
    }
    for (int c = 0; c < MAX_ACCOUNTS; c++) {
        myAsset.value_[c][0] = value[c];
        myAsset.growthRateAvg_[c] = G;
        for (year = R; year < MAX_YEARS; year++) {
            myAsset.availability_[c][year] = true;
        }
    }
    for (year = 1; year < MAX_YEARS; year++) {
        myAsset.inflation_[year] =  myAsset.inflation_[0];
    }

    myAsset.populateGrowthCurves(ModelOption::CONSTANT);
    myAsset.calculateN();

    EXPECT_EQ(myAsset.getFundLongevity(), L_EXPECTED);
}

TEST(AssetTest, MaxLongevityNoGrowthModel) {
    /* This test has a simple income, 1 year till retirement,
     * and assumes no growth, no recession.
     * Max longevity should be reached, and we validate the first
     * and the final years' values and distributions. */
    Asset myAsset;

    const unsigned int X = 5000;    // Income
    const unsigned int Y = X;    // Expense
    const unsigned int R = 1;
    /* Define just-enough starting fund value */
    const unsigned int V = MAX_YEARS * Y - X * R; 
    const unsigned int VALUE[MAX_ACCOUNTS] = {0, 0, 0, V};
    myAsset.takehomeIncome_ = X;
    myAsset.yearsTillRetirement_ = R;
    myAsset.expense_[0] = Y;

    int year;
    myAsset.availability_[INDIVIDUAL_INDEX][0] = true;
    myAsset.availability_[ROTH_INDEX][0] = false;
    myAsset.availability_[IRA_INDEX][0] = false;
    myAsset.availability_[R401K_INDEX][0] = false;

    for (int c = 0; c < MAX_ACCOUNTS; c++) {
        myAsset.value_[c][0] = VALUE[c];
        myAsset.growthRateAvg_[c] = 0;
        for (year = R; year < MAX_YEARS; year++) {
            myAsset.availability_[c][year] = true;
        }
    }
    for (int year = 0; year < MAX_YEARS; year++) {
        myAsset.inflation_[year] = 0;
    }

    myAsset.populateGrowthCurves(ModelOption::CONSTANT);
    myAsset.calculateN();

    EXPECT_EQ(myAsset.getFundLongevity(), MAX_YEARS);
    EXPECT_EQ(myAsset.value_[INDIVIDUAL_INDEX][0], 0);
    EXPECT_EQ(myAsset.value_[ROTH_INDEX][0], 0);
    EXPECT_EQ(myAsset.value_[IRA_INDEX][0], 0);
    EXPECT_EQ(myAsset.value_[R401K_INDEX][0], V);
    EXPECT_EQ(myAsset.distribution_[INDIVIDUAL_INDEX][0], 0);
    EXPECT_EQ(myAsset.distribution_[ROTH_INDEX][0], 0);
    EXPECT_EQ(myAsset.distribution_[IRA_INDEX][0], 0);
    EXPECT_EQ(myAsset.distribution_[R401K_INDEX][0], 0);
    EXPECT_EQ(myAsset.value_[INDIVIDUAL_INDEX][MAX_YEARS-1], 0);
    EXPECT_EQ(myAsset.value_[ROTH_INDEX][MAX_YEARS-1], 0);
    EXPECT_EQ(myAsset.value_[IRA_INDEX][MAX_YEARS-1], 0);
    EXPECT_EQ(myAsset.value_[R401K_INDEX][MAX_YEARS-1], Y);
    EXPECT_EQ(myAsset.distribution_[INDIVIDUAL_INDEX][MAX_YEARS-1], 0);
    EXPECT_EQ(myAsset.distribution_[ROTH_INDEX][MAX_YEARS-1], 0);
    EXPECT_EQ(myAsset.distribution_[IRA_INDEX][MAX_YEARS-1], 0);
    EXPECT_EQ(myAsset.distribution_[R401K_INDEX][MAX_YEARS-1], Y);
}