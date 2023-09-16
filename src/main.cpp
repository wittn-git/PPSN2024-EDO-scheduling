#include "utility/testing.hpp"
#include "utility/parsing.hpp"

/*
    Parameters (in order):
        - Algorithm: {"Mu1-const", "Mu1-unconst", "NOAH"}
        - Mutation-Operator: {"1RAI", "XRAI", "NSWAP"}
        - Output-File: String
        - runs: Int
        - mus: mu_1,mu_2,...,mu_w
        - ns: n_1,n_2,...,n_x
        - m: m_1,m_2,...m_y
        - alpha: a_1,a_2,...,a_z
        - lambda: Double (only for "XRAI", mean of the poisson distribution)
*/

int main(int argc, char **argv){

    auto [experiment_type, mutation_operator, output_file, mus, ns, ms, alphas, runs, operator_string] = parse_arguments(argc, argv);

    if(experiment_type == "Mu1-const" || experiment_type == "Mu1-unconst"){
        test_algorithm(mus, ns, ms, alphas, runs, output_file, experiment_type, operator_string, mutation_operator);
    }else if(experiment_type == "NOAH"){
        test_noah(mus, ns, ms, runs, 1, 0.5, 0.5, output_file, operator_string, mutation_operator);
    }else{
        throw std::invalid_argument("Invalid experiment type.");
    }

    return 0;
}