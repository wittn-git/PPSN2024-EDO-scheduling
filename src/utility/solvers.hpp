#pragma once

#include <vector>
#include <numeric>
#include <iostream>
#include <assert.h>

#include "generating.hpp"

using T = std::vector<std::vector<int>>;

std::vector<int> moores_algorithm(MachineSchedulingProblem problem){
    assert(problem.processing_times.size() == problem.due_dates.size());

    auto first_late = [&](std::vector<int> schedule) {
        int current_time = 0;
        for(int i = 0; i < schedule.size(); i++){
            if(problem.processing_times[schedule[i]] + current_time > problem.due_dates[schedule[i]]) return i;
            current_time += problem.processing_times[schedule[i]];
        }
        return -1;
    };

    int n = problem.processing_times.size();
    std::vector<int> schedule(n);
    std::iota(schedule.begin(), schedule.end(), 0);   
    std::vector<int> rejected_jobs;

    std::sort(schedule.begin(), schedule.end(), [&](int a, int b) {
        return problem.processing_times[a] < problem.processing_times[b];
    });

    while(true){
        int first_late_index = first_late(schedule);
        if(first_late_index == -1) break;
        int first_late_job = schedule[first_late_index];
        std::vector<int> early_jobs(schedule.begin(), schedule.begin() + first_late_index + 1);
        std::vector<int> late_jobs(schedule.begin() + first_late_index + 1, schedule.end());
        std::sort(early_jobs.begin(), early_jobs.end(), [&](int a, int b) {
            return problem.due_dates[a] < problem.due_dates[b];
        });
        if(first_late(early_jobs) != -1){
            int rejected_index = std::find(early_jobs.begin(), early_jobs.end(), first_late_job) - early_jobs.begin();
            rejected_jobs.emplace_back(early_jobs[rejected_index]);
            early_jobs.erase(early_jobs.begin() + rejected_index);   
        }
        schedule = early_jobs;
        schedule.insert(schedule.end(), late_jobs.begin(), late_jobs.end());
    }

    schedule.insert(schedule.end(), rejected_jobs.begin(), rejected_jobs.end());
    return schedule;
}

T approximation_algorithm(MachineSchedulingProblem problem, int m){

    assert(problem.processing_times.size() == problem.due_dates.size());

    auto first_late = [&](std::vector<int> schedule) {
        int current_time = 0;
        for(int i = 0; i < schedule.size(); i++){
            if(problem.processing_times[schedule[i]] + current_time > problem.due_dates[schedule[i]]) return i;
            current_time += problem.processing_times[schedule[i]];
        }
        return -1;
    };

    auto early_jobs_n = [&](std::vector<int> schedule) {
        int first_late_index = first_late(schedule);
        if(first_late_index == -1) return (int) schedule.size();
        return first_late_index;
    };

    auto start_time = [&](std::vector<int> schedule, int job) {
        int current_time = 0;
        for(int i = 0; i < schedule.size(); i++){
            if(schedule[i] == job) return current_time;
            current_time += problem.processing_times[schedule[i]];
        }
        return -1;
    };

    auto slacks = [&](std::vector<int> schedule) {
        std::vector<int> slacks(schedule.size());
        int current_time = 0;
        for(int i = 0; i < schedule.size(); i++){
            current_time += problem.processing_times[schedule[i]];
            slacks[i] = problem.due_dates[schedule[i]] - current_time;
        }
        return slacks;
    };

    auto minimum_slack = [&](std::vector<int> schedule, int start_index) {
        int min_slack = std::numeric_limits<int>::max();
        for (int i = start_index + 1; i < early_jobs_n(schedule); ++i) {
            min_slack = std::min(min_slack, slacks(schedule)[i]);
        }
        return min_slack;
    };
    
    std::vector<std::vector<int>> schedule(m);
    std::vector<int> rejected_jobs;
    std::vector<int> unscheduled_jobs;

    int n = problem.processing_times.size();
    int k = 0;

    for(int i = 0; i < n; i++){
        if(problem.processing_times[i] > problem.due_dates[i]){
            rejected_jobs.emplace_back(i);
        } else {
            schedule[k].emplace_back(i);
        }
    }
    
    std::sort(schedule[k].begin(), schedule[k].end(), [&](int a, int b) {
        return problem.due_dates[a] < problem.due_dates[b];
    });

    while(true){
        while(true){
            int first_late_index = first_late(schedule[k]);
            if(first_late_index == -1) break;
            std::vector<int> early_jobs(schedule[k].begin(), schedule[k].begin() + first_late_index + 1);
            int longest_job = *(std::max_element(early_jobs.begin(), early_jobs.end(), [&](int a, int b) {
                return problem.processing_times[a] < problem.processing_times[b];
            }));
            unscheduled_jobs.emplace_back(longest_job);
            schedule[k].erase(std::find(schedule[k].begin(), schedule[k].end(), longest_job));
        }
        if(k >= m - 1 || unscheduled_jobs.size() == 0){
            break;
        }
        for(int j = 0; j < early_jobs_n(schedule[k]); j++){
            for(int i = 0; i < early_jobs_n(unscheduled_jobs); i++){
                int unscheduled_job = unscheduled_jobs[i];
                int scheduled_job = schedule[k][j];
                std::vector<int> schedule_(schedule[k]);
                std::vector<int> unscheduled_jobs_(unscheduled_jobs);
                schedule_[j] = unscheduled_job;
                unscheduled_jobs_[i] = scheduled_job;
                if(
                    problem.due_dates[unscheduled_job] >= start_time(schedule[k], scheduled_job) + problem.processing_times[unscheduled_job] &&
                    minimum_slack(schedule[k], j) >= problem.processing_times[unscheduled_job] - problem.processing_times[scheduled_job] &&
                    (
                        minimum_slack(schedule_, j) > problem.due_dates[unscheduled_job] - start_time(schedule[k], scheduled_job) - problem.processing_times[unscheduled_job] ||
                        (
                            problem.due_dates[unscheduled_job] > problem.due_dates[scheduled_job] &&
                            problem.processing_times[unscheduled_job] > problem.processing_times[scheduled_job] &&
                            (problem.processing_times[unscheduled_job] - problem.processing_times[scheduled_job]) * (early_jobs_n(schedule_) - j + 2) > (problem.due_dates[unscheduled_job] - problem.due_dates[scheduled_job])
                        )
                    )
                ){
                    schedule[k] = schedule_;
                    unscheduled_jobs = unscheduled_jobs_;
                }
            }
        }
        k += 1;
        schedule[k] = unscheduled_jobs;
        unscheduled_jobs.clear();
    }

    int t = 0;
    rejected_jobs.insert(rejected_jobs.end(), unscheduled_jobs.begin(), unscheduled_jobs.end());
    for(auto job : rejected_jobs){
        schedule[t].emplace_back(job);
        t = (t + 1) % m;
    }

    return schedule;
}