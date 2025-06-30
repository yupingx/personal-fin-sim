/* ============================================================================
 * clparser.cpp
 *
 *  Description:
 *  ---------------------------------------------------------------------------
 *  Displays welcome messages and parses user's command line arguments.
 *
 *  Dependencies:
 *    - userDataLoading.h
 *    - C++ STL (iostream, filesystem, string)
 *
 *  Usage Context:
 *    First things called by main() before simulation initialization.
 *
 *  Created:    June 2025
 *  Author:     Yuping X
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <filesystem>
#include <regex>
#include "../include/clparser.h"
#include "../include/userDataLoading.h"

static void displayWelcomeMsg() {
    std::cout << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << "       Personal Finance Simulator v0.1"           << std::endl;
    std::cout << "   (for personal use only; no advice implied)"    << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: ./build/pfsim --user <name>" << std::endl;
    std::cout << "Example: ./build/pfsim --user demo" << std::endl;
    std::cout << "See README for more information such as adding " << std::endl;
    std::cout << "your own user profile." << std::endl;
    std::cout << std::endl;
}

void clArgParser(ClArgs& params, int argc, char** argv) {
    displayWelcomeMsg();

    std::regex valid_user_regex("^[a-zA-Z0-9-_]+$");

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if ((arg == "--user") && (i+1 < argc)) {
            if (!std::regex_match(argv[++i], valid_user_regex)) {
                std::cerr << "ERROR: Invalid user name. " \
                          << "Only letters, digits, dashes (-), and underscores (_) allowed." \
                          << std::endl;
                exit(1);
            }
            params.filename = USERDATA_DIR + argv[i] + USERDATA_FILE_ENDING;
        }
        else if (arg == "--help") {
            displayWelcomeMsg();
            exit(0);
        }
        else {
            std::cerr << "Unknown option: " << arg << std::endl;
            exit(1);
        }
    }

    if (!std::filesystem::exists(params.filename)) {
        std::cerr << "File " << params.filename << " not found! " \
                  << "Check spelling or create file and try again." \
                  << std::endl;
        exit(1); // Exit with error
    }
}