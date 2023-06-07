#include <functional>
#include <vector>

#include "../population/population.hpp"

// Termination operators ------------------------------------------------------

using T = std::vector<std::vector<int>>;
using L = double;

std::function<bool(const Population<T,L>&)> terminate_generations(int max_generations){
    return [max_generations](const Population<T,L>& population) -> bool {
        return population.get_generation() >= max_generations;
    };
}