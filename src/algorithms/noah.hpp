#pragma once

#include "../population/population_noah.hpp"
#include "../operators/operators_initialization.hpp"
#include "../operators/operators_parentSelection.hpp"
#include "../operators/operators_mutation.hpp"
#include "../operators/operators_recombination.hpp"
#include "../operators/operators_survivorSelection.hpp"
#include "../operators/operators_termination.hpp"
#include "../operators/operators_diversity.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

void objective_optimization(
    Population_Noah<T,L>& population,
    std::function<bool(Population<T,L>&)> termination_criterion,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_obj,
    std::function<Diversity_Preserver<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, const Diversity_Preserver<T>&, std::mt19937&)> select_survivors_obj,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    int obj_generations_n,
    double bound_value
){

    int n = population.get_size(true);
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_bd = mutate_bound(mutate, evaluate, bound_value);
    population.set_mutate(mutate_bd);
    population.set_selectParents(select_parents_obj);
    population.set_selectSurvivors_Div(select_survivors_obj);

    int i = 0;
    while(i < obj_generations_n && !termination_criterion(population)){
        population.execute();
        i++;
    }
}

double bound_change(
    Population_Noah<T,L>& population,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    int remaining_solutions_n
){

    std::vector<T> genes = population.get_genes(true);
    std::vector<L> fitnesses = evaluate(genes);

    if(remaining_solutions_n > population.get_size(true)){
        population.set_genes(genes);
        return *std::max_element(fitnesses.begin(), fitnesses.end());
    }

    std::vector<int> indices(genes.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::partial_sort(indices.begin(), indices.begin() + remaining_solutions_n, indices.end(), [fitnesses](int a, int b) {
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
    Population_Noah<T,L>& population,
    std::function<bool(Population<T,L>&)> termination_criterion,
    std::function<Diversity_Preserver<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, const Diversity_Preserver<T>&, std::mt19937&)> select_survivors_div,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<double(const std::vector<T>&)> diversity_value,
    double bound_value,
    int mu,
    int div_generations_con
){

    auto mutate_bd = mutate_bound(mutate, evaluate, bound_value);
    population.set_mutate(mutate_bd);
    population.set_selectSurvivors_Div(select_survivors_div);

    double old_diversity = diversity_value(population.get_genes(true));
    std::vector<T> old_genes = population.get_genes(true);

    int i = 0;
    while(i < div_generations_con && !termination_criterion(population)){
        std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_div = select_random(mu + 1 - population.get_size(true));
        population.set_selectParents(select_parents_div);
        population.execute();
        double new_diversity = diversity_value(population.get_genes(true));
        if(new_diversity <= old_diversity){
            population.set_genes(old_genes);
            i += 1;
        }else{
            old_diversity = new_diversity;
            old_genes = population.get_genes(true);
        }
    }
    int k = 0;
}

Population_Noah<T,L> noah(
    int seed, int mu, int n, int m,
    std::function<bool(Population<T,L>&)> termination_criterion,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_obj,
    std::function<double(const T&, const T&)> diversity_measure,
    int obj_generations_n, 
    int remaining_solutions_n,
    int div_generations_con
){

    double bound_value = -std::numeric_limits<double>::max();
    remaining_solutions_n = std::max({2, remaining_solutions_n});

    std::function<std::vector<T>(std::mt19937&)> initialize = initialize_random(mu, n, m);
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_bd = mutate_bound(mutate, evaluate, bound_value);
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> recombine = nullptr;
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors = nullptr;
    std::function<Diversity_Preserver<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, const Diversity_Preserver<T>&, std::mt19937&)> select_survivors_obj = select_pmu(mu, evaluate);
    std::function<Diversity_Preserver<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, const Diversity_Preserver<T>&, std::mt19937&)> select_survivors_div = select_npdiv(diversity_measure);
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);

    Population_Noah<T,L> population(seed, initialize, evaluate, select_parents_obj, mutate, recombine, select_survivors, select_survivors_obj);
    
    while(!termination_criterion(population) ){
        objective_optimization(population, termination_criterion, select_parents_obj, select_survivors_obj, mutate_bd, evaluate, obj_generations_n, bound_value);
        bound_value = bound_change(population, evaluate, remaining_solutions_n);
        diversity_optimization(population, termination_criterion, select_survivors_div, mutate, evaluate, diversity_value, bound_value, mu, div_generations_con);
    }
    
    return population;
}