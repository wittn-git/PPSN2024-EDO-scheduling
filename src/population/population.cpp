#include "population.hpp"

// Class Implementation ---------------------------------------------------------------------------------------------------------------------

Population::Population(
    int seed,
    const std::function<std::vector<T>(std::mt19937&)>& initialize,
    const std::function<std::vector<L>(const std::vector<T>&)>& evaluate,
    const std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents,
    const std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& mutate,
    const std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& recombine,
    const std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors
) : generator(seed), evaluate(evaluate), selectParents(selectParents), mutate(mutate), recombine(recombine), selectSurvivors(selectSurvivors) {
    assert((evaluate != nullptr && selectParents != nullptr));
    genes = initialize(generator);
}

void Population::execute() {
    std::vector<L> fitnesses = evaluate(genes);
    std::vector<T> parents = selectParents(genes, fitnesses, generator);
    std::vector<T> children = (recombine == nullptr) ? parents : recombine(parents, generator);
    children = (mutate == nullptr) ? children : mutate(children, generator);
    genes = (selectSurvivors == nullptr) ? children : selectSurvivors(genes, fitnesses, children, generator);
}

void Population::execute_multiple(int generations){
    for(int i = 0; i < generations; i++){
        execute();
    }
}

std::vector<T> Population::get_bests(bool keep_duplicats){
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

std::vector<T> Population::get_genes(){
    return genes;
}

std::string Population::to_string(){
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

std::string Population::bests_to_string(bool reciproc){
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

/*
void Population::set_evaluate(const std::function<std::vector<L>(const std::vector<T>&)>& evaluate){ this->evaluate = evaluate;}
void Population::set_selectParents(const std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, std::mt19937&)>& selectParents){ this->selectParents = selectParents;}
void Population::set_mutate(const std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& mutate){ this->mutate = mutate;}
void Population::set_recombine(const std::function<std::vector<T>(const std::vector<T>&, std::mt19937&)>& recombine){ this->recombine = recombine;}
void Population::set_selectSurvivors(const std::function<std::vector<T>(const std::vector<T>&, const std::vector<L>&, const std::vector<T>&, std::mt19937&)>& selectSurvivors){ this->selectSurvivors = selectSurvivors;}
*/