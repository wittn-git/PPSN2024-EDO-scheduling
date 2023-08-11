#include <chrono>

#include "utility/testing.hpp"
#include "utility/solvers.hpp"
#include "utility/parsing.hpp"

using T = std::vector<std::vector<int>>;

/*
    Parameters (in order):
        - Algorithm: {"Mu1-const", "Mu1-unconst", "Simple", "Base" "Survivor-Opt"}
        - Mutation-Operator: {"1RAI", "XRAI", "NSWAP"}
        - Output-File: String
        - runs: Int
        - mus: mu_1,mu_2,...,mu_w
        - ns: n_1,n_2,...,n_x
        - m: m_1,m_2,...m_y
        - alpha: a_1,a_2,...,a_z
*/

int main(int argc, char **argv){
    assert((argc == 9) && "Pass 8 arguments.");

    std::string experiment_type(argv[1]);
    std::string mutation_operator = argv[2];
    std::string output_file = argv[3];
    int runs = (std::string(argv[4]) == "") ? 0 : std::stoi(argv[4]);
    std::vector<int> mus = parse_list<int>(argv[5]);
    std::vector<int> ns = parse_list<int>(argv[6]);
    std::vector<int> ms = parse_list<int>(argv[7]);
    std::vector<double> alphas = (argc == 9) ? parse_list<double>(argv[8]) : std::vector<double>();
    
    if(experiment_type == "Mu1-const"){
        test_algorithm(mus, ns, ms, alphas, runs, output_file, experiment_type, mutation_operator);
    }else if(experiment_type == "Base"){
        test_base(ns, ms, mus, alphas, output_file); //TODO check why this does not print 
    }else if(experiment_type == "Survivor-Opt"){
        test_mu1_optimization(mus, ns, ms, runs, output_file);
    }else{
        throw std::invalid_argument("Invalid experiment type.");
    }
    return 0;
}