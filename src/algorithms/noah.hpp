#include <limits>
#include <iostream>
#include <numeric>

#include "../population/population.hpp"
#include "../operators/operators_initialization.hpp"
#include "../operators/operators_evaluation.hpp"
#include "../operators/operators_parentSelection.hpp"
#include "../operators/operators_mutation.hpp"
#include "../operators/operators_recombination.hpp"
#include "../operators/operators_survivorSelection.hpp"
#include "../operators/operators_termination.hpp"
#include "../operators/operators_diversity.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

void objective_optimization(
    Population<T,L>& population, 
    int obj_generations_n, 
    double bound_value,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_obj,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_obj
){
    population.set_selectParents(select_parents_obj);
    population.set_selectSurvivors(select_survivors_obj);
    population.execute_multiple(obj_generations_n, bound_value);
}

int bound_change(
    Population<T,L>& population,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    int remaining_solutions_n
){
    std::vector<T> genes = population.get_genes();
    std::vector<L> fitnesses = evaluate(genes);
    std::vector<int> indices(genes.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::partial_sort(indices.begin(), indices.begin() + remaining_solutions_n, indices.end(), [&](int a, int b) {
        return fitnesses[a] < fitnesses[b];
    });
    std::vector<T> new_genes(remaining_solutions_n);
    std::transform(indices.begin(), indices.begin() + remaining_solutions_n, new_genes.begin(), [&](int i) {
        return genes[i];
    });
    population.set_genes(new_genes);
    return fitnesses[indices[remaining_solutions_n-1]];
}

void diversity_optimization(
    Population<T,L>& population,
    double bound_value,
    int div_generations_con,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_div,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_div,
    std::function<double(const std::vector<T>&)> diversity_measure
){
    int i = 0;
    population.set_selectParents(select_parents_div);
    population.set_selectSurvivors(select_survivors_div);
    while(i < div_generations_con){
        std::vector<T> old_genes = population.get_genes();
        population.execute(bound_value);
        std::vector<T> new_genes = population.get_genes();
        if(diversity_measure(old_genes) > diversity_measure(new_genes)){
            population.set_genes(old_genes);
            i++;
        }
    }
}

void noah(
    int barrier_value,
    int obj_generations_n,
    int remaining_solutions_n,
    int div_generations_con,
    std::function<bool(Population<T,L>&)> termination_criterion,
    std::function<std::vector<T>(std::mt19937&)> initialize,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_obj,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_div,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_obj,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_div,
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> mutate,
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> recombine,
    std::function<double(const std::vector<T>&)> diversity_measure
){
    Population population(0, initialize, evaluate, select_parents_obj, mutate, recombine, select_survivors_obj);
    double bound_value = std::numeric_limits<double>::max();
    while( bound_value > barrier_value && !termination_criterion(population) ){
        objective_optimization(population, obj_generations_n, bound_value, select_parents_obj, select_survivors_obj);
        bound_value = bound_change(population, evaluate, remaining_solutions_n);
        diversity_optimization(population, bound_value, div_generations_con, select_parents_div, select_survivors_div, diversity_measure);
    }
    std::cout << population.bests_to_string(true) << std::endl;
}

void noah_test(){
    std::vector<int> processing_times   = {1, 1, 2, 6, 10, 2,  7,  7,  9,  15};
    std::vector<int> release_dates      = {0, 0, 0, 1, 10, 30, 0,  2,  5,  1};
    std::vector<int> due_dates          = {10,2, 4, 7, 21, 36, 20, 10, 17, 17};

    int population_size = 25;
    int barrier_value = 60;
    int obj_generations_n = 5;
    int remaining_solutions_n = population_size;
    int div_generations_con = 3;

    std::function<double(const std::vector<T>&)> diversity_measure = diversity_LCS();
    std::function<bool(Population<T,L>&)> termination_criterion = terminate_generations(25);
    std::function<std::vector<T>(std::mt19937&)> initialize = initialize_random(population_size, 10, 1);
    std::function<std::vector<L>(const std::vector<T>&)> evaluate = evaluate_makespan(processing_times, release_dates);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_obj = select_mu(population_size, evaluate);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_div = select_div(diversity_measure);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_obj = select_tournament(3, population_size);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_div = select_tournament(3, 1);
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> mutate = mutate_swap(0.1);
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> recombine = nullptr;

    noah(
        barrier_value,
        obj_generations_n,
        remaining_solutions_n,
        div_generations_con,
        termination_criterion,
        initialize,
        evaluate,
        select_survivors_obj,
        select_survivors_div,
        select_parents_obj,
        select_parents_div,
        mutate,
        recombine,
        diversity_measure
    );
}