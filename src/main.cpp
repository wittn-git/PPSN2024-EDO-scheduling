#include <iostream>

#include "population/population.hpp"
#include "operators/operators_mutation.hpp"
#include "operators/operators_recombination.hpp"
#include "operators/operators_parentSelection.hpp"
#include "operators/operators_initialization.hpp"
#include "operators/operators_survivorSelection.hpp"
#include "operators/operators_evaluation.hpp"

int main() {
    std::vector<int> processing_times   = {1, 1, 2, 6, 10, 2,  7,  7,  9,  15};
    std::vector<int> release_dates      = {0, 0, 0, 1, 10, 30, 0,  2,  5,  1};
    std::vector<int> due_dates          = {10,2, 4, 7, 21, 36, 20, 10, 17, 17};

    using T = std::vector<std::vector<int>>;
    using L = int;

    std::function<std::vector<T>(std::mt19937&)> initialize = initialize_random(10, 10, 1);
    std::function<std::vector<L>(const std::vector<T>&)> evaluate = evaluate_makespan(processing_times, release_dates);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents = select_tournament(2);
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate = mutate_swap(0.1);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<T>&, std::mt19937&)> recombine = nullptr;
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&)> select_survivors = select_mu(10, evaluate);

    Population population(0, initialize, evaluate, select_parents, mutate, recombine, select_survivors);
    population.execute_multiple(50);
    std::cout << population.to_string() << std::endl;
}