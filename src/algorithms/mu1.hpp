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

bool valid_addition(std::vector<std::tuple<int, int>> tuples, std::tuple<int, int> tuple){
    std::vector<int> registered = {std::get<0>(tuple)};
    int last = std::get<1>(tuple);
    while(tuples.size() > 0){
        std::tuple<int, int> follow_tuple;
        for(auto x: tuples){
            if(std::get<0>(x) == last){
                follow_tuple = x;
            }
        }
        if(follow_tuple == std::tuple<int, int>()){
            return true;
        }
        last = std::get<1>(follow_tuple);
        if(std::find(registered.begin(), registered.end(), last) != registered.end()){
            return false;
        }
        registered.push_back(std::get<0>(follow_tuple));
    }
    return true;
}

std::vector<std::tuple<int, int>> generate_tuples(int n, int k, std::mt19937& rng) {
    std::vector<std::tuple<int, int>> tuples;
    std::vector<std::tuple<int, int>> available;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                available.emplace_back(i, j);
            }
        }
    }

    for(int i = 0; i < k; i++){
        std::tuple<int, int> current;
        do{
            if(available.size() == 0){
                throw std::invalid_argument("Not enough tuples available.");
            }
            int index = rng() % available.size();
            current = available[index];
            available.erase(available.begin() + index);
        }while(!valid_addition(tuples, current));
        
        auto remove_predicate = [&](const std::tuple<int, int>& x) {
            return std::get<0>(x) == std::get<0>(current) || std::get<1>(x) == std::get<1>(current);
        };
        available.erase(std::remove_if(available.begin(), available.end(), remove_predicate), available.end());
        tuples.push_back(current);
    }

    return tuples;
}

L check_solutions(std::vector<T> solutions, std::function<std::vector<L>(const std::vector<T>&)> evaluate, std::vector<std::tuple<int, int>> tuples){
    std::vector<int> indices(solutions.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::vector<L> fitnesses = evaluate(solutions);
    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
        return fitnesses[a] < fitnesses[b];
    });
    for(auto index: indices){
        auto solution = solutions[index];
        for (auto tuple: tuples){
            int first = std::get<0>(tuple);
            int second = std::get<1>(tuple);
            bool contains = false;
            for(auto machine: solution){
                if(machine.size() < 2) continue;
                for(int i = 0; i < machine.size()-1; i++){
                    if(machine[i] == first && machine[i+1] == second){
                        contains = true;
                        goto contained;
                    }
                }
            }
            contained:
            if(!contains) goto not_contained;
        }
        return fitnesses[index];
        not_contained: true;
    }
    return -1;
}

std::string do_robustness_tests(std::vector<T> solutions, std::vector<int> robustness_tests, int n, int m, int seed, std::function<std::vector<L>(const std::vector<T>&)> evaluate){
    std::vector<L> results;
    std::mt19937 rng(seed);
    for(int k: robustness_tests){
        int k_m = k - m;
        if(k_m >= n || k_m < 0){
            results.emplace_back(-2);
            continue;
        }
        std::vector<std::tuple<int, int>> tuples = generate_tuples(n, k_m, rng);
        L fitness = check_solutions(solutions, evaluate, tuples);
        results.emplace_back(fitness);
    }
    std::string result = std::accumulate(results.begin(), results.end(), std::string{},
        [](const std::string& a, const L& b) -> std::string {
            return a.empty() ? std::to_string(b) : a + "," + std::to_string(b);
        }
    );
    return result;
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
    std::string output_file, int run, int OPT, std::string operator_string,
    std::vector<int> robustness_tests
){

    std::function<std::vector<T>(std::mt19937&)> initialize = initialize_random(mu, n, m);
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> recombine = nullptr;
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents = select_random(1);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors = nullptr;
    std::function<Diversity_Preserver<T>(const std::vector<T>&, const T&, const Diversity_Preserver<T>&, std::mt19937&)> selectSurvivors_Div = select_pdiv(diversity_measure);
    std::function<double (const std::vector<T> &)> diversity_value = diversity_vector(diversity_measure);

    Population_Mu1<T, L> population(seed, initialize, evaluate, select_parents, mutate, recombine, select_survivors, selectSurvivors_Div);
    std::string robustness_results = do_robustness_tests(population.get_genes(false), robustness_tests, n, m, seed, evaluate);
    std::string result = get_csv_line(seed, n, m, mu, run, diversity_value(population.get_genes(true)), population.get_best_fitness(), OPT, "Mu1-unconst", operator_string, robustness_results, "True");
    write_to_file(result, output_file);
    population.execute(termination_criterion);
    robustness_results = do_robustness_tests(population.get_genes(false), robustness_tests, n, m, seed, evaluate);
    result = get_csv_line(seed, n, m, mu, run, diversity_value(population.get_genes(true)), population.get_best_fitness(), OPT, "Mu1-unconst", operator_string, robustness_results, "False");
    write_to_file(result, output_file);

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
    std::string output_file, int run, int OPT, std::string operator_string,
    std::vector<int> robustness_tests
){

    std::function<std::vector<T>(std::mt19937&)> initialize = initialize_fixed(std::vector<T>(mu, initial_gene));
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> recombine = nullptr;
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_parents =  select_random(1);
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors = nullptr;
    std::function<Diversity_Preserver<T>(const std::vector<T>&, const T&, const Diversity_Preserver<T>&, std::mt19937&)> selectSurvivors_Div = select_qpdiv(alpha, n, OPT, diversity_measure, evaluate);
    std::function<double (const std::vector<T> &)> diversity_value = diversity_vector(diversity_measure);

    Population_Mu1<T,L> population(seed, initialize, evaluate, select_parents, mutate, recombine, select_survivors, selectSurvivors_Div);
    std::string robustness_results = do_robustness_tests(population.get_genes(false), robustness_tests, n, m, seed, evaluate);
    std::string result = get_csv_line(seed, n, m, mu, run, diversity_value(population.get_genes(true)), population.get_best_fitness(), OPT, "Mu1-const", operator_string, alpha, robustness_results, "True");
    write_to_file(result, output_file);
    population.execute(termination_criterion);
    robustness_results = do_robustness_tests(population.get_genes(false), robustness_tests, n, m, seed, evaluate);
    result = get_csv_line(seed, n, m, mu, run, diversity_value(population.get_genes(true)), population.get_best_fitness(), OPT, "Mu1-const", operator_string, alpha, robustness_results, "False");
    write_to_file(result, output_file);
    return population;
}