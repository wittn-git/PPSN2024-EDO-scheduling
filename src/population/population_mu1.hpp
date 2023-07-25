#pragma once

#include "population.hpp"

template <typename T>
struct Diversity_Preserver {
    int index;
    bool first;
    std::map<std::tuple<int, int>, double> diversity_scores;
    std::vector<T> genes;
};

// Class Outline ----------------------------------------------------------------------------------------------------------------------------

template <typename T, typename L> // T: type of genes, L: type of fitness values
class Population_Mu1 : public Population<T, L>{

private:

    // Function taking a vector of genes of type T, a child T and a diversity preserver and a diversity preserver
    std::function<Diversity_Preserver<T>(const std::vector<T>&, const T&, const Diversity_Preserver<T>&, std::mt19937&)>& selectSurvivors_Div;
    //struct saving the diversity scores of the genes
    Diversity_Preserver<T> div_preserver;

public:

    // Constructor for population of size size will with genes generated by function initialize
    Population_Mu1(
        int seed,
        std::function<std::vector<T>(std::mt19937&)>& initialize,
        std::function<std::vector<L>(const std::vector<T>&)>& evaluate,
        std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents,
        std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& mutate,
        std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& recombine,
        std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors,
        std::function<Diversity_Preserver<T>(const std::vector<T>&, const T&, const Diversity_Preserver<T>&, std::mt19937&)>& selectSurvivors_Div
    );

    //executes one iteration of the evolutionary algorithm
    void execute() override;  
    using Population<T, L>::execute;

    // setters of the operator functions
    void set_selectSurvivors_Div(const std::function<Diversity_Preserver<T>(const std::vector<T>&, const T&, const Diversity_Preserver<T>&, std::mt19937&)>& selectSurvivors_Div);
};

// Class Implementation ---------------------------------------------------------------------------------------------------------------------

template <typename T, typename L>
Population_Mu1<T, L>::Population_Mu1(
    int seed,
    std::function<std::vector<T>(std::mt19937&)>& initialize,
    std::function<std::vector<L>(const std::vector<T>&)>& evaluate,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& mutate,
    std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& recombine,
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors,
    std::function<Diversity_Preserver<T>(const std::vector<T>&, const T&, const Diversity_Preserver<T>&, std::mt19937&)>& selectSurvivors_Div
) : Population<T,L>(seed, initialize, evaluate, selectParents, mutate, recombine, selectSurvivors), selectSurvivors_Div(selectSurvivors_Div) {
    assert(selectSurvivors_Div == nullptr || selectSurvivors == nullptr && "selectSurvivors and selectSurvivors_Div cannot be set at the same time");
    div_preserver = Diversity_Preserver<T>{0, true, std::map<std::tuple<int, int>, double>(), this->genes};
}

template <typename T, typename L>
void Population_Mu1<T, L>::execute() {
    this->generation++;
    std::vector<L> fitnesses = (this->evaluate == nullptr) ? std::vector<L>(0) : this->evaluate(this->genes);
    assert(this->evaluate == nullptr || fitnesses.size() == this->genes.size());
    std::vector<T> parents = (this->selectParents == nullptr) ? this->genes : this->selectParents(this->genes, fitnesses, this->generator);
    std::vector<T> children = (this->recombine == nullptr) ? parents : this->recombine(parents, this->generator);
    children = (this->mutate == nullptr) ? children : this->mutate(children, this->generator);
    this->genes = (this->selectSurvivors == nullptr) ? this->genes : this->selectSurvivors(this->genes, fitnesses, children, this->generator);
    if(selectSurvivors_Div != nullptr){
        div_preserver = selectSurvivors_Div(this->genes, children[0], div_preserver, this->generator);
        this->genes = div_preserver.genes;
    }
}

template <typename T, typename L>
void Population_Mu1<T, L>::set_selectSurvivors_Div(const std::function<Diversity_Preserver<T>(const std::vector<T>&, const T&, const Diversity_Preserver<T>&, std::mt19937&)>& selectSurvivors_Div){ this->selectSurvivors_Div = selectSurvivors_Div;}