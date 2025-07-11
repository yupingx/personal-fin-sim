/* ============================================================================
 * main.cpp
 *
 * Entry point for the Personal Finance Simulator application.
 *
 * This file handles:
 *   - Parsing command-line arguments (via clArgParser)
 *   - Loading user financial profile from file
 *   - Validating input data
 *   - Invoking simulations across all supported models
 *
 * Dependencies:
 *   - clparser.h        (Command-line argument parsing)
 *   - userDataLoading.h (INI file loading and validation)
 *   - asset.h           (Simulation model and asset logic)
 *
 *  Created: 	June 2025
 *  Author:     Yuping X
 * ============================================================================
 */
#include <iostream>
#include <memory>
#include "../include/clparser.h"
#include "../include/userDataLoading.h"
#include "../include/asset.h"

void runSimAll(const UserData& user);

int main(int argc, char **argv) {

    std::unique_ptr<ClArgs> params = std::make_unique<ClArgs>();

    clArgParser(*params, argc, argv);

    std::unique_ptr<UserData> user = std::make_unique<UserData>();

    /* Load asset & financial settings */
    loadUserFinancialProfile(*user, params->filename); 
    
    if (!userDataWithinBounds(*user)) {
        return 1; // Exit with error
    }
    
    displayUserInfo(*user);

    runSimAll(*user);

    return 0;
}
