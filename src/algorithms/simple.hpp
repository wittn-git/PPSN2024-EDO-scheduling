#include <iostream>
#include <limits>

#include "../population/population.hpp"
#include "../operators/operators_mutation.hpp"
#include "../operators/operators_recombination.hpp"
#include "../operators/operators_parentSelection.hpp"
#include "../operators/operators_initialization.hpp"
#include "../operators/operators_survivorSelection.hpp"
#include "../operators/operators_evaluation.hpp"
#include "../operators/operators_termination.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

Population<T,L> simple_test(
    int seed, 
    std::function<std::vector<T>(std::mt19937&)> initialize,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors,
    int generations
){
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> recombine = nullptr;

    Population population(seed, initialize, evaluate, select_parents, mutate, recombine, select_survivors);
    population.execute(terminate_generations(generations));
    return population;
}