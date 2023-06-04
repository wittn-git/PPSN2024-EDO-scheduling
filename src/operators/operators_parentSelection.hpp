#include <functional>
#include <vector>
#include <random>

using T = std::vector<std::vector<int>>;
using L = int;

// Parent Selection Operators -------------------------------------------------------

/*
    Roulette Selection: Selection from all individuals using a roulette simulation, where higher fitness translates to higher probability
    Arguments:
        - evaluate: function taking a vector of genes and returning a vector of fitnesses
*/

std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_roulette() {
    return [](const std::vector<T>& genes, const std::vector<L>& fitnesses, std::mt19937& generator) -> std::vector<T> {
        std::vector<T> selected_genes(fitnesses.size());
        double total_fitness = std::accumulate(fitnesses.begin(), fitnesses.end(), 0.0);
        std::vector<double> probabilities(fitnesses.size());
        std::transform(fitnesses.begin(), fitnesses.end(), probabilities.begin(), [&](double fitness) {
            return fitness / total_fitness;
        });
        std::partial_sum(probabilities.begin(), probabilities.end(), probabilities.begin());
        std::uniform_real_distribution< double > distribute_rate(0, 1);
        std::transform(selected_genes.begin(), selected_genes.end(), selected_genes.begin(), [&](T& selected_gene) mutable -> T {
            auto it = std::upper_bound(probabilities.begin(), probabilities.end(), distribute_rate(generator));
            int index = std::distance(probabilities.begin(), it);
            return genes[index];
        });
        return selected_genes;
    };
}

/*
    Tournament Parent Selection: Take a random subgroup of a specified size and choose the one with the highest fitness value
    Arguments:
        - tournament_size: size of the chosen subgroup
        - evaluate:        function taking a vector of genes and returning a vector of fitnesses
*/

std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_tournament(int tournament_size) {
    return [tournament_size](const std::vector<T>& genes, const std::vector<L>& fitnesses, std::mt19937& generator) -> std::vector<T> {
        std::vector<T> selected_genes(genes.size());
        std::uniform_int_distribution< int > distribute_point(0, genes.size() - 1 );
        std::transform(selected_genes.begin(), selected_genes.end(), selected_genes.begin(), [&](T& selected_gene) mutable -> T {
            std::vector<int> tournament_genes(tournament_size);
            for (int i = 0; i < tournament_size; i++) {
                int rand_index = distribute_point(generator);
                tournament_genes[i] = rand_index;
            }
            auto max_it = std::max_element(tournament_genes.begin(), tournament_genes.end(), [&](int a, int b) {
                return fitnesses[a] < fitnesses[b];
            });
            return genes[*max_it];
        });
        return selected_genes;
    };
}