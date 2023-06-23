#pragma once

#include <numeric>
#include <cmath>
#include <limits>
#include <vector>
#include <iostream>
#include <functional>

using T = std::vector<std::vector<int>>;
using L = double;

// Diversity measure operators (gene level) ------------------------------------------

std::function<double(const T& , const T&)> diversity_DFM(){
    return [](const T& gene1, const T& gene2) -> double {
        std::vector<double> diversity;
        int common_DFS = 0;
        for(auto machine1 : gene1){
            for(auto machine2 : gene2){
                for(int i = 0; i < machine1.size() - 1; i++){
                    for(int j = 0; j < machine2.size() - 1; j++){
                        if(machine1[i] == machine2[j] && machine1[i+1] == machine2[j+1]){
                            common_DFS++;
                        }
                    }
                }
            }
        }
        return common_DFS;
    };
}

// Diversity measure operators (population level) --------------------------------------

std::function<double(const std::vector<T>&)> diversity_vector(std::function<double(const T& , const T&)> diversity_measure){
    return [diversity_measure](const std::vector<T>& genes) -> double {
        std::vector<double> diversity_scores;
        for(int i = 0; i < genes.size(); i++){
            for(int j = i + 1; j < genes.size(); j++){
                diversity_scores.emplace_back(diversity_measure(genes[i], genes[j]));
            }
        }
        return -(std::accumulate(diversity_scores.begin(), diversity_scores.end(), 0.0) / diversity_scores.size());
    };
}

std::function<double(const std::vector<double>&)> diversity_vector(){
    return [](const std::vector<double>& diversity_scores) -> double {
        return -(std::accumulate(diversity_scores.begin(), diversity_scores.end(), 0.0) / diversity_scores.size());
    };
}