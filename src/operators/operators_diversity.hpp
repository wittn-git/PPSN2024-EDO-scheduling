#pragma once

#include <numeric>
#include <cmath>
#include <limits>
#include <vector>
#include <functional>

using T = std::vector<std::vector<int>>;
using L = double;

// Utility ---------------------------------------------------------------------------

double euclideanNorm(const std::vector<double>& vec) {
    double sumOfSquares = 0.0;
    for (int num : vec) sumOfSquares += num * num;
    return std::sqrt(sumOfSquares);
}

// Diversity measure operators (gene level) ------------------------------------------

std::function<double(const T& , const T&)> diversity_DFM(){
    return [](const T& gene1, const T& gene2) -> double {
        std::vector<double> diversity;
        int common_DFS = 0;
        for(auto machine1 : gene1){
            if(machine1.empty()) continue;
            for(auto machine2 : gene2){
                if(machine2.empty()) continue;
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
        int n = std::accumulate(genes[0].begin(), genes[0].end(), 0, [](int sum, const std::vector<int>& machine) -> int {
            return sum + machine.size();
        });
        int m = genes[0].size();
        int mu = genes.size();
        std::vector<double> diversity_scores;
        for(int i = 0; i < genes.size(); i++){
            for(int j = i + 1; j < genes.size(); j++){
                diversity_scores.emplace_back(diversity_measure(genes[i], genes[j]));
            }
        }
        return 1 - (euclideanNorm(diversity_scores) / ((n-m) * std::sqrt((mu * mu - mu)/2)));
    };
}

std::function<double(const std::vector<double>&)> diversity_vector(int n, int m, int mu){
    return [n, m, mu](const std::vector<double>& diversity_scores) -> double {
        return 1 - (euclideanNorm(diversity_scores) / ((n-m) * std::sqrt((mu * mu - mu)/2)));
    };
}