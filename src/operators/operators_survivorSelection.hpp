#include <functional>
#include <vector>
#include <numeric>

using T = std::vector<std::vector<int>>;
using L = int;

// Survivor selection operators ----------------------------------------------------

/*
    mu-Selection: Selects the best mu individuals from the combined population of parents and offspring
    Arguments:
        - mu:       number of individuals to select
        - evaluate: function taking a vector of genes and returning a vector of fitnesses
*/

std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&)> select_mu(int mu, std::function<std::vector<L>(const std::vector<T>&)> evaluate) {
    return [mu, evaluate](const std::vector<T>& parents, const std::vector<L>& fitnesses_parents, const std::vector<T>& offspring) -> std::vector<T> {
        std::vector<T> combined = parents;
        combined.insert(combined.end(), offspring.begin(), offspring.end());
        std::vector<L> fitnesses = fitnesses_parents;
        std::vector<L> fitnesses_children = evaluate(offspring);
        fitnesses.insert(fitnesses.end(), fitnesses_children.begin(), fitnesses_children.end());
        std::vector<T> selected_genes(mu);
        std::vector<int> indices(combined.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::partial_sort(indices.begin(), indices.begin() + mu, indices.end(), [&](int a, int b) {
            return fitnesses[a] > fitnesses[b];
        });
        for (int i = 0; i < mu; i++) {
            selected_genes[i] = combined[indices[i]];
        }
        return selected_genes;
    };
};