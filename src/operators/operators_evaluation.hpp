#pragma once

#include <functional>
#include <vector>

#include "../utility/generating.hpp"

using T = std::vector<std::vector<int>>;
using L = double;

// Evaluation Operators -------------------------------------------------------------

/*
    Makespan Evaluation: Evaluates machine schedules based on the makespan
    Arguments:
        - problem:          MachineSchedulingProblem struct containing the problem data
*/

std::function<std::vector<L>(const std::vector<T>&)> evaluate_makespan(MachineSchedulingProblem problem) { 
    return [problem](const std::vector<T>& genes) -> std::vector<L> {
        std::vector<L> fitnesses(genes.size());
        std::transform(genes.begin(), genes.end(), fitnesses.begin(), [&](const T& gene) -> double {
            std::vector<int> end_points;
            for(auto schedule : gene){
                int current_time = 0;
                for(auto it = schedule.begin(); it != schedule.end(); it++){
                    current_time += problem.processing_times[*it];
                }
                end_points.emplace_back(current_time);
            }
            return (double) (-(1) * *std::max_element(end_points.begin(), end_points.end()));
        });
        return fitnesses;
    };
}

/*
    Tardyjobs Evaluation: Evaluates machine schedules based on the number of tardy jobs
    Arguments:
        - processing_times: Times it takes to complete each job
        - due_dates:        Points in time where every job is due
*/

std::function<std::vector<L>(const std::vector<T>&)> evaluate_tardyjobs(MachineSchedulingProblem problem) { 
    int n = problem.processing_times.size();
    return [problem, n](const std::vector<T>& genes) -> std::vector<L> {
        std::vector<L> fitnesses(genes.size());
        std::transform(genes.begin(), genes.end(), fitnesses.begin(), [&](const T& gene) -> double {
            int tardy_jobs_n = 0;
            for(auto schedule : gene){
                int current_time = 0;
                for(auto it = schedule.begin(); it != schedule.end(); it++){
                    current_time += problem.processing_times[*it];
                    if(current_time > problem.due_dates[*it]){
                        tardy_jobs_n++;
                    }
                }
            }
            return (double) ( n - tardy_jobs_n);
        });
        return fitnesses;
    };
}