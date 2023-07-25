#pragma once

#include <functional>
#include <vector>
#include <random>
#include <iostream>

using T = std::vector<std::vector<int>>;
using L = double;

// Mutation Operators ---------------------------------------------------------------

/*
    Remove Insert Mutation: Remove a job from a schedule and insert it again
    Arguments:
        - mutation_rate:        probability of a mutation occurring for each gene
*/

std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_removeinsert(double mutation_rate) {
    return [mutation_rate](const std::vector<T>& genes, std::mt19937& generator) -> std::vector<T> {
        std::vector<T> mutated_genes(genes.size());
        std::uniform_real_distribution< double > distribute_rate(0, 1);
        std::uniform_int_distribution< int > distribute_machine(0, genes[0].size() - 1 );
        std::transform(genes.begin(), genes.end(), mutated_genes.begin(), [mutation_rate, &generator, distribute_rate, distribute_machine](const T& gene) mutable -> T {
            T mutated_gene(gene);
            if(distribute_rate(generator) < mutation_rate){
                int machine_remove;
                do{
                    machine_remove = distribute_machine(generator);
                }while(mutated_gene[machine_remove].size() < 1);
                std::uniform_int_distribution< int > distribute_job_remove(0, mutated_gene[machine_remove].size() - 1);
                int job_remove = distribute_job_remove(generator);
                int job = mutated_gene[machine_remove][job_remove];
                mutated_gene[machine_remove].erase(mutated_gene[machine_remove].begin() + job_remove);
                int machine_insert = distribute_machine(generator);
                std::uniform_int_distribution< int > distribute_job_insert(0, mutated_gene[machine_insert].size());
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

std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_neighborswap(double mutation_rate) {
    return [mutation_rate](const std::vector<T>& genes, std::mt19937& generator) -> std::vector<T> {
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