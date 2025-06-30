/* =============================================================================
 * personalFinSim.cpp
 *
 * Core simulation driver for the Personal Finance Simulator.
 *
 * This file defines the main simulation control functions, including:
 *   - Running simulations for different financial models
 *   - Aggregating and displaying fund longevity statistics
 *
 * Simulation modes include:
 *   - Constant growth
 *   - Predefined year-0 loss
 *   - Randomized recession modeling
 *
 *  Created: 	June 2025
 *  Author:     Yuping X
 * ============================================================================
 */
#include <iostream>
#include <unordered_map>
#include <string.h>
#include <array>
#include "../include/asset.h"
#include "../include/constants.h"
#include "../include/modelRecession.h"
#include "../include/userDataLoading.h"

const std::unordered_map<ModelOption, std::string> modelOptionMap {
    {ModelOption::CONSTANT,                 "Constant growth model"},
    {ModelOption::PREDEFINED_YEAR0_LOSS,    "Predefined year-0 loss model"},
    {ModelOption::RECESSION_RANDOMIZED,     "Randomized model"}
};

/**
 * @brief Helper function to group iterated simulation results into bins.
 *  .
 *
 * @param results Array of longevity data from all iterations in a simulation.
 */
static void groupResultsAndDisplay(const std::array<int, ITERATIONS>& results) {
    const unsigned int BINS_COUNT = MAX_YEARS / RESULT_BINS_WIDTH + 1;

    /* Binned results count */
    std::array<unsigned int, BINS_COUNT> resultsBins = {0};  

    int bin_index = 0;
    for (int iter = 0; iter < ITERATIONS; iter++) {
        bin_index = results[iter] / RESULT_BINS_WIDTH;
        resultsBins[bin_index]++;
    }

    float resultsBinsPct;
    /* Output results summary */
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Randomized recession simulation summary" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Fund longevity statistics across " << ITERATIONS << " simulations:" << std::endl;

    for (int b = 0; b < BINS_COUNT - 1; b++) {
        resultsBinsPct = float(resultsBins[b]) / ITERATIONS * 100;
        std::cout << RESULT_BINS_WIDTH*b << " - " << RESULT_BINS_WIDTH*(b+1)-1 \
                  << " years: " << resultsBins[b] << " runs (" << resultsBinsPct << "%)" \
                  << std::endl;
    }

    /* Last bin printed separately */
    resultsBinsPct = float(resultsBins[BINS_COUNT-1]) / ITERATIONS * 100;
    std::cout << ">= " << (BINS_COUNT-1) * RESULT_BINS_WIDTH \
              << " years: " << resultsBins[BINS_COUNT-1] << " runs (" << resultsBinsPct << "%)" \
              << std::endl;
}

/**
 * @brief Runs a simulation of fund longevity based on the given model option.
 *
 * Initializes an Asset object using the provided user data and executes
 * the simulation using the specified growth model strategy. If the model
 * is randomized, the function runs multiple iterations and summarizes results
 * into bins. Otherwise, it runs a single deterministic simulation.
 *
 * @param user The user data struct for financial information and parameters.
 * @param option The simulation model to use.
 */
static void runSim(const UserData& user, ModelOption option) {
    Asset myAsset;

    /* Simulation iterations for investment modeling */
    int num_iterations = 1;
    
    /* Result bins for categorizing fund longevity */
    std::array<int, ITERATIONS> results;

    if (option == ModelOption::RECESSION_RANDOMIZED) {
        num_iterations = ITERATIONS;
    }

    /* Simulation iterations for investment modeling */
    for (int iter = 0; iter < num_iterations; iter++) {

        /* Initialize with user profile and populate financial planning variables over max years */
        myAsset.initializeFromUserData(user);

        /* Simulate growth curve and investment modeling */
        myAsset.populateGrowthCurves(option);
        myAsset.calculateN();

        /* Determine how long the funds lasted in this iteration */
        results[iter] = myAsset.getFundLongevity();
    }

    if (option == ModelOption::RECESSION_RANDOMIZED) {
        /* Binned results summary */
        groupResultsAndDisplay(results);
    }
    else {
        /* Simple results summary */
        std::cout << "----------------------------------------------" << std::endl;
        std::cout << modelOptionMap.find(option)->second << " simulation summary:" << std::endl;
        std::cout << "Fund longevity = " << myAsset.getFundLongevity()  << " years." << std::endl;
        std::cout << "----------------------------------------------" << std::endl;
    }
}
/**
 * @brief Runs all simulation models on the given user profile.
 *
 * @param user User financial profile used to run the simulations.
 */
void runSimAll(const UserData& user) {
    runSim(user, ModelOption::RECESSION_RANDOMIZED);
    runSim(user, ModelOption::PREDEFINED_YEAR0_LOSS);
    runSim(user, ModelOption::CONSTANT);
}