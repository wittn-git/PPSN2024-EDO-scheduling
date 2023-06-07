#pragma once

#include <vector>
#include <functional>
#include <random>
#include <assert.h>


// Class Outline ----------------------------------------------------------------------------------------------------------------------------

template <typename T, typename L> // T: type of genes, L: type of fitness values
class Population{

private:

    std::vector<T> genes;
    std::mt19937 generator;
    int generation;

    // Function taking a vector of genes of type T and returning its fitness value vector of type L
    std::function<std::vector<L>(const std::vector<T>&)>& evaluate;
    // Function taking a vector of genes of type T and returning a vector of parents of type T
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents;
    // Function taking a vector of genes of type T and returning a vector of mutated genes of type T
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)>& mutate;
    // Function taking a vector of genes of type T and returning a vector of recombined genes of type T
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)>& recombine;
    // Function taking two vectors of genes of type T (parents and children) and returning a selected vector of genes of type T
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors;

public:

    // Constructor for population of size size will with genes generated by function initialize
    Population(
        int seed,
        std::function<std::vector<T>(std::mt19937&)>& initialize,
        std::function<std::vector<L>(const std::vector<T>&)>& evaluate,
        std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents,
        std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)>& mutate,
        std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)>& recombine,
        std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors
    );

    void execute(double quality_bound);                             //executes one iteration of the evolutionary algorithm
    void execute_multiple(int generations, double quality_bound);   //executes 'generations' iterations of the evolutionary algorithm
    std::vector<T> get_bests(bool keep_duplicats);                  //returns the best genes in the population
    std::vector<T> get_genes();                                     //returns the current genes in the population
    int get_generation();                                          //returns the number of generation that have been executed
    void set_genes(std::vector<T> new_genes);                       //sets the genes in the population to new_genes
    std::string to_string();                                        //returns a string representation of the population
    std::string bests_to_string(bool reciproc);                     //returns a string representation of the best genes in the population, reciproc = true will return the reciproc of the fitness values
    
    // setters of the operator functions
    void set_evaluate(const std::function<std::vector<L>(const std::vector<T>&)>& evaluate); //sets the evaluate function
    void set_selectParents(const std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents); //sets the selectParents function
    void set_mutate(const std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)>& mutate); //sets the mutate function
    void set_recombine(const std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)>& recombine); //sets the recombine function
    void set_selectSurvivors(const std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors); //sets the selectSurvivors function
    
};

// Class Implementation ---------------------------------------------------------------------------------------------------------------------

template <typename T, typename L>
Population<T, L>::Population(
    int seed,
    std::function<std::vector<T>(std::mt19937&)>& initialize,
    std::function<std::vector<L>(const std::vector<T>&)>& evaluate,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents,
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)>& mutate,
    std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)>& recombine,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors
) : generation(0), generator(seed), evaluate(evaluate), selectParents(selectParents), mutate(mutate), recombine(recombine), selectSurvivors(selectSurvivors) {
    assert(initialize != nullptr && "initialize function must be set");
    assert((evaluate != nullptr || (selectParents == nullptr && selectSurvivors == nullptr)) && "evaluate function must be set if selectParents or selectSurvivors are set");
    genes = initialize(generator);
    assert(genes.size() > 0 && "initialize function must return a non-empty vector");
}

template <typename T, typename L>
void Population<T, L>::execute(double quality_bound) {
    generation++;
    std::vector<L> fitnesses = (evaluate == nullptr) ? std::vector<L>(0) : evaluate(genes);
    assert(evaluate == nullptr || fitnesses.size() == genes.size());
    std::vector<T> parents = (selectParents == nullptr) ? genes : selectParents(genes, fitnesses, generator);
    std::vector<T> children = (recombine == nullptr) ? parents : recombine(parents, quality_bound, generator);
    children = (mutate == nullptr) ? children : mutate(children, quality_bound, generator);
    genes = (selectSurvivors == nullptr) ? children : selectSurvivors(genes, fitnesses, children, generator);
}

template <typename T, typename L>
void Population<T, L>::execute_multiple(int generations, double quality_bound){
    for(int i = 0; i < generations; i++){
        execute(quality_bound);
    }
}

template <typename T, typename L>
std::vector<T> Population<T, L>::get_bests(bool keep_duplicats){
    std::vector<T> bests;
    std::vector<L> fitnesses = evaluate(genes);
    auto max_it = std::max_element(fitnesses.begin(), fitnesses.end());
    for(int i = 0; i < genes.size(); i++){
        if(fitnesses[i] == *max_it){
            bests.emplace_back(genes[i]);
        }
    }
    if(keep_duplicats){
        return bests;
    }
    std::sort(bests.begin(), bests.end());
    bests.erase(std::unique(bests.begin(), bests.end()), bests.end());
    return bests;
}

template <typename T, typename L>
std::vector<T> Population<T, L>::get_genes(){
    return genes;
}

template <typename T, typename L>
int Population<T, L>::get_generation(){
    return generation;
}

template <typename T, typename L>
void Population<T, L>::set_genes(std::vector<T> new_genes){
    genes = new_genes;
}

template <typename T, typename L>
std::string Population<T, L>::to_string(){
    std::string s;
    for (auto gene : genes) {
        int machine = 1;
        for (auto chromosome : gene) {
            s += std::to_string(machine) + ": ";
            for(auto job : chromosome){
                s += std::to_string(job) + " ";
            }
            s += "\n";
            machine++;
        }
        s += "\n";
    }
    return s;
}

template <typename T, typename L>
std::string Population<T, L>::bests_to_string(bool reciproc){
    std::string s;
    std::vector<T> bests = get_bests(false);
    std::vector<L> fitnesses = evaluate(bests);
    for (int i = 0; i < bests.size(); i++) {
        s += "Fitness: ";
        s += reciproc ? std::to_string(1/fitnesses[i]) : std::to_string(fitnesses[i]);
        s += "\n";
        int machine = 1;
        for (auto chromosome : bests[i]) {
            s += std::to_string(machine) + ": ";
            for(auto job : chromosome){
                s += std::to_string(job) + " ";
            }
            s += "\n";
            machine++;
        }
        s += "\n";
    }
    return s;
}

template <typename T, typename L>
void Population<T, L>::set_evaluate(const std::function<std::vector<L>(const std::vector<T>&)>& evaluate){ 
    assert((evaluate != nullptr || (selectParents == nullptr && selectSurvivors == nullptr)) && "evaluate function must be set if selectParents or selectSurvivors are set");
    this->evaluate = evaluate;
}
template <typename T, typename L>
void Population<T, L>::set_selectParents(const std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents){ this->selectParents = selectParents;}
template <typename T, typename L>
void Population<T, L>::set_mutate(const std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)>& mutate){ this->mutate = mutate;}
template <typename T, typename L>
void Population<T, L>::set_recombine(const std::function<std::vector<T>(const std::vector<T>&, double, std::mt19937&)>& recombine){ this->recombine = recombine;}
template <typename T, typename L>
void Population<T, L>::set_selectSurvivors(const std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors){ this->selectSurvivors = selectSurvivors;}