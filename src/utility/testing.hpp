#include "../algorithms/simple.hpp"
#include "../algorithms/noah.hpp"
#include "../algorithms/mu1.hpp"
#include "../utility/printing.hpp"
#include "../utility/generating.hpp"
#include "../utility/documenting.hpp"

#include <tuple>

using T = std::vector<std::vector<int>>;
using L = double;

void test_base(int n, int m, int mu, int seed, int max_processing_time){

    std::vector<int> processing_times = get_processing_times(seed, n, max_processing_time);
    std::vector<int> release_dates = get_release_dates(n);
    std::vector<int> due_dates = get_due_dates(seed, processing_times);

    std::function<std::vector<L>(const std::vector<T>&)> evaluate = evaluate_tardyjobs(processing_times, release_dates, due_dates);
    std::function<double(const T&, const T&)> diversity_measure = diversity_DFM();
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);

    Population<T,L> simple_pop = simple_test(
        seed,
        initialize_random(mu, processing_times.size(), m), evaluate, mutate_removeinsert(0.1), select_tournament(3, mu), select_mu(mu, evaluate),
        300
    );

    T best = simple_pop.get_bests(false, evaluate)[0];

    Population<T,L> noah_pop = noah(
        seed, m, n, mu, 
        terminate_generations(500), evaluate, mutate_removeinsert(0.1), select_tournament(3, mu), diversity_measure,
        0, 8, 15, 3
    );

    Population<T,L>  mu1_const_pop = mu1_constrained(
        seed, m, n, mu,
        terminate_generations(300), evaluate, mutate_removeinsert(1), diversity_measure,
        1, best
    );

    Population<T,L>  mu1_unconst_pop = mu1_unconstrained(
        seed, m, n, mu,
        terminate_generations(50), evaluate, mutate_removeinsert(1), diversity_measure
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
            std::vector<int> release_dates = get_release_dates(n);
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