#include <iostream>

#include "../population/population.hpp"
#include "../population/population_mu1.hpp"
#include "../operators/operators_initialization.hpp"
#include "../operators/operators_evaluation.hpp"
#include "../operators/operators_parentSelection.hpp"
#include "../operators/operators_mutation.hpp"
#include "../operators/operators_recombination.hpp"
#include "../operators/operators_survivorSelection.hpp"
#include "../operators/operators_termination.hpp"
#include "../utility/documenting.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

Population<T,L> mu1_unconstrained_unoptimized(
    int seed, 
    int m, 
    int n, 
    int mu,
    std::function<bool(Population<T,L>&)> termination_criterion,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate,
    std::function<double(const T&, const T&)> diversity_measure
){

    std::function<std::vector<T>(std::mt19937&)> initialize = initialize_random(mu, n, m);
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> recombine = nullptr;
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents = select_random(1);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors = select_div(diversity_measure);

    Population<T, L> population(seed, initialize, evaluate, select_parents, mutate, recombine, select_survivors);
    population.execute(termination_criterion);
    return population;
}

Population_Mu1<T,L> mu1_unconstrained(
    int seed, 
    int m, 
    int n, 
    int mu,
    std::function<bool(Population<T,L>&)> termination_criterion,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate,
    std::function<double(const T&, const T&)> diversity_measure,
    int run, int OPT, std::string operator_string, std::string output_file
){

    std::function<std::vector<T>(std::mt19937&)> initialize = initialize_random(mu, n, m);
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> recombine = nullptr;
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents = select_random(1);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors = nullptr;
    std::function<Diversity_Preserver<T>(const std::vector<T>&, const T&, const Diversity_Preserver<T>&, std::mt19937&)> selectSurvivors_Div = select_pdiv(diversity_measure);
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);

    Population_Mu1<T, L> population(seed, initialize, evaluate, select_parents, mutate, recombine, select_survivors, selectSurvivors_Div);
    while(!termination_criterion(population)){
        population.execute();
        std::string result = get_csv_line(seed, n, m, mu, run, population.get_generation(), n*n*mu, diversity_value(population.get_genes(true)), population.get_best_fitness(), OPT, "Mu1-unconst", operator_string);
        write_to_file(result, output_file);
    }
        
    return population;
}

Population_Mu1<T,L> mu1_constrained(
    int seed, 
    int m, 
    int n, 
    int mu,
    std::function<bool(Population<T,L>&)> termination_criterion,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate,
    std::function<double(const T&, const T&)> diversity_measure,
    double alpha,
    T initial_gene,
    int run, int OPT, std::string operator_string, std::string output_file
){

    double OPT = evaluate({initial_gene})[0];

    std::function<std::vector<T>(std::mt19937&)> initialize = initialize_fixed(std::vector<T>(mu, initial_gene));
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> recombine = nullptr;
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents =  select_random(1);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors = nullptr;
    std::function<Diversity_Preserver<T>(const std::vector<T>&, const T&, const Diversity_Preserver<T>&, std::mt19937&)> selectSurvivors_Div = select_qpdiv(alpha, n, OPT, diversity_measure, evaluate);
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);

    Population_Mu1<T,L> population(seed, initialize, evaluate, select_parents, mutate, recombine, select_survivors, selectSurvivors_Div);
    
    while(!termination_criterion(population)){
        population.execute();
        std::string result = get_csv_line(seed, n, m, mu, run, population.get_generation(), n*n*mu, diversity_value(population.get_genes(true)), population.get_best_fitness(), OPT, "Mu1-const", operator_string, alpha);
        write_to_file(result, output_file);
    }

    return population;
}