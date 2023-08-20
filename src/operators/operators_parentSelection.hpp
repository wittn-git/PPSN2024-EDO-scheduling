#pragma once

#include <functional>
#include <vector>
#include <random>
#include <assert.h>

using T = std::vector<std::vector<int>>;
using L = double;

// Parent Selection Operators -------------------------------------------------------

/*
    Roulette Selection: Selection from all individuals using a roulette simulation, where higher fitness translates to higher probability
    Arguments:
        - parent_count: number of individuals to select
*/

std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_roulette(int parent_count) {
    return [parent_count](const std::vector<T>& genes, const std::vector<L>& fitnesses, std::mt19937& generator) -> std::vector<T> {
        std::vector<T> selected_genes(parent_count);
        double total_fitness = std::accumulate(fitnesses.begin(), fitnesses.end(), 0.0);
        if(total_fitness == 0){
            std::uniform_int_distribution< int > distribute_point(0, genes.size() - 1 );
            std::transform(selected_genes.begin(), selected_genes.end(), selected_genes.begin(), [&](T& selected_gene) mutable -> T {
                int rand_index = distribute_point(generator);
                return genes[rand_index];
            });
            return selected_genes;
        }
        std::vector<double> probabilities(parent_count);
        std::transform(fitnesses.begin(), fitnesses.end(), probabilities.begin(), [&](double fitness) {
            return fitness / total_fitness;
        });
        std::partial_sum(probabilities.begin(), probabilities.end(), probabilities.begin());
        std::uniform_real_distribution< double > distribute_value(0, 1);
        std::transform(selected_genes.begin(), selected_genes.end(), selected_genes.begin(), [&](T& selected_gene) mutable -> T {
            double rand_value = distribute_value(generator);
            auto it = std::upper_bound(probabilities.begin(), probabilities.end(), rand_value);
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
        - parent_count:    number of individuals to select
*/

std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_tournament(int tournament_size, int parent_count) {
    return [tournament_size, parent_count](const std::vector<T>& genes, const std::vector<L>& fitnesses, std::mt19937& generator) -> std::vector<T> {
        assert(tournament_size <= genes.size());
        int selected_genes_n = parent_count > genes.size() ? genes.size() : parent_count;
        std::vector<T> selected_genes(parent_count);
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

/*
    Random Parent Selection: Select subgroup of specified size randomly
    Arguments:
        - parent_size: number of individuals to select
*/

std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_random(int parent_count) {
    return [parent_count](const std::vector<T>& genes, const std::vector<L>& fitnesses, std::mt19937& generator) -> std::vector<T> {
        std::vector<T> selected_genes(parent_count);
        std::uniform_int_distribution< int > distribute_point(0, genes.size() - 1 );
        std::transform(selected_genes.begin(), selected_genes.end(), selected_genes.begin(), [&](T& selected_gene) mutable -> T {
            int rand_index = distribute_point(generator);
            return genes[rand_index];
        });
        return selected_genes;
    };
}

/*
    All Parent Selection: Select all individuals
*/

std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)> select_all() {
    return [](const std::vector<T>& genes, const std::vector<L>& fitnesses, std::mt19937& generator) -> std::vector<T> {
        return genes;
    };
}