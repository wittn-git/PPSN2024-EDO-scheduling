#include <limits>
#include <iostream>
#include <numeric>

#include "../population/population.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

void objective_optimization(
    Population<T,L>& population, 
    int obj_generations_n, 
    int bound_value,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate_obj,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_obj
){
    // TODO include b in the variation operators
    population.set_evaluate(evaluate_obj);
    population.set_selectSurvivors(select_survivors_obj);
    population.execute_multiple(obj_generations_n);
}

int bound_change(
    Population<T,L>& population,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate_obj,
    int remaining_solutions_n
){
    std::vector<T> genes = population.get_genes();
    std::vector<L> fitnesses = evaluate_obj(genes);
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
    int bound_value,
    int div_generations_con,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate_div,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_div,
    std::function<bool(const std::vector<T>&, const std::vector<T>&)> compare_diversity
){
    // TODO include b in the variation operators
    int i = 0;
    while(i < div_generations_con){
        std::vector<T> old_genes = population.get_genes();
        population.execute();
        std::vector<T> new_genes = population.get_genes();
        if(!compare_diversity(old_genes, new_genes)){
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
    std::function<bool(const Population<T,L>&)> termination_criterion,
    std::function<std::vector<T>(std::mt19937&)> initialize,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate_obj,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate_div,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_obj,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_div,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> recombine,
    std::function<bool(const std::vector<T>&, const std::vector<T>&)> compare_diversity
){
    Population population(0, initialize, evaluate_obj, select_parents, mutate, recombine, select_survivors_obj);
    int bound_value = std::numeric_limits<int>::max();
    while( bound_value > barrier_value && !termination_criterion(population) ){
        objective_optimization(population, obj_generations_n, bound_value, evaluate_obj, select_survivors_obj);
        bound_value = bound_change(population, evaluate_obj, remaining_solutions_n);
        diversity_optimization(population, bound_value, div_generations_con, evaluate_div, select_survivors_div, compare_diversity);
    }
    std::cout << population.bests_to_string(true) << std::endl;
}