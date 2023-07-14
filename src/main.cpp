#include "utility/testing.hpp"

using T = std::vector<std::vector<int>>;

int main() {
    //test_all(15, 2);

    std::vector<int> ns = {3, 5, 10, 35, 50, 100, 200};
    int runs = 50;
    test_2mu_nswap_runtime(ns, 2, runs);
    test_2mu_rai_runtime(ns, 2, runs);   
}