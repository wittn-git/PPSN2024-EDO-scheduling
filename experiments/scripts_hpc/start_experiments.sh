#!/bin/bash

ns="5,10,25"
mus="2,10,25"
ms="1,3,5,10"
runs="30"

wall_time="7-00:00:00"

alphas="0.1 0.3 0.6"
lambdas="0.1 0.2 2" 

# algorithms=("Mu1-const" "Mu1-unconst" "NOAH")
algorithms=("Mu1-const" "Mu1-unconst")
mutation_operators=("1RAI" "XRAI" "NSWAP")

for algorithm in "${algorithms[@]}"; do
    if [ "$algorithm" == "Mu1-const" ]; then
        current_alpha="$alphas"
    else
        current_alpha="-"
    fi
    for mutation_operator in "${mutation_operators[@]}"; do
        current_lambdas="-"
        current_ms="$ms"
        if [ "$mutation_operator" == "XRAI" ]; then
            current_lambdas="$lambdas"
        elif [ "$mutation_operator" == "NSWAP" ]; then
            current_ms="1"
        fi
        ./start_experiment.sh "$wall_time" "$algorithm" "$mutation_operator" "$runs" "$mus" "$ns" "$current_ms" "$current_alpha" "$current_lambdas"
    done
done