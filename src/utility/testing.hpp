#pragma once

#include <iostream>
#include <chrono>

#include "../algorithms/simple.hpp"
#include "../algorithms/noah.hpp"
#include "../algorithms/mu1.hpp"
#include "../utility/printing.hpp"
#include "../utility/generating.hpp"
#include "../utility/documenting.hpp"
#include "../utility/solvers.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

void test_base(int n, int m, int mu, int seed, int max_processing_time){

    std::vector<int> processing_times = get_processing_times(seed, n, max_processing_time);
    std::vector<int> release_dates = get_release_dates(seed, processing_times);
    std::vector<int> due_dates = get_due_dates(seed, processing_times);

    std::function<std::vector<L>(const std::vector<T>&)> evaluate = evaluate_tardyjobs(processing_times, release_dates, due_dates);
    std::function<double(const T&, const T&)> diversity_measure = diversity_DFM();
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);

    Population<T,L> simple_pop = simple_test(
        seed,
        initialize_random(mu, processing_times.size(), m), evaluate, mutate_removeinsert(0.1), select_roulette(mu), select_mu(mu, evaluate),
        300
    );

    T best = simple_pop.get_bests(false, evaluate)[0];

    Population<T,L> noah_pop = noah(
        seed, m, n, mu, 
        terminate_generations(500), evaluate, mutate_removeinsert(0.1), select_roulette(mu), diversity_measure,
        0, 8, 15, 3
    );

    Population<T,L>  mu1_const_pop = mu1_constrained(
        seed, m, n, mu,
        terminate_generations(2500), evaluate, mutate_removeinsert(1), diversity_measure,
        0, best
    );

    Population<T,L>  mu1_unconst_pop = mu1_unconstrained(
        seed, m, n, mu,
        terminate_generations(2500), evaluate, mutate_removeinsert(1), diversity_measure
    );

    print(simple_pop, evaluate, diversity_value, "Simple", n, m);
    print(noah_pop, evaluate, diversity_value, "Noah", n, m);
    print(mu1_const_pop, evaluate, diversity_value, "Mu1 Constrained", n, m);
    print(mu1_const_pop, evaluate, diversity_value, "Mu1 Unconstrained", n, m);
}

void test_2mu_runtime(std::string operator_name, std::vector<int> ns, int m, int mu, int runs, std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate, std::function<double(int)> runtime_f){
    
    int max_processing_time = 50;

    std::function<double(const T&, const T&)> diversity_measure = diversity_DFM();
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);
    std::function<bool(Population<T,L>&)> termination_criterion = terminate_diversity(0, true, diversity_measure);

    int success_count = 0;

    for(int n : ns){        

        int runtime = 0;
        double expected = runtime_f(n);

        for(int i = 0; i < runs; i++){
        
            int seed = i;

            std::vector<int> processing_times = get_processing_times(seed, n, max_processing_time);
            std::vector<int> release_dates = get_release_dates(seed, processing_times);
            std::vector<int> due_dates = get_due_dates(seed, processing_times);

            std::function<std::vector<L>(const std::vector<T>&)> evaluate = evaluate_tardyjobs(processing_times, release_dates, due_dates);

            Population<T,L> mu1_pop = mu1_unconstrained(
                seed, m, n, mu,
                termination_criterion, evaluate, mutate, diversity_measure
            );

            if(mu1_pop.get_generation() <= expected){
                success_count++;
            }        
            runtime += mu1_pop.get_generation();
        }
        write_to_file(operator_name + "," + std::to_string(n) + "," + std::to_string(runtime/runs) + "," + std::to_string(runtime_f(n)), "runtimes.txt");
    }
    write_to_file(operator_name + "," + std::to_string(success_count) + "," + std::to_string(runs*ns.size()), "success_rates.txt");
}

void test_mu1_optimization(std::vector<int> mus, std::vector<int> ns, int runs){
    write_to_file("type,seed,n,mu,run,generations,max_generations,diversity,fitness,runtime\n", "test_mu1_optimization.txt", false);

    std::function<double(const T&, const T&)> diversity_measure = diversity_DFM();
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);

    int max_processing_time = 50;
    for(int n : ns){
        for(int mu : mus){
            if(mu > (n*n - n)/(n-1) ) continue;
            for(int run = 0; run < runs; run++) {
                std::cout << "n: " << n << ", mu: " << mu << ", run: " << run << std::endl;
                int seed = n*mu*mu+n*run*run+n;

                std::vector<int> processing_times = get_processing_times(seed, n, max_processing_time);
                std::vector<int> release_dates = get_release_dates(seed, processing_times);
                std::vector<int> due_dates = get_due_dates(seed, processing_times);

                std::function<std::vector<L>(const std::vector<T>&)> evaluate = evaluate_tardyjobs(processing_times, release_dates, due_dates);

                auto start = std::chrono::high_resolution_clock::now();
                Population_Mu1<T,L>  mu1_unconst_pop_opt = mu1_unconstrained(
                    seed, 1, n, mu,
                    terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutate_removeinsert(1), diversity_measure
                );
                auto stop = std::chrono::high_resolution_clock::now();
                std::string result_opt = "opt," + std::to_string(seed) + "," + std::to_string(n) + "," + std::to_string(mu) + "," + std::to_string(run+1) + "," + std::to_string(mu1_unconst_pop_opt.get_generation()) + "," + std::to_string(n*n*mu) + "," + std::to_string(diversity_value(mu1_unconst_pop_opt.get_genes(true))) + "," + std::to_string(evaluate({mu1_unconst_pop_opt.get_bests(false, evaluate)[0]})[0]) + "," + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()) + "\n";
                write_to_file(result_opt, "test_mu1_optimization.txt");

                start = std::chrono::high_resolution_clock::now();
                Population<T,L>  mu1_unconst_pop_unopt = mu1_unconstrained_unoptimized(
                    seed, 1, n, mu,
                    terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutate_removeinsert(1), diversity_measure
                );
                stop = std::chrono::high_resolution_clock::now();
                std::string result_unopt = "unopt," + std::to_string(seed) + "," + std::to_string(n) + "," + std::to_string(mu) + "," + std::to_string(run+1) + "," + std::to_string(mu1_unconst_pop_unopt.get_generation()) + "," + std::to_string(n*n*mu) + "," + std::to_string(diversity_value(mu1_unconst_pop_unopt.get_genes(true))) + "," + std::to_string(evaluate({mu1_unconst_pop_unopt.get_bests(false, evaluate)[0]})[0]) + "," + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()) + "\n";
                write_to_file(result_unopt, "test_mu1_optimization.txt");
            }
        }
    }
}

void test_mu1(std::vector<int> mus, std::vector<int> ns, std::vector<int> ms, std::vector<double> alphas, int runs, bool constrained){

    std::string filename_unconst = "test_mu1_unconstrained.txt";
    std::string filename_const = "test_mu1_constrained.txt";

    if(!constrained) write_to_file("seed,n,m,mu,run,generations,max_generations,diversity,fitness,opt\n", filename_unconst, false);
    if(constrained) write_to_file("seed,n,m,mu,run,generations,max_generations,diversity,fitness,opt,alpha\n", filename_const, false);

    std::function<double(const T&, const T&)> diversity_measure = diversity_DFM();
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);

    int max_processing_time = 50;
    for(int n : ns){
        for(int mu : mus){
            if(mu > (n*n - n)/(n-1) ) continue;
            for(int m : ms){
                if(m > n) continue;
                for(int run = 0; run < runs; run++) {

                    std::cout << "n: " << n << ", mu: " << mu << ", m: " << m << ", run: " << run << "\n";
                    int seed = n*mu*mu+n*run*run+n+m*run+m;

                    std::vector<int> processing_times = get_processing_times(seed, n, max_processing_time);
                    std::vector<int> release_dates = get_release_dates(seed, processing_times);
                    std::vector<int> due_dates = get_due_dates(seed, processing_times);

                    std::function<std::vector<L>(const std::vector<T>&)> evaluate = evaluate_tardyjobs(processing_times, release_dates, due_dates);
                    
                    T optimal_solution;
                    if(m == 1) optimal_solution = {moores_algorithm(processing_times, due_dates)};
                    else optimal_solution = approximation_algorithm(processing_times, due_dates, m);
                    int OPT = evaluate({optimal_solution})[0];

                    if(!constrained){
                        Population<T,L>  mu1_unconst_pop = mu1_unconstrained(
                            seed, m, n, mu,
                            terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutate_removeinsert(1), diversity_measure
                        );
                        std::string result_unconst = std::to_string(seed) + "," + std::to_string(n) + "," + std::to_string(m) + "," + std::to_string(mu) + "," + std::to_string(run+1) + "," + std::to_string(mu1_unconst_pop.get_generation()) + "," + std::to_string(n*n*mu) + "," + std::to_string(diversity_value(mu1_unconst_pop.get_genes(true))) + "," + std::to_string(evaluate({mu1_unconst_pop.get_bests(false, evaluate)[0]})[0]) + "," + std::to_string(OPT) + "\n";
                        write_to_file(result_unconst, filename_unconst);
                    }
                    if(constrained){
                        for(double alpha: alphas){
                            Population<T,L>  mu1_const_pop = mu1_constrained(
                                seed, m, n, mu,
                                terminate_diversitygenerations(1, true, diversity_measure, n*n*mu), evaluate, mutate_removeinsert(1), diversity_measure,
                                alpha, optimal_solution
                            );
                            std::string result_const = std::to_string(seed) + "," + std::to_string(n) + "," + std::to_string(m) + "," + std::to_string(mu) + "," + std::to_string(run+1) + "," + std::to_string(mu1_const_pop.get_generation()) + "," + std::to_string(n*n*mu) + "," + std::to_string(diversity_value(mu1_const_pop.get_genes(true))) + "," + std::to_string(evaluate({mu1_const_pop.get_bests(false, evaluate)[0]})[0]) + "," + std::to_string(OPT) + "," + std::to_string(alpha) + "\n";
                            write_to_file(result_const, filename_const);
                        }   
                    }                                     
                }
            }
        }
    }
}