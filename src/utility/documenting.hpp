#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "../population/population.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

void write_to_file(std::string content, std::string filename, bool append = true) {
    std::ofstream file;
    if (append) {
        file.open(filename, std::ios_base::out | std::ios_base::app);
    } else {
        file.open(filename, std::ios_base::out | std::ios_base::trunc);
    }

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    file << content;
    file.close();
}

std::string get_population_information_string(
    Population<T,L>& population,
    std::function<std::vector<L>(const std::vector<T>&)> evaluate,
    std::function<double(const std::vector<T>&)> diversity_value,
    std::string algorithm_name,
    int mu, int n, int m
){

    std::vector<T> bests = population.get_bests(false, evaluate);
    std::vector<L> fitnesses__bests = evaluate(bests);

    std::string output = algorithm_name + "\n";
    output += "mu: " + std::to_string(mu) + " ";
    output += "n: " + std::to_string(n) + " ";
    output += "m: " + std::to_string(m) + " ";
    output += "Generations: " + std::to_string(population.get_generation()) + "\n";
    output += "Diversity: " + std::to_string(diversity_value(population.get_genes(true))) + "\n";
    output += "Unique individuals: " + std::to_string(population.get_size(false)) + "\n";
    output +=  "Number of unique best individuals: " + std::to_string(bests.size()) + "\n";
    output += "Best fitness: " + std::to_string(fitnesses__bests[0]) + "\n\n";

    return output;
}

std::string get_csv_line(){
    //TODO implement
    return "";
}