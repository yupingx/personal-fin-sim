/* ============================================================================
 * clparser.h
 *
 *  Description:
 *  ---------------------------------------------------------------------------
 *  Declares the ClArgs structure and the function interface for parsing
 *  a user’s command line arguments.
 *
 *  Dependencies:
 *    - userDataLoading.h
 *
 *  Created: 	June 2025
 *  Author:     Yuping X
 * ============================================================================
 */

#ifndef CLPARSER_H_
#define CLPARSER_H_
#include <string>
#include "../include/userDataLoading.h"

/* If left unspecified by the user, default userdata profile is
 * demo_profile.ini 
 */
struct ClArgs {
    std::string filename = USERDATA_DIR + "demo" + USERDATA_FILE_ENDING;;
};

/**
 * @brief Parses command-line arguments and updates configuration parameters.
 *
 * @param params Reference to the command-line parameter struct to populate.
 * @param argc Argument count from `main()`.
 * @param argv Argument vector from `main()`.
 */
void clArgParser(ClArgs& params, int argc, char** argv);

#endif /* CLPARSER_H_ */