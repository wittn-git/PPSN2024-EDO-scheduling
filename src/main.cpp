#include <chrono>

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

void mu1_optimization_test(){
    std::vector<int> ns = {15};
    std::vector<int> mus = {10};
    int runs = 30;
    test_mu1_optimization(mus, ns, runs);
}

void emperical_experiments(){
    std::vector<int> ns = {3, 5, 10, 35, 50};
    std::vector<int> mus = {2, 5, 10, 20, 50};
    std::vector<int> ms = {1, 3, 5, 10};
    std::vector<double> alphas = {0.2, 0.5, 1};
    int runs = 30;
    test_mu1(mus, ns, ms, alphas, runs);
}

int main(){

    // declare df mdiversity measure
    std::function<double(const T& , const T&)> df = diversity_DFM();
    T gene1 = {{}, {0,1}, {2}};
    T gene2 = {{}, {0,1}, {2}};
    // decalre diversity vector
    std::function<double(const std::vector<T>&)> diversity = diversity_vector(df);
   // std::cout << (euclideanNorm(diversity_scores) / ((n-m) * std::sqrt((mu * mu - mu)/2))) << std::endl;
    emperical_experiments();
    return 0;
}