#include <iostream>

#include "../population/population.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

void print(
    Population<T,L>& population,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<double(const std::vector<T>&)> diversity_value,
    std::string algorithm_name,
    int n,
    int m
){
    std::cout << algorithm_name << "\n";
    std::cout << "Generations: " << population.get_generation() << "\n";
    std::cout << "Individuals: " << population.get_size(true) << "\n";
    std::cout << "Diversity score: " << diversity_value(population.get_genes(true)) << "\n";
    std::cout << "Diversity percentage: " << -diversity_value(population.get_genes(true)) / ((n-m) * std::sqrt((population.get_size(true) * population.get_size(true) - population.get_size(true))/2)) << "\n";
    std::cout << "Unique individuals: " << population.get_size(false) << "\n";
    std::vector<T> bests = population.get_bests(false, evaluate);
    std::vector<L> fitnesses__bests = evaluate(bests);
    std::cout << "Best fitness: " << fitnesses__bests[0] << "\n";
    std::cout << "Number of unique best individuals: " << bests.size() << "\n";
    std::cout << "\n";
}