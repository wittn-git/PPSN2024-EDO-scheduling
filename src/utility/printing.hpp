#include <iostream>

#include "../population/population.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

void print(
    Population<T,L>& population,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<double(const std::vector<T>&)> diversity_value,
    std::string algorithm_name
){
    std::cout << algorithm_name << "\n";
    std::cout << "Generations: " << population.get_generation() << "\n";
    std::cout << "Individuals: " << population.get_genes().size() << "\n";
    std::cout << "Diversity_score: " << 1/diversity_value(population.get_genes()) << "\n";
    std::cout << "Best Individuals: " << "\n";
    std::cout << population.bests_to_string(evaluate) << "\n";
}