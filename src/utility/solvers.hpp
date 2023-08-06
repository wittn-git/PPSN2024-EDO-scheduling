#pragma once

#include <vector>
#include <numeric>
#include <iostream>
#include <assert.h>

using T = std::vector<std::vector<int>>;

std::vector<int> moores_algorithm(std::vector<int> processing_times, std::vector<int> due_dates){
    assert(processing_times.size() == due_dates.size());

    auto first_late = [&](std::vector<int> schedule) {
        int current_time = 0;
        for(int i = 0; i < schedule.size(); i++){
            if(processing_times[schedule[i]] + current_time > due_dates[schedule[i]]) return i;
            current_time += processing_times[schedule[i]];
        }
        return -1;
    };

    int n = processing_times.size();
    std::vector<int> schedule(n);
    std::iota(schedule.begin(), schedule.end(), 0);   
    std::vector<int> rejected_jobs;

    std::sort(schedule.begin(), schedule.end(), [&](int a, int b) {
        return processing_times[a] < processing_times[b];
    });

    while(true){
        int first_late_index = first_late(schedule);
        if(first_late_index == -1) break;
        int first_late_job = schedule[first_late_index];
        std::vector<int> early_jobs(schedule.begin(), schedule.begin() + first_late_index + 1);
        std::vector<int> late_jobs(schedule.begin() + first_late_index + 1, schedule.end());
        std::sort(early_jobs.begin(), early_jobs.end(), [&](int a, int b) {
            return due_dates[a] < due_dates[b];
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

T approximation_algorithm(std::vector<int> processing_times, std::vector<int> due_dates, int m){
    assert(processing_times.size() == due_dates.size() && m <= processing_times.size());
    int n = processing_times.size();
    std::vector<int> jobs(n);
    std::iota(jobs.begin(), jobs.end(), 0);   
    std::sort(jobs.begin(), jobs.end(), [&](int a, int b) {
        return processing_times[a] > processing_times[b];
    });

    std::vector<std::vector<int>> schedule(m);
    for(int i = 0; i < m; i++){
        schedule[i].emplace_back(jobs[0]);
        jobs.erase(jobs.begin());
    }
    std::vector<int> machines_times(m, 0);
    for(int i = 0; i < m; i++){
        machines_times[i] += processing_times[schedule[i][0]];
    }
    
    while(!jobs.empty()){
        int machine = std::min_element(machines_times.begin(), machines_times.end()) - machines_times.begin();
        schedule[machine].emplace_back(jobs[0]);
        machines_times[machine] += processing_times[jobs[0]];
        jobs.erase(jobs.begin());
    }

    return schedule;
}