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

std::vector<std::tuple<int, int>> generateRandomTuples(int n, int k, std::mt19937& rng) {

    assert(k <= n);

    std::vector<std::tuple<int, int>> tuples;
    std::vector<int> available_first(n);
    std::vector<int> available_second(n);
    std::iota(available_first.begin(), available_first.end(), 0);
    std::iota(available_second.begin(), available_second.end(), 0);

    for (int i = 0; i < k; i++) {
        int first_index = rng() % available_first.size();
        int first = available_first[first_index];
        available_first.erase(available_first.begin() + first_index);

        int second_index = rng() % available_second.size();
        int fail_safe = 0;
        while(
            (first == available_second[second_index] || 
            std::find_if(tuples.begin(), tuples.end(), [first, second_index, available_second](const std::tuple<int, int>& tuple) {
                return std::get<0>(tuple) == available_second[second_index] && std::get<1>(tuple) == first;
            }) != tuples.end()) &&
            fail_safe < 10
        ){
            second_index = rng() % available_second.size();
            fail_safe++;
        }
        if(fail_safe == 10) std::cout << "WARNING: Could not generate random tuple" << std::endl;
        int second = available_second[second_index];
        available_second.erase(available_second.begin() + second_index);

        tuples.push_back(std::make_tuple(first, second));
    }
    return tuples;
}

L checkSolutions(std::vector<T> solutions, std::function<std::vector<L>(const std::vector<T>&)> evaluate, std::vector<std::tuple<int, int>> tuples){
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

std::string do_robustness_tests(std::vector<T> solutions, std::vector<int> robustness_tests, int n, int seed, std::function<std::vector<L>(const std::vector<T>&)> evaluate){
    std::vector<L> results;
    std::mt19937 rng(seed);
    for(int k: robustness_tests){
        if(k > n) results.emplace_back(-2);
        std::vector<std::tuple<int, int>> tuples = generateRandomTuples(n, k, rng);
        L fitness = checkSolutions(solutions, evaluate, tuples);
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
    std::string robustness_results = do_robustness_tests(population.get_genes(false), robustness_tests, n, seed, evaluate);
    std::string result = get_csv_line(seed, n, m, mu, run, diversity_value(population.get_genes(true)), population.get_best_fitness(), OPT, "Mu1-unconst", robustness_results, true);
    write_to_file(result, output_file);
    population.execute(termination_criterion);
    robustness_results = do_robustness_tests(population.get_genes(false), robustness_tests, n, seed, evaluate);
    result = get_csv_line(seed, n, m, mu, run, diversity_value(population.get_genes(true)), population.get_best_fitness(), OPT, "Mu1-unconst", robustness_results, false);
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
    std::string robustness_results = do_robustness_tests(population.get_genes(false), robustness_tests, n, seed, evaluate);
    std::string result = get_csv_line(seed, n, m, mu, run, diversity_value(population.get_genes(true)), population.get_best_fitness(), OPT, "Mu1-const", alpha, robustness_results, true);
    write_to_file(result, output_file);
    population.execute(termination_criterion);
    robustness_results = do_robustness_tests(population.get_genes(false), robustness_tests, n, seed, evaluate);
    result = get_csv_line(seed, n, m, mu, run, diversity_value(population.get_genes(true)), population.get_best_fitness(), OPT, "Mu1-const", alpha, robustness_results, false);
    write_to_file(result, output_file);
    return population;
}