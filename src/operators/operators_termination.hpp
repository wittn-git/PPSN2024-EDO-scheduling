#pragma once

#include <functional>
#include <vector>

#include "../population/population.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

// Termination operators ------------------------------------------------------

std::function<bool(Population<T,L>&)> terminate_generations(int max_generations){
    return [max_generations](Population<T,L>& population) -> bool {
        return population.get_generation() >= max_generations;
    };
}