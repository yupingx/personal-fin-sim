/* ============================================================================
 * userDataLoading.cpp
 *
 *  Description:
 *  ---------------------------------------------------------------------------
 *  Implements routines for reading and parsing user financial profile data
 *  from an INI-formatted input file. Extracted data includes asset names,
 *  values, growth rates, and general financial planning parameters such as
 *  cost-of-living, inflation, and years until retirement.
 *
 *  Key Function:
 *    - loadUserFinancialProfile: Populates the UserData structure from file
 *      input, organizing parameters by section (e.g., [Assets], [General]).
 *    - userDataWithinBounds: Check if user data is within bounds.
 *    - displayUserInfo: Prints loaded data for inspection
 *
 *  Dependencies:
 *    - userDataLoading.h
 *    - constants.h
 *    - C++ STL (iostream, fstream, sstream, string)
 *
 *  Usage Context:
 *    Called during simulation initialization to prepare user-specific
 *    configuration data for use by the Asset class.
 *
 *  Created:    June 2025
 *  Author:     Yuping X
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>
#include "../include/userDataLoading.h"
#include "../include/constants.h"

/* Helper function to trim blank spaces at the beginning and end of a string */
static std::string_view trim(const std::string_view s) {
    size_t start = 0;
    size_t end = s.size() - 1;
    while ((start < end) && (s[start] == ' ')) {
            start++;
    }

    while ((end > start) && (s[end] == ' '))  {
        end--;
    }
    return(s.substr(start, end - start + 1));
}

/* Reads user data from INI file format. */
void loadUserFinancialProfile(UserData& user, const std::string filename) {
    std::ifstream file;
    file.open(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open INI file!" << std::endl;
        return;
    }

    std::cout << "Loading user data from file " << filename << "...\n" << std::endl;

    std::string line, section;

    short index = 0;

    /* Map for General section key-to-action mapping */
    const std::unordered_map<std::string, std::function<void(const std::string&)>> generalHandlers = {
        {"Cost-of-living",              [&](const std::string& v) { user.initialExpense = stoi(v); }},
        {"Current-annual-takehome-income", [&](const std::string& v) { user.takehomeIncome = stoi(v); }},
        {"Current-annual-roth-contribution", [&](const std::string& v) { user.contributionRoth = stoi(v); }},
        {"Current-annual-ira-contribution",  [&](const std::string& v) { user.contributionIra = stoi(v); }},
        {"Current-annual-r401k-contribution", [&](const std::string& v) { user.contributionR401k = stoi(v); }},
        {"Pension-estimate",            [&](const std::string& v) { user.pensionEstimate = stoi(v); }},
        {"Inflation",                   [&](const std::string& v) { user.initialInflation = stof(v); }},
        {"Years-till-retirement",       [&](const std::string& v) { user.yearsTillRetirement = static_cast<unsigned short>(stoi(v)); }},
        {"Years-till-withdrawal",       [&](const std::string& v) { user.yearsTillWithdrawal = static_cast<unsigned short>(stoi(v)); }},
        {"Years-till-pension",          [&](const std::string& v) { user.yearsTillPension = static_cast<unsigned short>(stoi(v)); }}
    };

    while (getline(file, line)) {
        line = line.substr(0, line.find(';')); // Ignore comments
        if (line.empty()) continue;

        if (line[0] == '[' && line.back() == ']') {
            section = line.substr(1, line.length() - 2);
        } else {
            std::stringstream ss(line);
            std::string key, value1, value2;
            getline(ss, key, '=');
            key = trim(key);

            try {
                if ((section == "Assets") && index < MAX_ACCOUNTS) {
                    getline(ss, value1, ',');
                    getline(ss, value2);
                    user.name[index] = key;
                    user.value[index] = stoi(value1);
                    user.rate[index++] = stof(value2);

                } else if (section == "General") {
                    getline(ss, value1, ',');
                    auto it = generalHandlers.find(key);
                    if (it != generalHandlers.end()) {
                        try {
                            it->second(value1);
                        } catch (const std::exception& e) {
                            throw std::runtime_error("Error parsing value for '" + key + "': " + e.what());
                        }
                    }
                    else {
                        throw std::runtime_error("Unknown key in General section: " + key);
                    }
                }
            } catch (const std::invalid_argument& e) {
                throw std::runtime_error("Invalid format on line '" + line + "': " + e.what());
            } catch (const std::out_of_range& e) {
                throw std::runtime_error("Out-of-range number on line '" + line + "': " + e.what());
            }
            
        }
    }
    file.close();
}

/* Check if any numeric field of UserData is out of bounds 
 * Returns true if all data is within bounds,
 * false otherwise. 
 * */
bool userDataWithinBounds(const UserData& user) {
    /* Initialize a count for the number of out-of-bounds data identified */
    unsigned int outOfBounds = 0;
    for (int c = 0; c < MAX_ACCOUNTS; c++) {
        if (user.value[c] < 0) {
            outOfBounds++;
            std::cerr << "ERROR: starting value for " << user.name[c] \
                      << " must be non-negative" \
                      << std::endl;
        }
        if ((user.rate[c] < 0) || (user.rate[c] > MAX_AVG_GROWTH)) {
            outOfBounds++;
            std::cerr << "ERROR: growth rate for " << user.name[c] \
                      << " must be within [0, " << MAX_AVG_GROWTH \
                      << "]" << std::endl;
        }
    }
    if (user.initialExpense < 0) {
        outOfBounds++;
        std::cerr << "ERROR: current annual expense must be non-negative" \
                  << std::endl;
    }
    if (user.takehomeIncome < 0) {
        outOfBounds++;
        std::cerr << "ERROR: current takehome income must be non-negative" \
                  << std::endl;
    }
    if ((user.contributionRoth < 0) || (user.contributionRoth > MAX_ROTH_CONTRIBUTION)) {
        outOfBounds++;
        std::cerr << "ERROR: Roth contribution " \
                    << " must be non-negative and less than " << MAX_ROTH_CONTRIBUTION \
                    << std::endl;
    }
    if ((user.contributionIra < 0) || (user.contributionIra > MAX_IRA_CONTRIBUTION)) {
        outOfBounds++;
        std::cerr << "ERROR: IRA contribution " \
                    << " must be non-negative and less than " << MAX_IRA_CONTRIBUTION \
                    << std::endl;
    }
    if ((user.contributionR401k < 0) || (user.contributionR401k > MAX_R401K_CONTRIBUTION)) {
        outOfBounds++;
        std::cerr << "ERROR: IRA contribution " \
                    << " must be non-negative and less than " << MAX_R401K_CONTRIBUTION \
                    << std::endl;
    }
    if (user.pensionEstimate < 0) {
        outOfBounds++;
        std::cerr << "ERROR: pension estimate must be non-negative" \
                  << std::endl;
    }
    if ((user.initialInflation < 0) || (user.initialInflation > MAX_AVG_INFLATION)) {
        outOfBounds++;
        std::cerr << "ERROR: inflation must be within [0, " << MAX_AVG_INFLATION \
                  << "]" << std::endl;
    }
    if ((user.yearsTillRetirement < 0) || (user.yearsTillRetirement > MAX_YEARS)) {
        outOfBounds++;
        std::cerr << "ERROR: years till retirement must be within [0, " << MAX_YEARS \
                  << "]" << std::endl;
    }
    if ((user.yearsTillWithdrawal < 0) || (user.yearsTillWithdrawal > MAX_YEARS)) {
        outOfBounds++;
        std::cerr << "ERROR: years till withdrawal must be within [0, " << MAX_YEARS \
                  << "]" << std::endl;
    }
    if ((user.yearsTillPension < 0) || (user.yearsTillPension > MAX_YEARS)) {
        outOfBounds++;
        std::cerr << "ERROR: years till pension must be within [0, " << MAX_YEARS \
                  << "]" << std::endl;
    }
    if (outOfBounds) {
        std::cout << "Please correct these " << outOfBounds \
                  << " out-of-bounds number(s) in your user_profile.ini file." << std::endl;
    }

    return(outOfBounds==0);
}

/* Print loaded data for inspection */
void displayUserInfo(const UserData& user) {
    std::cout << "===========================================================" << std::endl;
    std::cout << "User's general financial settings:" << std::endl;
    std::cout << "Takehome Income (after tax, 401k and roth contributions): $" \
              << user.takehomeIncome << std::endl;
    std::cout << "Annual Roth contribution: $" << user.contributionRoth  << std::endl;
    std::cout << "Annual IRA contribution: $"  << user.contributionIra   << std::endl;
    std::cout << "Annual 401k contribution: $" << user.contributionR401k << std::endl;
    std::cout << "Expense: $" << user.initialExpense << std::endl;
    std::cout << "Inflation Rate: " << user.initialInflation << std::endl;
    std::cout << "Pension Estimate: $" << user.pensionEstimate << std::endl;
    std::cout << "Years till retirement: " << user.yearsTillRetirement << std::endl;
    std::cout << "Years till withdrawal: " << user.yearsTillWithdrawal << std::endl;
    std::cout << "Years till pension: " << user.yearsTillPension << std::endl;

    std::cout << "\nUser's Asset Data:" << std::endl;
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        std::cout << "Asset: " << user.name[i]
                  << ", Initial Value: $" << user.value[i]
                  << ", Growth Rate: " << user.rate[i] << std::endl;
    }
    std::cout << "===========================================================" << std::endl;
}