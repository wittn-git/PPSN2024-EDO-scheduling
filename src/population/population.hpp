#pragma once

#include <vector>
#include <functional>
#include <random>
#include <assert.h>

// Class Outline ----------------------------------------------------------------------------------------------------------------------------

template <typename T, typename L> // T: type of genes, L: type of fitness values
class Population{

protected:

    std::vector<T> genes;
    std::mt19937 generator;
    int generation;

    // Function taking a vector of genes of type T and returning its fitness value vector of type L
    std::function<std::vector<L>(const std::vector<T>&)>& evaluate;
    // Function taking a vector of genes of type T and returning a vector of parents of type T
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents;
    // Function taking a vector of genes of type T and returning a vector of mutated genes of type T
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& mutate;
    // Function taking a vector of genes of type T and returning a vector of recombined genes of type T
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& recombine;
    // Function taking two vectors of genes of type T (parents and children) and returning a selected vector of genes of type T
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors;

    std::string gene_to_string(T gene);

public:

    // Constructor for population of size size will with genes generated by function initialize
    Population(
        int seed,
        std::function<std::vector<T>(std::mt19937&)>& initialize,
        std::function<std::vector<L>(const std::vector<T>&)>& evaluate,
        std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents,
        std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& mutate,
        std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& recombine,
        std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors
    );

    // Default constructor for initialization purposes
    Population();

    //executes one iteration of the evolutionary algorithm
    virtual void execute();   
    //executes iterations of the evolutionary algorithm until the termination criterion is met                                              
    virtual void execute(std::function<bool(Population<T,L>&)> termination_criterion);                         
    //returns the best genes in the population, using the given evaluate function
    std::vector<T> get_bests(bool keep_duplicats, std::function<std::vector<L>(const std::vector<T>&)>& evaluate);       
    //returns the current genes in the population           
    std::vector<T> get_genes(bool keep_duplicats);   
    //returns the number of generation that have been executed               
    int get_generation();
    //returns the size of the population
    int get_size(bool keep_duplicates);        
    //sets the genes in the population to new_genes                                         
    void set_genes(std::vector<T> new_genes);      
    //returns a string representation of the population                 
    std::string to_string(bool keep_duplicates);                         
    //returns a string representation of the best genes in the population, using the given evaluate function
    std::string bests_to_string(std::function<std::vector<L>(const std::vector<T>&)>& evaluate);                     
    
    // setters of the operator functions
    void set_evaluate(const std::function<std::vector<L>(const std::vector<T>&)>& evaluate);
    void set_selectParents(const std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents);
    void set_mutate(const std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& mutate);
    void set_recombine(const std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& recombine);
    void set_selectSurvivors(const std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors);
};

// Class Implementation ---------------------------------------------------------------------------------------------------------------------

template <typename T, typename L>
Population<T, L>::Population(
    int seed,
    std::function<std::vector<T>(std::mt19937&)>& initialize,
    std::function<std::vector<L>(const std::vector<T>&)>& evaluate,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& mutate,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& recombine,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors
) : generation(0), generator(seed), evaluate(evaluate), selectParents(selectParents), mutate(mutate), recombine(recombine), selectSurvivors(selectSurvivors) {
    assert(initialize != nullptr && "initialize function must be set");
    genes = initialize(generator);
    assert(genes.size() > 0 && "initialize function must return a non-empty vector");
}

template <typename T, typename L>
Population<T, L>::Population(){};

template <typename T, typename L>
void Population<T, L>::execute() {
    generation++;
    std::vector<L> fitnesses = (evaluate == nullptr) ? std::vector<L>(0) : evaluate(genes);
    assert(evaluate == nullptr || fitnesses.size() == genes.size());
    std::vector<T> parents = (selectParents == nullptr) ? genes : selectParents(genes, fitnesses, generator);
    std::vector<T> children = (recombine == nullptr) ? parents : recombine(parents, generator);
    children = (mutate == nullptr) ? children : mutate(children, generator);
    genes = (selectSurvivors == nullptr) ? children : selectSurvivors(genes, fitnesses, children, generator);
}

template <typename T, typename L>
void Population<T, L>::execute(std::function<bool(Population<T,L>&)> termination_criterion){
    while(!termination_criterion(*this)){
        execute();
    }
}

template <typename T, typename L>
std::vector<T> Population<T, L>::get_bests(bool keep_duplicats, std::function<std::vector<L>(const std::vector<T>&)>& evaluate){
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
std::vector<T> Population<T, L>::get_genes(bool keep_duplicats){
    if(keep_duplicats) return genes;
    std::sort(genes.begin(), genes.end());
    genes.erase(std::unique(genes.begin(), genes.end()), genes.end());
    return genes;
}

template <typename T, typename L>
int Population<T, L>::get_generation(){
    return generation;
}

template <typename T, typename L>
int Population<T, L>::get_size(bool keep_duplicates){
    return get_genes(keep_duplicates).size();
}

template <typename T, typename L>
void Population<T, L>::set_genes(std::vector<T> new_genes){
    genes = new_genes;
}

template <typename T, typename L>
std::string Population<T, L>::gene_to_string(T gene){
    std::string s;
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
    return s;
}

template <typename T, typename L>
std::string Population<T, L>::to_string(bool keep_duplicates){
    std::string s;
    for (auto gene : get_genes(keep_duplicates)) {
        s += gene_to_string(gene);
    }
    return s;
}

template <typename T, typename L>
std::string Population<T, L>::bests_to_string(std::function<std::vector<L>(const std::vector<T>&)>& evaluate){
    std::string s;
    std::vector<T> bests = get_bests(false, evaluate);
    std::vector<L> fitnesses = evaluate(bests);
    for (int i = 0; i < bests.size(); i++) {
        s += "Fitness: " + std::to_string(fitnesses[i]) + "\n";
        s += gene_to_string(bests[i]);
    }
    return s;
}

template <typename T, typename L>
void Population<T, L>::set_evaluate(const std::function<std::vector<L>(const std::vector<T>&)>& evaluate){ this->evaluate = evaluate;}
template <typename T, typename L>
void Population<T, L>::set_selectParents(const std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents){ this->selectParents = selectParents;}
template <typename T, typename L>
void Population<T, L>::set_mutate(const std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& mutate){ this->mutate = mutate;}
template <typename T, typename L>
void Population<T, L>::set_recombine(const std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& recombine){ this->recombine = recombine;}
template <typename T, typename L>
void Population<T, L>::set_selectSurvivors(const std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors){ this->selectSurvivors = selectSurvivors;}