#include <pagmo/pagmo.hpp>

class my_mutation : public pagmo::base::mutation_base {
public:
    my_mutation(double mr) : m_mutation_rate(mr) {}

    std::vector<std::vector<double>> mutate(const std::vector<std::vector<double>>& population) const override {
        std::vector<std::vector<double>> mutated_population(population.size());

        for (size_t i = 0; i < population.size(); ++i) {
            mutated_population[i] = mutate_individual(population[i]);
        }

        return mutated_population;
    }

private:
    std::vector<double> mutate_individual(const std::vector<double>& individual) const {
        std::vector<double> mutated_individual = individual;

        for (size_t i = 0; i < individual.size(); ++i) {
            if (pagmo::random::uniform() < m_m
