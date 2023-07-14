#include "../algorithms/simple.hpp"
#include "../algorithms/noah.hpp"
#include "../algorithms/mu1.hpp"
#include "../utility/printing.hpp"
#include "../utility/generating.hpp"
#include "../utility/documenting.hpp"

#include <tuple>

using T = std::vector<std::vector<int>>;

void test_all(int n, int m){

    int max_processing_time = 50;
    int seed = 945845;

    std::vector<int> processing_times = get_processing_times(seed, n, max_processing_time);
    std::vector<int> release_dates = get_release_dates(seed, n, processing_times);
    std::vector<int> due_dates = get_due_dates(seed, n, processing_times, release_dates);

    std::function<std::vector<L>(const std::vector<T>&)> evaluate = evaluate_tardyjobs(processing_times, release_dates, due_dates);
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_high = mutate_swap(1);
    std::function<std::vector<T>(const std::vector<T>&,  std::mt19937&)> mutate_low = mutate_swap(0.1);;
    std::function<double(const T&, const T&)> diversity_measure = diversity_DFM();
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);

    Population<T,L> simple_pop = simple_test(
        seed, m, processing_times, release_dates, due_dates,
        evaluate, mutate_low,
        10, 100, 3
    );

    double best = evaluate(simple_pop.get_bests(false, evaluate))[0];

    Population<T,L> noah_pop = noah_test(
        seed, m, processing_times, release_dates, due_dates,
        evaluate, mutate_low, terminate_generations(25), diversity_measure,
        50, best, 3, 25, 10, 3
    );

    Population<T,L>  mu1_pop = mu1_test(
        seed, m, processing_times, release_dates, due_dates,
        evaluate, mutate_high, terminate_generations(50), diversity_measure,
        50, 0, best, simple_pop.get_genes(true)
    );

    print(simple_pop, evaluate, diversity_value, "Simple", n, m);
    print(noah_pop, evaluate, diversity_value, "Noah", n, m);
    print(mu1_pop, evaluate, diversity_value, "Mu1", n, m);
}

std::tuple<int, double> test_2mu_runtime(int n, int m, int mu, int runs, std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate, auto runtime_f){
    
    int max_processing_time = 50;

    std::function<double(const T&, const T&)> diversity_measure = diversity_DFM();
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);
    std::function<bool(Population<T,L>&)> termination_criterion = terminate_diversitygenerations(-0.1, diversity_measure, 2*n*std::log(n));

    int success_count = 0;
    double expected = runtime_f(n);
    int runtime = 0;

    for(int i = 0; i < runs; i++){
        
        int seed = i;

        std::vector<int> processing_times = get_processing_times(seed, n, max_processing_time);
        std::vector<int> release_dates = get_release_dates(seed, n, processing_times);
        std::vector<int> due_dates = get_due_dates(seed, n, processing_times, release_dates);

        std::function<std::vector<L>(const std::vector<T>&)> evaluate = evaluate_tardyjobs(processing_times, release_dates, due_dates);

        Population<T,L> mu1_pop = mu1_test(
            seed, m, processing_times, release_dates, due_dates,
            evaluate, mutate, termination_criterion, diversity_measure,
            mu, 0, -n, {}
        );

        if(mu1_pop.get_generation() <= expected){
            success_count++;
        }        
        runtime += mu1_pop.get_generation();
    }
    return {success_count, runtime/runs};
}

void test_2mu_nswap_runtime(std::vector<int> ns, int mu, int runs){
    std::cout << "Testing 2mu runtime for n-swap with mu = " << mu << std::endl;
    int success_count = 0;
    auto runtime_f = [](int n) {
        return n * std::log(n);
    };
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_nswp = mutate_neighborswap(1);
    for(int n : ns){
        std::tuple<int, int> result = test_2mu_runtime(n, 1, mu, runs, mutate_nswp, runtime_f);
        success_count += std::get<0>(result);
        std::cout << "For n = " << n << ", avg runtime " << std::get<1>(result) << " ; worst case " << runtime_f(n) << std::endl;
        write_to_file("NSWAP,"+ std::to_string(n)+","+std::to_string(std::get<1>(result))+","+std::to_string(runtime_f(n)), "runtimes.txt");
    }
    std::cout << "Out of " << runs*ns.size() << " runs, " << success_count << " were successful (runtime check n-swap)" << std::endl;
    std::cout << "End of 2mu runtime test for n-swap" << std::endl;
    write_to_file("NSWAP,"+ std::to_string(success_count)+","+std::to_string(runs*ns.size()), "success_rates.txt");
}

void test_2mu_rai_runtime(std::vector<int> ns, int mu, int runs){
    std::cout << "Testing 2mu runtime for r+i with mu = " << mu << std::endl;
    int success_count = 0;
    auto runtime_f = [](int n) {
        return n * n * std::log(n);
    };
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_rai = mutate_removeinsert(1);
    for(int n : ns){
        std::tuple<int, int> result = test_2mu_runtime(n, 1, mu, runs, mutate_rai, runtime_f);
        success_count += std::get<0>(result);
        std::cout << "For n = " << n << ", avg runtime " << std::get<1>(result) << " ; worst case " << runtime_f(n) << std::endl;
        write_to_file("RAI,"+ std::to_string(n)+","+std::to_string(std::get<1>(result))+","+std::to_string(runtime_f(n)), "runtimes.txt");
    }
    
    std::cout << "Out of " << runs*ns.size() << " runs, " << success_count << " were successful (runtime check r+i)" << std::endl;
    std::cout << "End of 2mu runtime test for r+i" << std::endl;
    write_to_file("RAI,"+ std::to_string(success_count)+","+std::to_string(runs*ns.size()), "success_rates.txt");
}