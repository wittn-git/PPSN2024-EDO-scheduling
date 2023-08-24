#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "../population/population.hpp"

using L = double;

void write_to_file(std::string content, std::string filename, bool append = true) {
    #pragma omp critical
    {
        std::ofstream file;
        if (append) {
            file.open(filename, std::ios_base::out | std::ios_base::app);
        } else {
            file.open(filename, std::ios_base::out | std::ios_base::trunc);
        }

        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
        }else{
            file << content;
            file.close();
        }        
    }
}

std::string createPopulationReport(
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
    output += "m: " + std::to_string(m) + "\n";
    output += "Generations: " + std::to_string(population.get_generation()) + "\n";
    output += "Diversity: " + std::to_string(diversity_value(population.get_genes(true))) + "\n";
    output += "Unique individuals: " + std::to_string(population.get_size(false)) + "\n";
    output +=  "Number of unique best individuals: " + std::to_string(bests.size()) + "\n";
    output += "Best fitness: " + std::to_string(fitnesses__bests[0]) + "\n";

    return output;
}

template<typename... Args>
std::string get_csv_line(const Args&... args) {
    std::ostringstream oss;
    bool isFirst = true;
    ((oss << (isFirst ? "" : ",") << args, isFirst = false), ...);
    oss << "\n";
    return oss.str();
}