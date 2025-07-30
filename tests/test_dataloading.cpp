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
#include <fstream>
#include "userDataLoading.h"

TEST(UserDataLoadingTest, OutOfRangeLargeValue) {
    // Create a temporary .ini file with a known bad line
    const std::string TESTFILE = "invalid_profile.ini";
    std::ofstream fout(TESTFILE);
    ASSERT_TRUE(fout.is_open());

    fout << "[Assets]\n";
    fout << "LargeAccount=2147483648,0.07\n";  // Larger than INT_MAX
    fout.close();

    UserData user;
    try {
        loadUserFinancialProfile(user, TESTFILE);
        FAIL() << "Expected exception not thrown";
    } catch (const std::runtime_error& e) {
        EXPECT_TRUE(std::string(e.what()).find("Out-of-range") != std::string::npos);
    }

    /* Simple clean up */
    std::remove(TESTFILE.c_str());
}

TEST(UserDataLoadingTest, MissingDataField) {
    // Create a temporary .ini file with a known bad line
    const std::string TESTFILE = "invalid_profile.ini";
    std::ofstream fout(TESTFILE);
    ASSERT_TRUE(fout.is_open());

    fout << "[Assets]\n";
    fout << "BadlyFormattedAccount=21.07\n";  // Bad format, missing data field
    fout.close();

    UserData user;
    try {
        loadUserFinancialProfile(user, TESTFILE);
        FAIL() << "Expected exception not thrown";
    } catch (const std::runtime_error& e) {
        EXPECT_TRUE(std::string(e.what()).find("Invalid format") != std::string::npos);
    }

    /* Simple clean up */
    std::remove(TESTFILE.c_str());
}

TEST(UserDataLoadingTest, UnknownKeyInGeneralSection) {
    // Create a temporary .ini file with a known bad line
    const std::string TESTFILE = "invalid_profile.ini";
    std::ofstream fout(TESTFILE);
    ASSERT_TRUE(fout.is_open());

    fout << "[General]\n";
    fout << "Cost-of-liv=1000\n";  // Typo in key name
    fout.close();

    UserData user;
    try {
        loadUserFinancialProfile(user, TESTFILE);
        FAIL() << "Expected exception not thrown";
    } catch (const std::runtime_error& e) {
        EXPECT_TRUE(std::string(e.what()).find("Unknown key in General section") != std::string::npos);
    }

    /* Simple clean up */
    std::remove(TESTFILE.c_str());
}

TEST(UserDataLoadingTest, CannotParseKeyValueInGeneralSection) {
    // Create a temporary .ini file with a known bad line
    const std::string TESTFILE = "invalid_profile.ini";
    std::ofstream fout(TESTFILE);
    ASSERT_TRUE(fout.is_open());

    fout << "[General]\n";
    fout << "Cost-of-living=a\n";  // Value cannot be parsed as an integer
    fout.close();

    UserData user;
    try {
        loadUserFinancialProfile(user, TESTFILE);
        FAIL() << "Expected exception not thrown";
    } catch (const std::runtime_error& e) {
        EXPECT_TRUE(std::string(e.what()).find("Error parsing value") != std::string::npos);
    }

    /* Simple clean up */
    std::remove(TESTFILE.c_str());
}

TEST(UserDataLoadingTest, BoundsCheckSuccess) {
    // Create a temporary .ini file with all valid data
    const std::string TESTFILE = "valid_profile.ini";
    std::ofstream fout(TESTFILE);
    ASSERT_TRUE(fout.is_open());

    fout << "[Assets]\n";
    fout << "Individual=1000,0.1\n";  // Valid
    fout << "Individual_roth=2000,0.2\n";  // Valid
    fout << "Individual_ira=3000,0.3\n";   // Valid
    fout << "Individual_401k=4000,0.3\n";  // Valid
    fout << "[General]\n";
    fout << "Cost-of-living=1000\n";  // Valid
    fout << "Current-annual-takehome-income=50000\n";  // Valid
    fout << "Current-annual-roth-contribution=6000\n";  // Valid
    fout << "Current-annual-ira-contribution=6000\n";  // Valid
    fout << "Current-annual-r401k-contribution=19500\n";  // Valid
    fout << "Pension-estimate=20000\n";  // Valid
    fout << "Inflation=0.02\n";  // Valid
    fout << "Years-till-retirement=30\n";  // Valid
    fout << "Years-till-withdrawal=5\n";  // Valid
    fout << "Years-till-pension=10\n";  // Valid

    fout.close();

    UserData user;
    try {
        loadUserFinancialProfile(user, TESTFILE);
    } catch (const std::runtime_error& e) {
        FAIL() << "No exception expected here";
    }

    /* No problem expected by userDataWithinBounds() */
    EXPECT_TRUE(userDataWithinBounds(user));

    /* Simple clean up */
    std::remove(TESTFILE.c_str());
}

TEST(UserDataLoadingTest, BoundsCheckNegativeValue) {
    /* Create a temporary .ini file with all valid data except one line with
     * a negative value */

    const std::string TESTFILE = "invalid_profile.ini";
    std::ofstream fout(TESTFILE);
    ASSERT_TRUE(fout.is_open());

    fout << "[Assets]\n";
    fout << "Individual=-1000,0.1\n";  // Negative value
    fout << "Individual_roth=2000,0.2\n";  // Valid
    fout << "Individual_ira=3000,0.3\n";   // Valid
    fout << "Individual_401k=4000,0.3\n";  // Valid
    fout << "[General]\n";
    fout << "Cost-of-living=1000\n";  // Valid
    fout << "Current-annual-takehome-income=50000\n";  // Valid
    fout << "Current-annual-roth-contribution=6000\n";  // Valid
    fout << "Current-annual-ira-contribution=6000\n";  // Valid
    fout << "Current-annual-r401k-contribution=19500\n";  // Valid
    fout << "Pension-estimate=20000\n";  // Valid
    fout << "Inflation=0.02\n";  // Valid
    fout << "Years-till-retirement=30\n";  // Valid
    fout << "Years-till-withdrawal=5\n";  // Valid
    fout << "Years-till-pension=10\n";  // Valid

    fout.close();

    UserData user;
    try {
        loadUserFinancialProfile(user, TESTFILE);
    } catch (const std::runtime_error& e) {
        FAIL() << "No exception expected here";
    }

    /* Negative value should be caught by userDataWithinBounds() */
    EXPECT_FALSE(userDataWithinBounds(user));

    /* Simple clean up */
    std::remove(TESTFILE.c_str());
}

TEST(UserDataLoadingTest, BoundsCheckLargeValue) {
    /* Create a temporary .ini file with all valid data except one line with
     * a too-large value */

    const std::string TESTFILE = "invalid_profile.ini";
    std::ofstream fout(TESTFILE);
    ASSERT_TRUE(fout.is_open());

    fout << "[Assets]\n";
    fout << "Individual=1000,0.1\n";  // Valid
    fout << "Individual_roth=2000,0.2\n";  // Valid
    fout << "Individual_ira=3000,0.3\n";   // Valid
    fout << "Individual_401k=4000,0.3\n";  // Valid
    fout << "[General]\n";
    fout << "Cost-of-living=1000\n";  // Valid
    fout << "Current-annual-takehome-income=50000\n";  // Valid
    /* Value too large*/
    fout << "Current-annual-roth-contribution=" << (MAX_ROTH_CONTRIBUTION+1) << "\n";
    fout << "Current-annual-ira-contribution=6000\n";  // Valid
    fout << "Current-annual-r401k-contribution=19500\n";  // Valid
    fout << "Pension-estimate=20000\n";  // Valid
    fout << "Inflation=0.02\n";  // Valid
    fout << "Years-till-retirement=30\n";  // Valid
    fout << "Years-till-withdrawal=5\n";  // Valid
    fout << "Years-till-pension=10\n";  // Valid

    fout.close();

    UserData user;
    try {
        loadUserFinancialProfile(user, TESTFILE);
    } catch (const std::runtime_error& e) {
        FAIL() << "No exception expected here";
    }

    /* Too-large value should be caught by userDataWithinBounds() */
    EXPECT_FALSE(userDataWithinBounds(user));

    /* Simple clean up */
    std::remove(TESTFILE.c_str());
}