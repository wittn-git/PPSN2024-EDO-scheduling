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

void loop_parameters(std::vector<int> mus, std::vector<int> ns, std::vector<int> ms, int runs, std::function<void(int, int, int, int)> func, bool restricted){
    for(int n : ns){
        for(int mu : mus){
            for(int m : ms){
                if(restricted && ((mu > (n*n - n)/(n-m)) || m > n) ) continue;
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

void test_base(std::vector<int> mus, std::vector<int> ns, std::vector<int> ms, std::vector<double> alphas, int runs, std::string output_file, std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutation_operator){
    
    int max_processing_time = 50;
    write_to_file("", output_file, false);

    auto base_test = [alphas, output_file, max_processing_time, mutation_operator](int mu, int n, int m, int run) {
        int seed = generate_seed(mu, n, m, run);
        MachineSchedulingProblem problem = get_problem(seed, n, max_processing_time);
        auto [evaluate, diversity_measure, diversity_value] = get_eval_div_funcs(problem);
        auto [OPT, optimal_solution] = get_optimal_solution(problem, m, evaluate);

        Population<T,L> simple_pop = simple_test(
            seed,
            initialize_random(mu, n, m), evaluate, mutation_operator, select_roulette(mu), select_mu(mu, evaluate),
            300
        );
        write_to_file(createPopulationReport(simple_pop, evaluate, diversity_value, "Simple", mu, n, m) + "\n", output_file);
        
        for(double alpha : alphas){
            Population<T,L>  mu1_const_pop = mu1_constrained(
                seed, m, n, mu,
                terminate_generations(2500), evaluate, mutation_operator, diversity_measure,
                alpha, optimal_solution
            );
            write_to_file(createPopulationReport(mu1_const_pop, evaluate, diversity_value, "Mu1-const " + std::to_string(alpha), mu, n, m) + "\n", output_file);
        }
        
        Population<T,L>  mu1_unconst_pop = mu1_unconstrained(
            seed, m, n, mu,
            terminate_generations(2500), evaluate, mutation_operator, diversity_measure
        );
        write_to_file(createPopulationReport(mu1_unconst_pop, evaluate, diversity_value, "Mu1-unconst", mu, n, m) + "\n", output_file);

        for(double alpha : alphas){
            Population<T,L> noah_pop = noah(
                seed, mu, n, m,
                terminate_generations(500), evaluate, mutation_operator, select_tournament(2, mu), diversity_measure,
                OPT*(1+alpha), mu, 0.5*mu, 0.5*mu
            );
            write_to_file(createPopulationReport(noah_pop, evaluate, diversity_value, "Noah " + std::to_string(alpha), mu, n, m), output_file);
        }
    };

    loop_parameters(mus, ns, ms, runs, base_test, true);
}   

void test_mu1_optimization(std::vector<int> mus, std::vector<int> ns, std::vector<int> ms, int runs, std::string output_file, std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutation_operator){
    
    write_to_file("type,seed,n,m,mu,run,generations,max_generations,diversity,fitness,runtime\n", output_file, false);
    int max_processing_time = 50;

    auto mu1_optimization_test = [runs, output_file, max_processing_time, mutation_operator](int mu, int n, int m, int run) {
        int seed = generate_seed(mu, n, m, run);
        MachineSchedulingProblem problem = get_problem(seed, n, max_processing_time);
        auto [evaluate, diversity_measure, diversity_value] = get_eval_div_funcs(problem);
        auto [OPT, optimal_solution] = get_optimal_solution(problem, m, evaluate);
        auto start = std::chrono::high_resolution_clock::now();
        Population_Mu1<T,L>  opt_pop = mu1_unconstrained(
            seed, 1, n, mu,
            terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutation_operator, diversity_measure
        );
        auto stop = std::chrono::high_resolution_clock::now();
        std::string result_opt = get_csv_line("opt", seed, n, m, mu, run, opt_pop.get_generation(), n*n*mu, diversity_value(opt_pop.get_genes(true)), evaluate({opt_pop.get_bests(false, evaluate)[0]})[0], std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count());
        write_to_file(result_opt, output_file);

        start = std::chrono::high_resolution_clock::now();
        Population<T,L>  unopt_pop = mu1_unconstrained_unoptimized(
            seed, 1, n, mu,
            terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutation_operator, diversity_measure
        );
        stop = std::chrono::high_resolution_clock::now();
        std::string result_unopt = get_csv_line("unopt", seed, n, m, mu, run, unopt_pop.get_generation(), n*n*mu, diversity_value(unopt_pop.get_genes(true)), evaluate({unopt_pop.get_bests(false, evaluate)[0]})[0], std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count());
        write_to_file(result_unopt, output_file);
    };

    loop_parameters(mus, ns, ms, runs, mu1_optimization_test, true);
}

void test_algorithm(std::vector<int> mus, std::vector<int> ns, std::vector<int> ms, std::vector<double> alphas, int runs, std::string output_file, std::string algorithm, std::string operator_string, std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutation_operator){
   
    std::string header = "seed,n,m,mu,run,generations,max_generations,diversity,fitness,opt,algorithm,mutation";
    header += algorithm == "Mu1-const" ? ",alpha\n" : "\n";
    write_to_file(header, output_file, false);
    int max_processing_time = 50;

    auto algorithm_test = [output_file, max_processing_time, algorithm, mutation_operator, alphas, operator_string](int mu, int n, int m, int run) {
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
            result += get_csv_line(seed, n, m, mu, run, population.get_generation(), n*n*mu, diversity_value(population.get_genes(true)), evaluate({population.get_bests(false, evaluate)[0]})[0], OPT, algorithm, operator_string);
        }else if(algorithm == "Mu1-unconst"){
            Population<T,L> population = mu1_unconstrained(
                seed, m, n, mu,
                terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutation_operator, diversity_measure
            );
            result += get_csv_line(seed, n, m, mu, run, population.get_generation(), n*n*mu, diversity_value(population.get_genes(true)), evaluate({population.get_bests(false, evaluate)[0]})[0], OPT, algorithm, operator_string);
        }else if(algorithm == "Mu1-const"){
            for(double alpha: alphas){
                Population<T,L> population = mu1_constrained(
                    seed, m, n, mu,
                    terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutation_operator, diversity_measure,
                    alpha, optimal_solution
                );
                result += get_csv_line(seed, n, m, mu, run, population.get_generation(), n*n*mu, diversity_value(population.get_genes(true)), evaluate({population.get_bests(false, evaluate)[0]})[0], OPT, alpha, algorithm, operator_string);
            }
        }
        write_to_file(result, output_file);
    };

    loop_parameters(mus, ns, ms, runs, algorithm_test, true);
}

void test_noah(std::vector<int> mus, std::vector<int> ns, std::vector<int> ms, std::vector<double> alphas, int runs, double g_ratio, double r_ratio, double c_ratio, std::string output_file, std::string operator_string, std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutation_operator){
    
    std::string header = "seed,n,m,mu,run,generations,max_generations,diversity,fitness,opt,algorithm,mutation,alpha,g,r,c\n";
    write_to_file(header, output_file, false);
    int max_processing_time = 50;

    auto noah_test = [output_file, max_processing_time, mutation_operator, operator_string, alphas, g_ratio, r_ratio, c_ratio](int mu, int n, int m, int run) {
        int seed = generate_seed(mu, n, m, run);
        MachineSchedulingProblem problem = get_problem(seed, n, max_processing_time);
        auto [evaluate, diversity_measure, diversity_value] = get_eval_div_funcs(problem);
        auto [OPT, optimal_solution] = get_optimal_solution(problem, m, evaluate);
        int g = g_ratio*mu, r = r_ratio*mu, c = c_ratio*mu;
        std::cout << "mu: " << mu << ", n: " << n << ", m: " << m << ", run: " << run << "\n";
        std::string result;
        for(double alpha: alphas){
            Population<T,L> population = noah(
                seed, mu, n, m, 
                terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutation_operator, select_tournament(2, mu), diversity_measure,
                OPT*(1+alpha), g, r, c
            );
            result += get_csv_line(seed, n, m, mu, run, population.get_generation(), n*n*mu, diversity_value(population.get_genes(true)), evaluate({population.get_bests(false, evaluate)[0]})[0], OPT, "NOAH", operator_string, alpha, g, r, c);
        }
        write_to_file(result, output_file);
    };

    loop_parameters(mus, ns, ms, runs, noah_test, true);
}