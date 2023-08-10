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

void loop_parameters(std::vector<int> mus, std::vector<int> ns, std::vector<int> ms, std::function<void(int, int, int)> func, bool restricted){
    for(int n : ns){
        for(int mu : mus){
            for(int m : ms){
                if(restricted && ((mu > (n*n - n)/(n-m)) || m > n) ) continue;
                func(n, mu, m);
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

void test_base(std::vector<int> mus, std::vector<int> ns, std::vector<int> ms, std::vector<double> alphas, std::string output_file){
    int max_processing_time = 50;
    write_to_file("", output_file, false);
    auto base_test = [alphas, output_file, max_processing_time](int mu, int n, int m) {
        int seed = generate_seed(mu, n, m, 0);
        MachineSchedulingProblem problem = get_problem(seed, n, max_processing_time);
        auto [evaluate, diversity_measure, diversity_value] = get_eval_div_funcs(problem);
        auto [OPT, optimal_solution] = get_optimal_solution(problem, m, evaluate);

        Population<T,L> simple_pop = simple_test(
            seed,
            initialize_random(mu, n, m), evaluate, mutate_removeinsert(0.1), select_roulette(mu), select_mu(mu, evaluate),
            300
        );
        write_to_file(get_population_information_string(simple_pop, evaluate, diversity_value, "Simple", mu, n, m), output_file, false);
        
        for(int alpha : alphas){
            Population<T,L>  mu1_const_pop = mu1_constrained(
                seed, m, n, mu,
                terminate_generations(2500), evaluate, mutate_removeinsert(1), diversity_measure,
                alpha, optimal_solution
            );
            write_to_file(get_population_information_string(mu1_const_pop, evaluate, diversity_value, "Mu1-const", mu, n, m), output_file, false);
        }
        
        Population<T,L>  mu1_unconst_pop = mu1_unconstrained(
            seed, m, n, mu,
            terminate_generations(2500), evaluate, mutate_removeinsert(1), diversity_measure
        );
        write_to_file(get_population_information_string(mu1_unconst_pop, evaluate, diversity_value, "Mu1-unconst", mu, n, m), output_file, false);

        Population<T,L> noah_pop = noah(
            seed, m, n, mu, 
            terminate_generations(500), evaluate, mutate_removeinsert(0.1), select_roulette(mu), diversity_measure,
            0, 8, 15, 3
        );
        write_to_file(get_population_information_string(noah_pop, evaluate, diversity_value, "Noah", mu, n, m), output_file, false);
    };
    loop_parameters(mus, ns, ms, base_test, true);
}   

void test_mu1_optimization(std::vector<int> mus, std::vector<int> ns, std::vector<int> ms, int runs, std::string output_file){
    write_to_file("type,seed,n,mu,run,generations,max_generations,diversity,fitness,runtime\n", output_file, false);
    int max_processing_time = 50;
    auto mu1_optimization_test = [runs, output_file, max_processing_time](int mu, int n, int m) {
        for(int run = 0; run < runs; run++) {
            int seed = generate_seed(mu, n, m, run);
            MachineSchedulingProblem problem = get_problem(seed, n, max_processing_time);
            auto [evaluate, diversity_measure, diversity_value] = get_eval_div_funcs(problem);
            auto [OPT, optimal_solution] = get_optimal_solution(problem, m, evaluate);
            auto start = std::chrono::high_resolution_clock::now();
            Population_Mu1<T,L>  opt_pop = mu1_unconstrained(
                seed, 1, n, mu,
                terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutate_removeinsert(1), diversity_measure
            );
            auto stop = std::chrono::high_resolution_clock::now();
            std::string result_opt = "opt," + std::to_string(seed) + "," + std::to_string(n) + "," + std::to_string(mu) + "," + std::to_string(run+1) + "," + std::to_string(opt_pop.get_generation()) + "," + std::to_string(n*n*mu) + "," + std::to_string(diversity_value(opt_pop.get_genes(true))) + "," + std::to_string(evaluate({opt_pop.get_bests(false, evaluate)[0]})[0]) + "," + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()) + "\n";
            write_to_file(result_opt, output_file);

            start = std::chrono::high_resolution_clock::now();
            Population<T,L>  unopt_pop = mu1_unconstrained_unoptimized(
                seed, 1, n, mu,
                terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutate_removeinsert(1), diversity_measure
            );
            stop = std::chrono::high_resolution_clock::now();
            std::string result_unopt = "unopt," + std::to_string(seed) + "," + std::to_string(n) + "," + std::to_string(mu) + "," + std::to_string(run+1) + "," + std::to_string(unopt_pop.get_generation()) + "," + std::to_string(n*n*mu) + "," + std::to_string(diversity_value(unopt_pop.get_genes(true))) + "," + std::to_string(evaluate({unopt_pop.get_bests(false, evaluate)[0]})[0]) + "," + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()) + "\n";
            write_to_file(result_unopt, output_file);
        }
    };
    loop_parameters(mus, ns, ms, mu1_optimization_test, true);
}

void test_algorithm(std::vector<int> mus, std::vector<int> ns, std::vector<int> ms, std::vector<double> alphas, int runs, std::string output_file, std::string algorithm, std::string mutation){

    write_to_file("seed,n,m,mu,run,generations,max_generations,diversity,fitness,opt\n", output_file, false);
    int max_processing_time = 50;
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutation_operator;
    if(mutation == "1RAI"){
        mutation_operator = mutate_removeinsert(1);
    }else if(mutation == "XRAI"){
        throw std::invalid_argument("XRAI is not implemented yet.");
    }else if(mutation == "NSWAP"){
        mutation_operator = mutate_neighborswap(1);
    }else{
        throw std::invalid_argument("Invalid mutation operator.");
        return;
    }

    auto algorithm_test = [runs, output_file, max_processing_time, algorithm, mutation_operator, alphas](int mu, int n, int m) {
        for(int run = 0; run < runs; run++) {
            int seed = generate_seed(mu, n, m, run);
            MachineSchedulingProblem problem = get_problem(seed, n, max_processing_time);
            auto [evaluate, diversity_measure, diversity_value] = get_eval_div_funcs(problem);
            auto [OPT, optimal_solution] = get_optimal_solution(problem, m, evaluate);
            std::string result = "";
            if(algorithm == "Simple"){
                Population<T,L> simple_pop = simple_test(
                    seed,
                    initialize_random(mu, n, m), evaluate, mutation_operator, select_roulette(mu), select_mu(mu, evaluate),
                    300
                );
                result = std::to_string(seed) + "," + std::to_string(n) + "," + std::to_string(m) + "," + std::to_string(mu) + "," + std::to_string(run+1) + "," + std::to_string(simple_pop.get_generation()) + "," + std::to_string(n*n*mu) + "," + std::to_string(diversity_value(simple_pop.get_genes(true))) + "," + std::to_string(evaluate({simple_pop.get_bests(false, evaluate)[0]})[0]) + "," + std::to_string(OPT) + "\n";
            }else if(algorithm == "Mu1-unconst"){
                Population<T,L>  mu1_unconst_pop = mu1_unconstrained(
                    seed, m, n, mu,
                    terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutation_operator, diversity_measure
                );
                result = std::to_string(seed) + "," + std::to_string(n) + "," + std::to_string(m) + "," + std::to_string(mu) + "," + std::to_string(run+1) + "," + std::to_string(mu1_unconst_pop.get_generation()) + "," + std::to_string(n*n*mu) + "," + std::to_string(diversity_value(mu1_unconst_pop.get_genes(true))) + "," + std::to_string(evaluate({mu1_unconst_pop.get_bests(false, evaluate)[0]})[0]) + "," + std::to_string(OPT) + "\n";
            }else if(algorithm == "Mu1-const"){
                for(double alpha: alphas){
                    Population<T,L>  mu1_const_pop = mu1_constrained(
                        seed, m, n, mu,
                        terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutation_operator, diversity_measure,
                        alpha, optimal_solution
                    );
                    result = std::to_string(seed) + "," + std::to_string(n) + "," + std::to_string(m) + "," + std::to_string(mu) + "," + std::to_string(run+1) + "," + std::to_string(mu1_const_pop.get_generation()) + "," + std::to_string(n*n*mu) + "," + std::to_string(diversity_value(mu1_const_pop.get_genes(true))) + "," + std::to_string(evaluate({mu1_const_pop.get_bests(false, evaluate)[0]})[0]) + "," + std::to_string(OPT) + "," + std::to_string(alpha) + "\n";
                }
            }
            write_to_file(result, output_file);
        }
    };
    loop_parameters(mus, ns, mus, algorithm_test, true);
}