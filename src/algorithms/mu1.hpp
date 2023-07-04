#include <iostream>

#include "../population/population.hpp"
#include "../operators/operators_initialization.hpp"
#include "../operators/operators_evaluation.hpp"
#include "../operators/operators_parentSelection.hpp"
#include "../operators/operators_mutation.hpp"
#include "../operators/operators_recombination.hpp"
#include "../operators/operators_survivorSelection.hpp"
#include "../operators/operators_termination.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

Population<T,L> mu1(
    int seed,
    std::function<bool(Population<T,L>&)> termination_criterion,
    std::function<std::vector<T>(std::mt19937&)> initialize,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents,
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> mutate,
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> recombine
){
    std::function<std::vector<L>(const std::vector<T>&)> evaluate_nullptr = nullptr;
    Population<T, L> population(seed, initialize, evaluate, select_parents, mutate, recombine, select_survivors);
    while(!termination_criterion(population)){
        population.execute(0);
    }
    return population;
}

Population<T,L> mu1_test(
    int seed,
    int m,
    std::vector<int> processing_times, 
    std::vector<int> release_dates, 
    std::vector<int> due_dates,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<double(const T&, const T&)> diversity_measure,
    int population_size,
    double alpha,
    double OPT,
    double mutation_rate,
    int termination_generations,
    std::vector<T> initial_population
){

    std::function<bool(Population<T,L>&)> termination_criterion = terminate_generations(termination_generations);
    //std::function<std::vector<T>(std::mt19937&)> initialize = initialize_quality(population_size, processing_times.size(), m, OPT, alpha, evaluate);
    std::function<std::vector<T>(std::mt19937&)> initialize = initialize_fix(initial_population);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors = select_qdiv(alpha, OPT, diversity_measure, evaluate);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents =  select_random(1);
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> mutate = mutate_swap(mutation_rate);
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> recombine = nullptr;

    return mu1(
        seed,
        termination_criterion,
        initialize,
        evaluate,
        select_survivors,
        select_parents,
        mutate,
        recombine
    );
}