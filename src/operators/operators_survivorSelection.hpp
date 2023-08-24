#pragma once

#include <functional>
#include <vector>
#include <numeric>
#include <random>
#include <map>
#include <tuple>
#include <assert.h>

#include "operators_diversity.hpp"
#include "../population/population_mu1.hpp"

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
    div-Selection: Returns a vector of combined parents and offspring removing the individual which yields the highest diversity value when removed
    Arguments
        - diversity_measure:    function taking two genes and returning a double representing the diversity
*/

std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_div(std::function<double(const T&, const T&)> diversity_measure) {
    return [diversity_measure](const std::vector<T>& parents, const std::vector<L>& fitnesses_parents, const std::vector<T>& offspring, std::mt19937& generator) -> std::vector<T> {
        std::vector<T> selected_genes = parents;
        selected_genes.insert(selected_genes.end(), offspring.begin(), offspring.end());
        std::vector<std::tuple<int, int, double>> diversity_scores;
        diversity_scores.reserve(selected_genes.size() * (selected_genes.size() - 1) / 2);
        for(int i = 0; i < selected_genes.size(); i++){
            for(int j = i + 1; j < selected_genes.size(); j++){
                diversity_scores.emplace_back(i, j, diversity_measure(selected_genes[i], selected_genes[j]));
            }
        }
        std::vector<int> indices(selected_genes.size());
        std::iota(indices.begin(), indices.end(), 0);
        int n = std::accumulate(selected_genes[0].begin(), selected_genes[0].end(), 0, [](int sum, const std::vector<int>& machine) -> int {
            return sum + machine.size();
        });
        int m = selected_genes[0].size();
        int mu = parents.size();
        std::function<double(const std::vector<double>&)> div_value = diversity_vector(n, m, mu);
        std::vector<double> diversity_values;
        diversity_values.reserve(indices.size());
        for (const auto& index : indices) {
            std::vector<double> div_vector;
            div_vector.reserve(indices.size());
            auto isIndexExcluded = [&index](const auto& score) {
                return (std::get<0>(score) != index) && (std::get<1>(score) != index);
            };
            for (const auto& score : diversity_scores) {
                if (isIndexExcluded(score)) {
                    div_vector.push_back(std::get<2>(score));
                }
            }
            diversity_values.emplace_back(div_value(div_vector));
        }
        auto max_it = std::max_element(indices.begin(), indices.end(), [&](int a, int b) {
            return diversity_values[a] < diversity_values[b];
        });
        selected_genes.erase(selected_genes.begin() + *max_it);
        return selected_genes;
    };
};

/*
    qdiv-Selection: Selects the mu (=parent size) individuals with the highest diversity from the combined population of parents and offspring, if quality of offspring is at least OPT * alpha
    Arguments:
        - alpha:                parameter for quality threshold
        - OPT:                  fitness value of optimal solution
        - diversity_measure:    function taking two genes and returning a double representing the diversity
        - evaluate:             function taking a vector of genes and returning a vector of fitnesses
*/

std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_qdiv(double alpha, double OPT, std::function<double(const T&, const T&)> diversity_measure, std::function<std::vector<L>(const std::vector<T>&)> evaluate) {
    return [alpha, OPT, diversity_measure, evaluate](const std::vector<T>& parents, const std::vector<L>& fitnesses_parents, const std::vector<T>& offspring, std::mt19937& generator) -> std::vector<T> {
        assert(offspring.size() == 1);
        if(evaluate(offspring)[0] < OPT * alpha) return parents;
        std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> div = select_div(diversity_measure);
        return div(parents, {}, offspring, generator);
    };
};

/*
    quality-Selection: Selects the mu(=parent_size) individuals with a fitness value of at least quality_bound
    Arguments:
        - quality_bound:    fitness value threshold
        - evaluate:         function taking a vector of genes and returning a vector of fitnesses
*/
std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)> select_quality(double quality_bound, std::function<std::vector<L>(const std::vector<T>&)> evaluate) {
    return [quality_bound, evaluate](const std::vector<T>& parents, const std::vector<L>& fitnesses_parents, const std::vector<T>& offspring, std::mt19937& generator) -> std::vector<T> {
        int mu = parents.size();
        std::vector<T> combined = parents;
        std::vector<L> fitnesses = fitnesses_parents;
        combined.insert(combined.end(), offspring.begin(), offspring.end());
        std::vector<L> fitnesses_offspring = evaluate(offspring);
        fitnesses.insert(fitnesses.end(), fitnesses_offspring.begin(), fitnesses_offspring.end());
        for(int i = 0; i < fitnesses.size(); i++){
            if(fitnesses[i] < quality_bound){
                combined.erase(combined.begin() + i);
                fitnesses.erase(fitnesses.begin() + i);
                i--;
            }
        }
        if(combined.size() <= mu) return combined;
        std::vector<T> selected_genes(mu);
        std::vector<int> indices(mu);
        std::iota(indices.begin(), indices.end(), 0);
        std::partial_sort(indices.begin(), indices.begin() + parents.size(), indices.end(), [&](int a, int b) {
            return fitnesses[a] > fitnesses[b];
        });
        for (int i = 0; i < mu; i++) {
            selected_genes[i] = combined[indices[i]];
        }
        return selected_genes;
    };
};

/*
    pdiv-Selection: Selects the mu (=parent size) individuals with the highest diversity from the combined population of parents and one offspring, preserve diversity scores to improve runtime
    Arguments
        - diversity_measure:    function taking two genes and returning a double representing the diversity
*/

std::function<Diversity_Preserver<T>(const std::vector<T>&, const T&, const Diversity_Preserver<T>&, std::mt19937&)> select_pdiv(std::function<double(const T&, const T&)> diversity_measure) {
    return [diversity_measure](const std::vector<T>& parents, const T& offspring, const Diversity_Preserver<T>& diversity_preserver, std::mt19937& generator) -> Diversity_Preserver<T> {
        std::vector<T> selected_genes = parents;
        selected_genes.emplace(selected_genes.begin() + diversity_preserver.index, offspring);

        std::map<std::tuple<int, int>, double> diversity_scores;
        if(diversity_preserver.first){
            for(int i = 0; i < selected_genes.size(); i++){
                for(int j = i + 1; j < selected_genes.size(); j++){
                    diversity_scores[{i,j}] = diversity_measure(selected_genes[i], selected_genes[j]);
                }
            }
        }else{
            diversity_scores = diversity_preserver.diversity_scores;
            for(int i = 0; i < diversity_preserver.index; i++){
                diversity_scores[{i,diversity_preserver.index}] = diversity_measure(selected_genes[i], selected_genes[diversity_preserver.index]);
            }
            for(int i = diversity_preserver.index + 1; i < selected_genes.size(); i++){
                diversity_scores[{diversity_preserver.index,i}] = diversity_measure(selected_genes[diversity_preserver.index], selected_genes[i]);
            }
        }
        
        std::vector<int> indices(selected_genes.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), generator);

        int n = std::accumulate(selected_genes[0].begin(), selected_genes[0].end(), 0, [](int sum, const std::vector<int>& machine) -> int {
            return sum + machine.size();
        });
        int m = selected_genes[0].size();
        int mu = parents.size();

        std::function<double(const std::vector<double>&)> div_value = diversity_vector(n, m, mu);
        std::vector<double> diversity_values;
        diversity_values.reserve(indices.size());
        for (const auto& index : indices) {
            std::vector<double> div_vector;
            div_vector.reserve(indices.size());
            auto isIndexExcluded = [&index](const auto& entry) {
                const auto& [firstIndex, secondIndex] = entry.first;
                return (firstIndex != index) && (secondIndex != index);
            };
            for (const auto& entry : diversity_scores) {
                if (isIndexExcluded(entry)) {
                    div_vector.push_back(entry.second);
                }
            }
            diversity_values[index] = div_value(div_vector);
        }
        auto max_it = std::max_element(indices.begin(), indices.end(), [&](int a, int b) {
            return diversity_values[a] < diversity_values[b];
        });
        selected_genes.erase(selected_genes.begin() + *max_it);
        return { *max_it, false, diversity_scores, selected_genes };
    };
}

/*
    qpdiv-Selection: Selects the mu (=parent size) individuals with the highest diversity from the combined population of parents and offspring, if quality of offspring is at least OPT * alpha, preserve diversity scores to improve runtime
    Arguments:
        - alpha:                parameter for quality threshold
        - OPT:                  fitness value of optimal solution
        - diversity_measure:    function taking two genes and returning a double representing the diversity
        - evaluate:             function taking a vector of genes and returning a vector of fitnesses
*/

std::function<Diversity_Preserver<T>(const std::vector<T>&, const T&, const Diversity_Preserver<T>&, std::mt19937&)> select_qpdiv(double alpha, double OPT, std::function<double(const T&, const T&)> diversity_measure, std::function<std::vector<L>(const std::vector<T>&)> evaluate) {
    return [alpha, OPT, diversity_measure, evaluate](const std::vector<T>& parents, const T& offspring, const Diversity_Preserver<T>& diversity_preserver, std::mt19937& generator) -> Diversity_Preserver<T> {
        if(evaluate({offspring})[0] < OPT * alpha) return diversity_preserver;
        std::function<Diversity_Preserver<T>(const std::vector<T>&, const T&, const Diversity_Preserver<T>&, std::mt19937&)> div = select_pdiv(diversity_measure);
        return div(parents, offspring, diversity_preserver, generator);
    };
};