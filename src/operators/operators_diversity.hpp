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

double longest_common_sequence_machine(M machine1, M machine2){
    if(machine1.size() == 0 || machine2.size() == 0){
        return 0;
    }
    if(machine1[0] == machine2[0]){
        return 1 + longest_common_sequence_machine(std::vector<int>(machine1.begin()+1, machine1.end()), std::vector<int>(machine2.begin()+1, machine2.end()));
    }
    return std::max(longest_common_sequence_machine(std::vector<int>(machine1.begin()+1, machine1.end()), machine2), longest_common_sequence_machine(machine1, std::vector<int>(machine2.begin()+1, machine2.end())));
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