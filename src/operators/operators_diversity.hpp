#pragma once

#include <numeric>
#include <cmath>
#include <limits>
#include <vector>
#include <functional>

using M = std::vector<int>;
using T = std::vector<M>;
using L = double;

// Utility ----------------------------------------------------------------------------

double euclideanNorm(const std::vector<double> vec) {
    double squareSum = std::inner_product(vec.begin(), vec.end(), vec.begin(), 0.0);
    return std::sqrt(squareSum);
}

double longest_common_sequence_machine(M machine1, M machine2) {
    int size1 = machine1.size();
    int size2 = machine2.size();
    std::vector<std::vector<double>> dp(size1 + 1, std::vector<double>(size2 + 1, 0));
    for (int i = size1 - 1; i >= 0; i--) {
        for (int j = size2 - 1; j >= 0; j--) {
            if (machine1[i] == machine2[j]) {
                dp[i][j] = 1 + dp[i + 1][j + 1];
            } else {
                dp[i][j] = std::max(dp[i + 1][j], dp[i][j + 1]);
            }
        }
    }
    return dp[0][0];
}

double longest_common_sequence_schedule(T schedule1, T schedule2){
    double minVal = std::numeric_limits<double>::max();
    for(auto machine1 : schedule1){
        for(auto machine2 : schedule2){
            double lcsm = longest_common_sequence_machine(machine1, machine2);
            if(lcsm < minVal){
                minVal = lcsm;
            }
        }
    }
    return minVal;
}

// Diversity measure operators --------------------------------------------------------

std::function<double(const std::vector<T>&)> diversity_LCS(){
    return [](const std::vector<T>& genes) -> double {
        std::vector<double> diversity;
        for(auto gene1 : genes){
            for(auto gene2 : genes){
                if(gene1 != gene2){
                    diversity.push_back(longest_common_sequence_schedule(gene1, gene2));
                }
            }
        }
        return 1/euclideanNorm(diversity);
    };
}