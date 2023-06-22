#include "algorithms/simple_test.hpp"
#include "algorithms/noah.hpp"
#include "algorithms/mu1.hpp"
#include "utility/printing.hpp"
#include "utility/generating.hpp"

int main() {

    int m = 1;
    int n = 50;
    int max_processing_time = 50;
    int seed = 0;

    std::vector<int> processing_times = get_processing_times(seed, n, max_processing_time);
    std::vector<int> release_dates = get_release_dates(seed, n, processing_times);
    std::vector<int> due_dates = get_due_dates(seed, n, processing_times, release_dates);

    std::function<std::vector<L>(const std::vector<T>&)> evaluate = evaluate_tardyjobs(processing_times, release_dates, due_dates);
    std::function<double(const T&, const T&)> diversity_measure = diversity_DFM();
    std::function<double(const std::vector<T>&)> diversity_value = diversity_vector(diversity_measure);

    Population<T,L> simple_pop = simple_test(
        seed, m, processing_times, release_dates, due_dates,
        evaluate, 
        100, 100
    );

    Population<T,L> noah_pop = noah_test(
        seed, m, processing_times, release_dates, due_dates,
        evaluate, diversity_measure,
        50, 60, 5, 25, 3, 100, 0.1, 3
    );

    double best = evaluate(simple_pop.get_bests(false, evaluate))[0];
    Population<T,L>  mu1_pop = mu1_test(
        seed, m, processing_times, release_dates, due_dates,
        evaluate, diversity_measure,
        50, 0.2, -45, 0.1, 100
    );

    print(simple_pop, evaluate, diversity_value, "Simple");
    print(noah_pop, evaluate, diversity_value, "Noah");
    print(mu1_pop, evaluate, diversity_value, "Mu1");
}