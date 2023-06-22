#pragma once

#include <functional>
#include <vector>

using T = std::vector<std::vector<int>>;
using L = double;

// Evaluation Operators -------------------------------------------------------------

/*
    Makespan Evaluation: Evaluates machine schedules based on the makespan
    Arguments:
        - processing_times: Times it takes to complete each job
        - release_dates:    Points in time where every job is released
        - due_dates:        Points in time where every job is due
*/

std::function<std::vector<L>(const std::vector<T>&)> evaluate_makespan(std::vector<int> processing_times, std::vector<int> release_dates) { 
    return [processing_times, release_dates](const std::vector<T>& genes) -> std::vector<L> {
        std::vector<L> fitnesses(genes.size());
        std::transform(genes.begin(), genes.end(), fitnesses.begin(), [&](const T& gene) -> double {
            std::vector<int> end_points;
            for(auto schedule : gene){
                int current_time = 0;
                for(auto it = schedule.begin(); it != schedule.end(); it++){
                    if(release_dates[*it] > current_time){
                        current_time = release_dates[*it] + processing_times[*it];
                    }else{
                        current_time += processing_times[*it];
                    }
                }
                end_points.emplace_back(current_time);
            }
            return (double) - *std::max_element(end_points.begin(), end_points.end());
        });
        return fitnesses;
    };
}

/*
    Tardyjobs Evaluation: Evaluates machine schedules based on the number of tardy jobs
    Arguments:
        - processing_times: Times it takes to complete each job
        - release_dates:    Points in time where every job is released
        - due_dates:        Points in time where every job is due
*/

std::function<std::vector<L>(const std::vector<T>&)> evaluate_tardyjobs(std::vector<int> processing_times, std::vector<int> release_dates, std::vector<int> due_dates) { 
    return [processing_times, release_dates, due_dates](const std::vector<T>& genes) -> std::vector<L> {
        std::vector<L> fitnesses(genes.size());
        std::transform(genes.begin(), genes.end(), fitnesses.begin(), [&](const T& gene) -> double {
            int tardy_jobs_n = 0;
            for(auto schedule : gene){
                int current_time = 0;
                for(auto it = schedule.begin(); it != schedule.end(); it++){
                    if(release_dates[*it] > current_time){
                        current_time = release_dates[*it] + processing_times[*it];
                    }else{
                        current_time += processing_times[*it];
                    }
                    if(current_time > due_dates[*it]){
                        tardy_jobs_n++;
                    }
                }
            }
            return (double) - tardy_jobs_n;
        });
        return fitnesses;
    };
}

/*
    Lateness Evaluation: Evaluates machine schedules based on the maximum lateness
    Arguments:
        - processing_times: Times it takes to complete each job
        - release_dates:    Points in time where every job is released
        - due_dates:        Points in time where every job is due
*/