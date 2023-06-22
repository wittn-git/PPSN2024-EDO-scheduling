#pragma once

#include <functional>
#include <vector>
#include <numeric>
#include <random>
#include <map>
#include <tuple>
#include <assert.h>
#include "operators_diversity.hpp"

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
        - diversity_measure:    function taking two genes and returning a double representing the diversity
*/

std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_div(std::function<double(const T&, const T&)> diversity_measure) {
    return [diversity_measure](const std::vector<T>& parents, const std::vector<L>& fitnesses_parents, const std::vector<T>& offspring, std::mt19937& generator) -> std::vector<T> {
        assert(offspring.size() == 1);
        std::vector<T> selected_genes = parents;
        selected_genes.emplace_back(offspring[0]);
        std::map<std::tuple<int, int>, double> diversity_scores;
        for(int i = 0; i < selected_genes.size(); i++){
            for(int j = i + 1; j < selected_genes.size(); j++){
                diversity_scores[{i,j}] = diversity_measure(selected_genes[i], selected_genes[j]);
            }
        }
        std::vector<int> indices(selected_genes.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::vector<double> diversity_values(selected_genes.size());
        std::function<double(const std::vector<double>&)> div_value = diversity_vector();
        for(auto index_it = indices.begin(); index_it != indices.end(); ++index_it){
            std::vector<double> div_vector;
            for(auto score_it = diversity_scores.begin(); score_it != diversity_scores.end(); ++score_it){
                if(std::get<0>((*score_it).first) != *index_it && std::get<1>((*score_it).first) != *index_it){
                    div_vector.emplace_back((*score_it).second);
                }
            }
            diversity_values.emplace_back(div_value(div_vector));
        }
        auto min_it = std::max_element(indices.begin(), indices.end(), [&](int a, int b) {
            return diversity_values[a] < diversity_values[b];
        });
        selected_genes.erase(selected_genes.begin() + *min_it);
        return selected_genes;
    };
};

/*
    qdiv-Selection: Selects the mu (=parent size) individuals with the highest diversity from the combined population of parents and offspring, if quality of offspring is at least OPT * (1+alpha)
    Arguments:
        - alpha:                parameter for quality threshold
        - OPT:                  fitness value of optimal solution
        - diversity_measure:    function taking two genes and returning a double representing the diversity
        - evaluate:             function taking a vector of genes and returning a vector of fitnesses
*/

std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_qdiv(double alpha, double OPT, std::function<double(const T&, const T&)> diversity_measure, std::function<std::vector<L>(const std::vector<T>&)> evaluate) {
    return [alpha, OPT, diversity_measure, evaluate](const std::vector<T>& parents, const std::vector<L>& fitnesses_parents, const std::vector<T>& offspring, std::mt19937& generator) -> std::vector<T> {
        assert(offspring.size() == 1);
        if(evaluate(offspring)[0] < OPT * (1 + alpha)) return parents;
        std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> div = select_div(diversity_measure);
        return div(parents, fitnesses_parents, offspring, generator);
    };
};