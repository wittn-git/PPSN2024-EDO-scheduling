#include "utility/testing.hpp"
#include "utility/parsing.hpp"

/*
    Parameters (in order):
        - Algorithm: {"Mu1-const", "Mu1-unconst", "Simple", "Base", "Survivor-Opt", "NOAH"}
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

    if(argc != 4){
        std::cout << "Usage: " << argv[0] << " <mu> <n> <m>" << std::endl;
        return 1;
    }

    int mu = std::stoi(argv[1]);
    int n = std::stoi(argv[2]);
    int m = std::stoi(argv[3]);
    int seed = n * m * mu;

    MachineSchedulingProblem problem = get_problem(seed, n, 50);
    auto evaluate = evaluate_tardyjobs(problem);
    auto mutation_operator = mutate_removeinsert(1);
    auto diversity_measure = diversity_DFM();

    std::ofstream file;
    file.open("output_componenttimes.txt", std::ios_base::out | std::ios_base::trunc);
    file << "mu=" << mu << " n=" << n << " m=" << m << std::endl;
    file.close();

    Population<T,L>  unopt_pop = mu1_unconstrained_unoptimized(
        n*mu*m, 1, n, mu,
        terminate_generations(100), evaluate, mutation_operator, diversity_measure
    );

    return 0;
}