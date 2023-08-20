#pragma once

#include <functional>
#include <vector>
#include <random>
#include <iostream>

using T = std::vector<std::vector<int>>;
using L = double;

// Utility Functions ----------------------------------------------------------------

T remove_and_insert(const T& gene, std::mt19937& generator){
    T mutated_gene(gene);
    std::uniform_int_distribution< int > distribute_machine(0, gene.size() - 1 );
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
    return mutated_gene;
}

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
        std::transform(genes.begin(), genes.end(), mutated_genes.begin(), [mutation_rate, &generator, distribute_rate](const T& gene) mutable -> T {
            T mutated_gene;
            if(distribute_rate(generator) < mutation_rate){
                mutated_gene = remove_and_insert(gene, generator);
            }
            return mutated_gene;
        });
        return mutated_genes;
    };
}

/*
    X Remove Insert Mutation: Remove a job from a schedule and insert it again, number of jobs sampled from a poisson distribution
    Arguments:
        - mutation_rate:        probability of a mutation occurring for each gene
        - mean:                 mean of the poisson distribution used to sample the number of jobs to be removed and inserted
*/

std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_xremoveinsert(double mutation_rate, double mean) {
    return [mutation_rate, mean](const std::vector<T>& genes, std::mt19937& generator) -> std::vector<T> {
        std::vector<T> mutated_genes(genes.size());
        std::uniform_real_distribution< double > distribute_rate(0, 1);
        std::poisson_distribution< int > distribute_actions(mean);
        std::transform(genes.begin(), genes.end(), mutated_genes.begin(), [mutation_rate, &generator, distribute_rate, distribute_actions](const T& gene) mutable -> T {
            T mutated_gene(gene);
            if(distribute_rate(generator) < mutation_rate){
                int actions = 1 + distribute_actions(generator);
                for(int i = 0; i < actions; i++){
                    mutated_gene = remove_and_insert(mutated_gene, generator);
                }
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

/*
    Bounded-Mutation: Mutate genes with given operator such that the resulting genes are bounded by a given value
    Arguments:
        - mutation_operator:    mutation operator to be used
        - evaluate:             fitness function
        - mu:                   number of solutions to be generated
*/

std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutate_bound(std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)> mutation_operator, std::function<std::vector<L>(const std::vector<T>&)> evaluate, double bound, int mu){
    return [mutation_operator, evaluate, bound, mu](const std::vector<T>& genes, std::mt19937& generator) -> std::vector<T> {
        std::vector<T> offspring;
        std::uniform_int_distribution< int > distribute_parent(0, genes.size() - 1);
        while(offspring.size() < mu){
            std::vector<T> parents;
            for(int i = 0; i < mu - offspring.size(); i++){
                parents.emplace_back(distribute_parent(generator));
            }
            std::vector<T> mutated_genes = mutation_operator(parents, generator);
            std::vector<L> fitnesses = evaluate(mutated_genes);
            for(int i = 0; i < mutated_genes.size(); i++){
                if(bound <= fitnesses[i]){
                    offspring.emplace_back(mutated_genes[i]);
                }
            }
        }
        return offspring;
    };
}