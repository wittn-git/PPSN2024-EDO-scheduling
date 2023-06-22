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

void mu1(
    std::function<bool(Population<T,L>&)> termination_criterion,
    std::function<std::vector<T>(std::mt19937&)> initialize,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents,
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> mutate,
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> recombine
){
    std::function<std::vector<L>(const std::vector<T>&)> evaluate_nullptr = nullptr;
    Population<T, L> population(0, initialize, evaluate, select_parents, mutate, recombine, select_survivors);
    while(!termination_criterion(population)){
        population.execute(0);
    }
    std::cout << "Simple" << std::endl << "Generations: " << population.get_generation() << std::endl << "Best Individuals: " << std::endl;
    std::cout << population.bests_to_string(true, evaluate) << std::endl;
}

void mu1_test(
    int m,
    std::vector<int> processing_times, 
    std::vector<int> release_dates, 
    std::vector<int> due_dates,
    int population_size,
    double alpha,
    double OPT,
    double mutation_rate,
    int termination_generations
){

    std::function<bool(Population<T,L>&)> termination_criterion = terminate_generations(termination_generations);
    std::function<std::vector<L>(const std::vector<T>&)> evaluate = evaluate_makespan(processing_times, release_dates);
    std::function<std::vector<T>(std::mt19937&)> initialize = initialize_quality(population_size, processing_times.size(), m, OPT, alpha, evaluate);
    std::function<double(const T&, const T&)> diversity_measure = diversity_DFM();
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors = select_qdiv(alpha, OPT, diversity_measure, evaluate);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents =  select_random(1);
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> mutate = mutate_swap(mutation_rate);
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> recombine = nullptr;

    mu1(
        termination_criterion,
        initialize,
        evaluate,
        select_survivors,
        select_parents,
        mutate,
        recombine
    );

}