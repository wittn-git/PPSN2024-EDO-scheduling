#pragma once

#include <functional>
#include <vector>

#include "../population/population.hpp"
#include "../operators/operators_diversity.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

// Termination operators ------------------------------------------------------

/*
    Generation termination: Terminate after a certain number of generations.
    Args:
        max_generations: maximum number of generations
*/
std::function<bool(Population<T,L>&)> terminate_generations(int max_generations){
    return [max_generations](Population<T,L>& population) -> bool {
        return population.get_generation() >= max_generations;
    };
}

/*
    Diversity termination: Terminate when the diversity is higher than a certain threshold.
    Args:
        threshold:          threshold for the diversity
        diversity_measure:  diversity measure to use
*/
std::function<bool(Population<T,L>&)> terminate_diversity(double threshold, std::function<double(const T&, const T&)> diversity_measure){
    std::function<double(const std::vector<T>&)> div_vector = diversity_vector(diversity_measure);
    return [div_vector, threshold](Population<T,L>& population) -> bool {
        return div_vector(population.get_genes(true)) >= threshold;
    };
}

/*
    Diversity or generation termination: Terminate when the diversity is higher than a certain threshold or after a certain number of generations.
    Args:
        threshold:          threshold for the diversity
        diversity_measure:  diversity measure to use
        max_generations:    maximum number of generations
*/
std::function<bool(Population<T,L>&)> terminate_diversitygenerations(double threshold, std::function<double(const T&, const T&)> diversity_measure, int max_generations){
    std::function<double(const std::vector<T>&)> div_vector = diversity_vector(diversity_measure);
    return [div_vector, threshold, max_generations](Population<T,L>& population) -> bool {
        return div_vector(population.get_genes(true)) >= threshold || population.get_generation() >= max_generations;
    };
}