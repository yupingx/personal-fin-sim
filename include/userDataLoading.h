/* ============================================================================
 * userDataLoading.h
 *
 *  Description:
 *  ---------------------------------------------------------------------------
 *  Declares the UserData structure and the function interface for loading
 *  a user’s financial profile from an INI-formatted configuration file.
 *
 *  Constants:
 *    - INI_FILE: Path to the user data file.
 *
 *  Dependencies:
 *    - constants.h
 *
 *  Created: 	June 2025
 *  Author:     Yuping X
 * ============================================================================
 */


#ifndef USER_DATA_LOADING_H_
#define USER_DATA_LOADING_H_

#include <string>
#include "constants.h"

const std::string USERDATA_DIR = "data/";
const std::string USERDATA_FILE_ENDING = "_profile.ini";

/**
 * @brief Struct to store a user's financial profile information.
 *
 * This struct contains data for all income sources, asset values,
 * growth rates, retirement settings, and inflation assumptions. It is
 * typically populated from a user-supplied `.ini` file.
 */
struct UserData {
    /**
     * @brief Names of investment accounts.
     */
    std::string name[MAX_ACCOUNTS];

    /**
     * @brief Initial dollar values of each account.
     */
    int value[MAX_ACCOUNTS];

    /**
     * @brief Expected average annual growth rate for each account.
     */
    float rate[MAX_ACCOUNTS];

    /**
     * @brief Initial annual living expense in dollars.
     */
    int initialExpense;

    /**
     * @brief Current after-tax annual income (excluding contributions).
     */
    int takehomeIncome;

    /**
     * @brief Annual Roth contribution amount in dollars.
     */
    int contributionRoth;

    /**
     * @brief Annual IRA contribution amount in dollars.
     */
    int contributionIra;

    /**
     * @brief Annual 401(k) contribution amount in dollars.
     */
    int contributionR401k;

    /**
     * @brief Initial inflation rate. Must be in the range of [0, 1)
     */
    float initialInflation;

    /**
     * @brief Number of years until retirement.
     */
    unsigned short yearsTillRetirement;
};


/**
 * @brief Loads user financial settings from an INI-style configuration file.
 *
 * Parses assets, income, expense, and retirement details from the given file.
 *
 * @param user UserData struct to populate.
 * @param filename Path to the INI file containing user profile data.
 */
void loadUserFinancialProfile(UserData& user, const std::string filename);

/**
 * @brief Validates whether the user's financial inputs are within bounds.
 *
 * Checks for values such as non-negative income, valid retirement years, etc.
 *
 * @param user User data to validate.
 * @return true if the data is valid, false otherwise.
 */
bool userDataWithinBounds(const UserData& user);

/**
 * @brief Prints a summary of the user's financial input to stdout.
 *
 * Useful for visual inspection before simulation.
 *
 * @param user User profile to display.
 */
void displayUserInfo(const UserData& user);

#endif /* USER_DATA_LOADING_H_ */