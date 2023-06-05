#include <limits>
#include <iostream>

#include "../population/population.hpp"

void objective_optimization(
    Population& population, 
    int obj_generations_n, 
    int bound_values    
){
    //TODO: implement
}

int bound_change(
    Population& population,
    int bound_value, 
    int remaining_solutions_n
){
    //TODO: implement
    return bound_value;
}

void diversity_optimization(
    Population& population,
    int population_size,
    int bound_value,
    int div_generations_con
){
    //TODO: implement
}

void noah(
    int population_size,
    int barrier_value,
    int obj_generations_n,
    int remaining_solutions_n,
    int div_generations_con,
    std::function<bool(const Population&)> termination_criterion,
    std::function<std::vector<T>(std::mt19937&)> initialize,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate_obj,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents_obj,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_obj,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> recombine_obj,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors_obj
){

    Population population(0, initialize, evaluate_obj, select_parents_obj, mutate_obj, recombine_obj, select_survivors_obj);
    int bound_value = std::numeric_limits<int>::max();
    while( bound_value > barrier_value && !termination_criterion(population) ){
        objective_optimization(population, obj_generations_n, bound_value);
        bound_value = bound_change(population, bound_value, remaining_solutions_n);
        diversity_optimization(population, population_size, bound_value, div_generations_con);
    }

    std::cout << population.bests_to_string(true) << std::endl;

}