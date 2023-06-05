#include <functional>
#include <vector>
#include <random>

using T = std::vector<std::vector<int>>;
using L = double;

// Mutation Operators ---------------------------------------------------------------

/*  
    Swap Mutation: Swap two jobs
    Arguments:
        - mutation_rate:        overall probability of a mutation occurring
*/

std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_swap(double mutation_rate) {
    return [mutation_rate](const std::vector<T>& genes, std::mt19937& generator) -> std::vector<T> {
        std::vector<T> mutated_genes(genes.size());
        std::uniform_real_distribution< double > distribute_rate(0, 1);
        std::uniform_int_distribution< int > distribute_machine(0, genes[0].size() - 1 );
        std::transform(genes.begin(), genes.end(), mutated_genes.begin(), [mutation_rate, &generator, distribute_rate, distribute_machine](const T& gene) mutable -> T {
            T mutated_gene(gene);
            if(distribute_rate(generator) < mutation_rate){
                int machine1 = distribute_machine(generator);
                int machine2 = distribute_machine(generator);
                std::uniform_int_distribution< int > distribute_job1(0, gene[machine1].size() - 1 );
                std::uniform_int_distribution< int > distribute_job2(0, gene[machine2].size() - 1 );
                int job1 = distribute_job1(generator);
                int job2 = distribute_job2(generator);
                std::swap(mutated_gene[machine1][job1], mutated_gene[machine2][job2]);
            }
            return mutated_gene;
        });
        return mutated_genes;
    };
}