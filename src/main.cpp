#include "utility/testing.hpp"
#include "utility/solvers.hpp"

using T = std::vector<std::vector<int>>;

void test_mutation_runtimes(){
    std::vector<int> ns = {3, 5, 10, 35, 50, 100, 200};
    int runs = 50;
    std::function<double(int)> runtime_linearithmic = [](int n) {
        return n * std::log(n);
    };
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_rai = mutate_removeinsert(1);
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_nswap = mutate_neighborswap(1);
    test_2mu_runtime("RAI", ns, 1, 2, runs, mutate_rai, runtime_linearithmic);
    test_2mu_runtime("NSWAP", ns, 1, 2, runs, mutate_nswap, runtime_linearithmic);
}

void test_algorithms_basic(){
    test_base(15, 2, 10, 0, 50);
}

void emperical_experiments(){
    std::vector<int> ns = {3, 5, 10, 35, 50, 100, 200};
    std::vector<int> mus = {2, 5, 10, 20, 50, 100};
    int runs = 20;
    test_mu1_unconstrained_single(mus, ns, runs);
}

int main(){
    emperical_experiments();
    return 0;
}