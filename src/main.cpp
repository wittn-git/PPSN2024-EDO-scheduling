#include "utility/testing.hpp"

using T = std::vector<std::vector<int>>;

int main() {
    //int n = 15, m = 2;
    //test_all(15, 2);

    std::vector<int> ns = {3, 5, 10, 35, 50, 100, 200};
    int runs = 50;
    auto runtime_linearithmic = [](int n) {
        return n * std::log(n);
    };
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_rai = mutate_removeinsert(1);
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_nswap = mutate_neighborswap(1);
    test_2mu_runtime("RAI", ns, 1, 2, runs, mutate_rai, runtime_linearithmic);
    test_2mu_runtime("NSWAP", ns, 1, 2, runs, mutate_nswap, runtime_linearithmic);
}