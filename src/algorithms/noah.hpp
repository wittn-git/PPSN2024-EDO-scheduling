#include <limits>
#include <numeric>
#include <iostream>

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
    int mu,
    double bound_value,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_obj,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_obj,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate
){
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_bd = mutate_bound(mutate, evaluate, bound_value, mu);
    population.set_mutate(mutate_bd);
    population.set_selectParents(select_parents_obj);
    population.set_selectSurvivors(select_survivors_obj);
    population.execute(terminate_generations(obj_generations_n));
}

int bound_change(
    Population<T,L>& population,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    int remaining_solutions_n
){
    std::vector<T> genes = population.get_genes(true);
    std::vector<L> fitnesses = evaluate(genes);
    std::vector<int> indices(genes.size());
    std::iota(indices.begin(), indices.end(), 0);

    if(remaining_solutions_n > genes.size()){
        population.set_genes(genes);
        return *std::min_element(fitnesses.begin(), fitnesses.end());
    }

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
    Population<T,L>& population,
    int div_generations_con,
    int mu,
    double bound_value,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_div,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_div,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<double(const std::vector<T>&)> diversity_value
){
    int i = 0;
    population.set_selectParents(select_parents_div);
    population.set_selectSurvivors(select_survivors_div);
    double diversity_old = diversity_value(population.get_genes(true));
    while(i < div_generations_con){
        std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_bd = mutate_bound(mutate, evaluate, bound_value, 2*mu - population.get_size(true));
        population.set_mutate(mutate_bd);
        std::vector<T> old_genes = population.get_genes(true);
        population.execute();
        std::vector<T> new_genes = population.get_genes(true);
        double diversity_new = diversity_value(new_genes);
        if(diversity_old >= diversity_new && old_genes.size() > 1){
            population.set_genes(old_genes);
            i++;
        }else{
            diversity_old = diversity_new;
        }
    }
}

Population<T,L> noah(
    int seed,
    int m,
    int n,
    int mu,
    std::function<bool(Population<T,L>&)> termination_criterion,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_obj,
    std::function<double(const T&, const T&)> diversity_measure,
    int barrier_value,
    int obj_generations_n,
    int remaining_solutions_n,
    int div_generations_con
){

    std::function<std::vector<T>(std::mt19937&)> initialize = initialize_random(mu, n, m);
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> recombine = nullptr;
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_div = select_all();
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_obj = select_mu(mu, evaluate);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_div = select_div(diversity_measure);
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);

    Population population(seed, initialize, evaluate, select_parents_obj, mutate, recombine, select_survivors_obj);
    double bound_value = -std::numeric_limits<double>::max();
    
    while( bound_value < barrier_value && !termination_criterion(population) ){
        objective_optimization(population, obj_generations_n, mu, bound_value, select_parents_obj, select_survivors_obj, mutate, evaluate);
        bound_value = bound_change(population, evaluate, remaining_solutions_n);
        diversity_optimization(population, div_generations_con, mu, bound_value, select_parents_div, select_survivors_div, mutate, evaluate, diversity_value);
    }
    return population;
}