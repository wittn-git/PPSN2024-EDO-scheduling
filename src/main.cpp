#include "algorithms/simple_test.hpp"
#include "algorithms/noah.hpp"
#include "algorithms/mu1.hpp"
#include "utility/printing.hpp"
#include "utility/generating.hpp"

using T = std::vector<std::vector<int>>;

int main() {

    /*std::function<double(const T&, const T&)> diversity_measure = diversity_DFM();
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_survivors = select_div(diversity_measure);
    std::mt19937 rng(945845);
    std::vector<T> parents = {
        {{1,2,3}},
        {{1,2,3}}
    };
    std::vector<T> children = {
        {{3,2,1}}
    };

    std::vector<T> surv = select_survivors(parents, {{}}, children, rng);
    for(auto& s : surv){
        for(auto& j : s){
            for(auto& t : j){
                std::cout << t << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }*/

    int m = 2;
    int n = 15;
    int max_processing_time = 50;
    int seed = 945845;

    std::vector<int> processing_times = get_processing_times(seed, n, max_processing_time);
    std::vector<int> release_dates = get_release_dates(seed, n, processing_times);
    std::vector<int> due_dates = get_due_dates(seed, n, processing_times, release_dates);

    std::function<std::vector<L>(const std::vector<T>&)> evaluate = evaluate_tardyjobs(processing_times, release_dates, due_dates);
    std::function<double(const T&, const T&)> diversity_measure = diversity_DFM();
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);

    Population<T,L> simple_pop = simple_test(
        seed, m, processing_times, release_dates, due_dates,
        evaluate, 
        10, 100, 3, 0.1
    );

    double best = evaluate(simple_pop.get_bests(false, evaluate))[0];

    Population<T,L> noah_pop = noah_test(
        seed, m, processing_times, release_dates, due_dates,
        evaluate, diversity_measure,
        50, best, 3, 25, 10, 25, 0.1, 3
    );

    Population<T,L>  mu1_pop = mu1_test(
        seed, m, processing_times, release_dates, due_dates,
        evaluate, diversity_measure,
        50, 0, best, 1, 50, simple_pop.get_genes(true)
    );

    print(simple_pop, evaluate, diversity_value, "Simple");
    print(noah_pop, evaluate, diversity_value, "Noah");
    print(mu1_pop, evaluate, diversity_value, "Mu1");
}