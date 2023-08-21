#!/bin/bash

wall_times=("0-12:00:00" "3-00:00:00" "6-00:00:00")
n_batches=("5,10,25" "50" "100")
suffixes=("sm" "50" "100")

mus="2,10,25,50"
ms="1,3,5,10"
alphas="0.2,0.5,1"
lambdas="0,0.25,1" 
runs="30"

for ((i = 0; i < ${#wall_times[@]}; i++)); do
    ./start_experiment.sh "${wall_times[i]}" "NOAH" "1RAI" $runs $mus "${n_batches[i]}" $ms "-" "-" "${suffixes[i]}"
    ./start_experiment.sh "${wall_times[i]}" "NOAH" "XRAI" $runs $mus "${n_batches[i]}" $ms "-" $lambdas "${suffixes[i]}"
    ./start_experiment.sh "${wall_times[i]}" "NOAH "NSWAP" $runs $mus "${n_batches[i]}" "1" "-" "-" "${suffixes[i]}"
done

: '
for ((i = 0; i < ${#wall_times[@]}; i++)); do
    ./start_experiment.sh "${wall_times[i]}" "Mu1-unconst" "1RAI" $runs $mus "${n_batches[i]}" $ms "-" "-" "${suffixes[i]}"
    ./start_experiment.sh "${wall_times[i]}" "Mu1-const" "1RAI" $runs $mus "${n_batches[i]}" $ms $alphas "-" "${suffixes[i]}"
    ./start_experiment.sh "${wall_times[i]}" "Mu1-unconst" "XRAI" $runs $mus "${n_batches[i]}" $ms "-" $lambdas "${suffixes[i]}"
    ./start_experiment.sh "${wall_times[i]}" "Mu1-const" "XRAI" $runs $mus "${n_batches[i]}" $ms $alphas $lambdas "${suffixes[i]}"
    ./start_experiment.sh "${wall_times[i]}" "Mu1-unconst" "NSWAP" $runs $mus "${n_batches[i]}" "1" "-" "-" "${suffixes[i]}"
    ./start_experiment.sh "${wall_times[i]}" "Mu1-const" "NSWAP" $runs $mus "${n_batches[i]}" "1" $alphas "-" "${suffixes[i]}"
done


./start_experiment.sh "0-01:00:00" "Survivor-Opt" "1RAI" "30" "10,25" "40" "1" "-" "-" "-"
'