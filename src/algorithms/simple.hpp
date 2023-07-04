#include <iostream>
#include <limits>

#include "../population/population.hpp"
#include "../operators/operators_mutation.hpp"
#include "../operators/operators_recombination.hpp"
#include "../operators/operators_parentSelection.hpp"
#include "../operators/operators_initialization.hpp"
#include "../operators/operators_survivorSelection.hpp"
#include "../operators/operators_evaluation.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

Population<T,L> simple_test(
    int seed, 
    int m, 
    std::vector<int> processing_times, 
    std::vector<int> release_dates, 
    std::vector<int> due_dates, 
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> mutate,
    int population_size, 
    int generations,
    int tournament_size,
    double mutation_rate
){

    std::function<std::vector<T>(std::mt19937&)> initialize = initialize_random(population_size, processing_times.size(), m);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents = select_tournament(tournament_size, population_size);
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> recombine = nullptr;
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors = select_mu(population_size, evaluate);

    Population population(seed, initialize, evaluate, select_parents, mutate, recombine, select_survivors);
    population.execute_multiple(generations, std::numeric_limits<int>::max());
    return population;
}