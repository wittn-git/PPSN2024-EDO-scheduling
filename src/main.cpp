#include "utility/testing.hpp"

using T = std::vector<std::vector<int>>;

int main() {
    //test_all(15, 2);

    std::vector<int> ns = {5, 10, 35, 50};
    std::vector<int> mus = {2, 5, 10};
    int runs = 50;
    for(auto mu : mus){
        test_2mu_nswap_runtime(ns, mu, runs);
        test_2mu_rai_runtime(ns, mu, runs);
    }
    
}