#pragma once

#include <functional>
#include <vector>
#include <numeric>
#include <random>
#include <assert.h>

using T = std::vector<std::vector<int>>;
using L = double;

// Survivor selection operators ----------------------------------------------------

/*
    mu-Selection: Selects the best mu individuals from the combined population of parents and offspring
    Arguments:
        - mu:       number of individuals to select
        - evaluate: function taking a vector of genes and returning a vector of fitnesses
*/

std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_mu(int mu, std::function<std::vector<L>(const std::vector<T>&)> evaluate) {
    return [mu, evaluate](const std::vector<T>& parents, const std::vector<L>& fitnesses_parents, const std::vector<T>& offspring, std::mt19937& generator) -> std::vector<T> {
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

/*
    div-Selection: Selects the mu (=parent size) individuals with the highest diversity from the combined population of parents and one offspring
    Arguments
        - diversity_measure:    function taking a vector of genes and returning a double representing the diversity
*/

std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_div(std::function<double(const std::vector<T>&)> diversity_measure) {
    return [diversity_measure](const std::vector<T>& parents, const std::vector<L>& fitnesses_parents, const std::vector<T>& offspring, std::mt19937& generator) -> std::vector<T> {
        assert(offspring.size() == 1);
        std::vector<T> selected_genes = parents;
        selected_genes.emplace_back(offspring[0]);
        std::vector<double> diversities(selected_genes.size());
        for (int i = 0; i < selected_genes.size(); i++) {
            std::vector<T> sub_selected_genes = selected_genes;
            sub_selected_genes.erase(sub_selected_genes.begin() + i);
            diversities[i] = diversity_measure(sub_selected_genes);
        }
        int min_index = std::distance(diversities.begin(), std::min_element(diversities.begin(), diversities.end()));
        selected_genes.erase(selected_genes.begin() + min_index);
        return selected_genes;
    };
};