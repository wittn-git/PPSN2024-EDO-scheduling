#pragma once

#include <functional>
#include <vector>
#include <random>
#include <algorithm>

using T = std::vector<std::vector<int>>;
using L = double;

//Initialization Operators ----------------------------------------------------------

/*
    Random Parallel Initialization: Randomly initialize genes with a parallel schedule 
    Arguments:
        - population_size:  Number of genes to be generated
        - jobs_n:           Number of jobs
        - machines_n:       Number of machines
*/

std::function<std::vector<T>(std::mt19937&)> initialize_random(int population_size, int jobs_n, int machines_n) {
    return [population_size, jobs_n, machines_n](std::mt19937& generator) -> std::vector<T> {
        std::vector<T> genes(population_size);
        std::uniform_int_distribution< int > distribute_machines(0, machines_n-1);
        std::transform(genes.begin(), genes.end(), genes.begin(), [population_size, jobs_n, machines_n, &generator, distribute_machines](T& gene) mutable -> T {
            gene = std::vector<std::vector<int>>(machines_n);
            for(int i = 0; i < jobs_n; i++){
                gene[distribute_machines(generator)].emplace_back(i);
            }
            for(auto& schedule : gene){
                std::shuffle(schedule.begin(), schedule.end(), generator);
            }
            return gene;
        });
        return genes;
    };
}

std::function<std::vector<T>(std::mt19937&)> initialize_quality(int population_size, int jobs_n, int machines_n, double OPT, double alpha, std::function<std::vector<L>(const std::vector<T>&)> evaluate){
    return [population_size, jobs_n, machines_n, OPT, alpha, evaluate](std::mt19937& generator) -> std::vector<T> {
        std::vector<T> genes(population_size);
        std::uniform_int_distribution< int > distribute_machines(0, machines_n-1);
        std::transform(genes.begin(), genes.end(), genes.begin(), [population_size, jobs_n, machines_n, &generator, distribute_machines, OPT, alpha, evaluate](T& gene) mutable -> T {
            do {
                gene = std::vector<std::vector<int>>(machines_n);
                for(int i = 0; i < jobs_n; i++){
                    gene[distribute_machines(generator)].emplace_back(i);
                }
                for(auto& schedule : gene){
                    std::shuffle(schedule.begin(), schedule.end(), generator);
                }
                return gene;
            }while(evaluate({gene})[0] > (1+alpha) * OPT);       
        });
        return genes;
    };
}