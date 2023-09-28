#pragma once

#include <iostream>
#include <chrono>

#include "../algorithms/simple.hpp"
#include "../algorithms/noah.hpp"
#include "../algorithms/mu1.hpp"
#include "../utility/generating.hpp"
#include "../utility/documenting.hpp"
#include "../utility/solvers.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

// Utility function for testing -----------------------------------------------------

int generate_seed(int mu, int n, int m, int run){
    return n*mu*mu+n*run*run+n+m*run+m*m+run;
}

bool is_viable_combination(int mu, int n, int m){
    return (m < n) && (mu <= (n*n - n)/(n-m));
}

void loop_parameters(std::vector<int> mus, std::vector<int> ns, std::vector<int> ms, int runs, std::function<void(int, int, int, int)> func){
    #pragma omp parallel for collapse(4)
    for(int n : ns){
        for(int mu : mus){
            for(int m : ms){
                for(int run = 0; run < runs; run++) func(mu, n, m, run);
            }
        }
    }
}

std::tuple<int, T> get_optimal_solution(MachineSchedulingProblem problem, int m, std::function<std::vector<L>(const std::vector<T>&)> evaluate) {
    T optimal_solution;
    if (m == 1) optimal_solution = {moores_algorithm(problem)};
    else optimal_solution = approximation_algorithm(problem, m);
    int OPT = evaluate({optimal_solution})[0];
    return std::make_tuple(OPT, optimal_solution);
}

std::tuple<std::function<std::vector<L>(const std::vector<T>&)>, std::function<double(const T&, const T&)>, std::function<double(const std::vector<T>&)>> get_eval_div_funcs(MachineSchedulingProblem problem){
    std::function<std::vector<L>(const std::vector<T>&)> evaluate = evaluate_tardyjobs(problem);
    std::function<double(const T&, const T&)> diversity_measure = diversity_DFM();
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);
    return std::make_tuple(evaluate, diversity_measure, diversity_value);
}

// Test functions ------------------------------------------------------------------

void test_algorithm(std::vector<int> mus, std::vector<int> ns, std::vector<int> ms, std::vector<double> alphas, int runs, std::string output_file, std::string algorithm, std::string operator_string, std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutation_operator){
   
    std::string header = "seed,n,m,mu,run,generations,max_generations,diversity,fitness,opt,algorithm,mutation";
    header += algorithm == "Mu1-const" ? ",alpha\n" : "\n";
    write_to_file(header, output_file, false);
    int max_processing_time = 50;

    auto algorithm_test = [output_file, max_processing_time, algorithm, mutation_operator, alphas, operator_string](int mu, int n, int m, int run) {

        if(!is_viable_combination(mu, n, m)) return;
        std::vector<std::vector<int>> combinations_unconst = {
            {2,5,20},
            {2,25,15},
            {10,5,23},
            {10,10,20},
            {10,25,0},
            {25,25,2}
        };
        std::vector<std::vector<int>> combinations_const = {
            {2,5,6},
            {2,10,0},
            {2,25,3},
            {10,5,8},
            {10,10,11},
            {10,25,1},
            {25,25,29}
        };
        if(algorithm == "Mu1-unconst"){
            if(std::find(combinations_unconst.begin(), combinations_unconst.end(), std::vector<int>{mu, n, run}) == combinations_unconst.end()) return;
        }else if(algorithm == "Mu1-const"){
            if(std::find(combinations_const.begin(), combinations_const.end(), std::vector<int>{mu, n, run}) == combinations_const.end()) return;
        }
        
        int seed = generate_seed(mu, n, m, run);
        MachineSchedulingProblem problem = get_problem(seed, n, max_processing_time);
        auto [evaluate, diversity_measure, diversity_value] = get_eval_div_funcs(problem);
        auto [OPT, optimal_solution] = get_optimal_solution(problem, m, evaluate);
        std::string result;
        if(algorithm == "Simple"){
            Population<T,L> population = simple_test(
                seed,
                initialize_random(mu, n, m), evaluate, mutation_operator, select_roulette(mu), select_mu(mu, evaluate),
                300
            );
        }else if(algorithm == "Mu1-unconst"){
            Population<T,L> population = mu1_unconstrained(
                seed, m, n, mu,
                terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutation_operator, diversity_measure,
                run, OPT, operator_string, output_file
            );
        }else if(algorithm == "Mu1-const"){
            for(double alpha: alphas){
                Population<T,L> population = mu1_constrained(
                    seed, m, n, mu,
                    terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutation_operator, diversity_measure,
                    alpha, optimal_solution,
                    run, OPT, operator_string, output_file
                );
            }
        }
    };

    loop_parameters(mus, ns, ms, runs, algorithm_test);
}