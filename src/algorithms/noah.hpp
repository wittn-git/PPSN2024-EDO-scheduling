#include <limits>
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
    std::function<double(const std::vector<T>&)> diversity_value
){
    int i = 0;
    population.set_selectParents(select_parents_div);
    population.set_selectSurvivors(select_survivors_div);
    while(i < div_generations_con){
        std::vector<T> old_genes = population.get_genes();
        population.execute(bound_value);
        std::vector<T> new_genes = population.get_genes();
        if(diversity_value(old_genes) > diversity_value(new_genes)){
            population.set_genes(old_genes);
            i++;
        }
    }
}

Population<T,L> noah(
    int seed,
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
    std::function<double(const std::vector<T>&)> diversity_value
){
    Population population(seed, initialize, evaluate, select_parents_obj, mutate, recombine, select_survivors_obj);
    double bound_value = std::numeric_limits<double>::max();
    int iteration = 0;
    while( bound_value > barrier_value && !termination_criterion(population) ){
        objective_optimization(population, obj_generations_n, bound_value, select_parents_obj, select_survivors_obj);
        bound_value = bound_change(population, evaluate, remaining_solutions_n);
        diversity_optimization(population, bound_value, div_generations_con, select_parents_div, select_survivors_div, diversity_value);
    } 
    return population;
}

Population<T,L> noah_test(
    int seed,
    int m,
    std::vector<int> processing_times, 
    std::vector<int> release_dates, 
    std::vector<int> due_dates,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<double(const T&, const T&)> diversity_measure,
    int population_size,
    int barrier_value,
    int obj_generations_n,
    int remaining_solutions_n,
    int div_generations_con,
    int termination_generations,
    double mutation_rate,
    int tournament_size
){

    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);
    std::function<bool(Population<T,L>&)> termination_criterion = terminate_generations(termination_generations);
    std::function<std::vector<T>(std::mt19937&)> initialize = initialize_random(population_size, processing_times.size(), m);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_obj = select_mu(population_size, evaluate);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_div = select_div(diversity_measure);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_obj = select_tournament(tournament_size, population_size);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_div = select_tournament(tournament_size, 1);
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> mutate = mutate_swap(mutation_rate);
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> recombine = nullptr;

    return noah(
        seed,
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
        diversity_value
    );
}