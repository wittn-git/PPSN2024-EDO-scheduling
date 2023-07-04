#pragma once

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

std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> mutate_swap(double mutation_rate) {
    return [mutation_rate](const std::vector<T>& genes, double quality_bound, std::mt19937& generator) -> std::vector<T> {
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

/*
    Remove Insert Mutation: Remove a job from a schedule and insert it again
    Arguments:
        - mutation_rate:        probability of a mutation occurring for each gene
*/

std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> mutate_removeinsert(double mutation_rate) {
    return [mutation_rate](const std::vector<T>& genes, double quality_bound, std::mt19937& generator) -> std::vector<T> {
        std::vector<T> mutated_genes(genes.size());
        std::uniform_real_distribution< double > distribute_rate(0, 1);
        std::uniform_int_distribution< int > distribute_machine(0, genes[0].size() - 1 );
        std::transform(genes.begin(), genes.end(), mutated_genes.begin(), [mutation_rate, &generator, distribute_rate, distribute_machine](const T& gene) mutable -> T {
            T mutated_gene(gene);
            if(distribute_rate(generator) < mutation_rate){
                int machine_remove = distribute_machine(generator);
                std::uniform_int_distribution< int > distribute_job_remove(0, gene[machine_remove].size() - 1 );
                int job_remove = distribute_job_remove(generator);
                int job = mutated_gene[machine_remove][job_remove];
                mutated_gene[machine_remove].erase(mutated_gene[machine_remove].begin() + job_remove);
                int machine_insert = distribute_machine(generator);
                std::uniform_int_distribution< int > distribute_job_insert(0, gene[machine_insert].size() - 1 );
                int job_insert = distribute_job_insert(generator);
                mutated_gene[machine_insert].insert(mutated_gene[machine_insert].begin() + job_insert, job);
            }
            return mutated_gene;
        });
        return mutated_genes;
    };
}

/*
    Neighbor-Swap Mutation: Swap two adjacent jobs from the same machine
    Arguments:
        - mutation_rate:        probability of a mutation occurring for each gene
*/

std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)> mutate_neighborswap(double mutation_rate) {
    return [mutation_rate](const std::vector<T>& genes, double quality_bound, std::mt19937& generator) -> std::vector<T> {
        std::vector<T> mutated_genes(genes.size());
        std::uniform_real_distribution< double > distribute_rate(0, 1);
        std::uniform_int_distribution< int > distribute_machine(0, genes[0].size() - 1 );
        std::transform(genes.begin(), genes.end(), mutated_genes.begin(), [mutation_rate, &generator, distribute_rate, distribute_machine](const T& gene) mutable -> T {
            T mutated_gene(gene);
            if(distribute_rate(generator) < mutation_rate){
                int machine;
                do{
                    machine = distribute_machine(generator);
                }while(gene[machine].size() < 2);
                std::uniform_int_distribution< int > distribute_job(0, gene[machine].size() - 2 );
                int job = distribute_job(generator);
                std::swap(mutated_gene[machine][job], mutated_gene[machine][job + 1]);
            }
            return mutated_gene;
        });
        return mutated_genes;
    };
}