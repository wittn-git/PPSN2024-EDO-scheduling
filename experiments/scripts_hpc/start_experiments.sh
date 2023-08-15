#!/bin/bash

wall_time_short="0-12:00:00"
wall_time_medium="3-00:00:00"
wall_time_long="9-00:00:00"
mutation_operators_RAI="1RAI,XRAI"
mutation_operators_NSWAP="NSWAP"
algorithms="Mu1-const,Mu1-unconst"

mus="2,10,25,50"
ns="5,10,25"
ms="1,3,5,10"
alphas="0.2,0.5,1"
lambdas="0,0.2,0.5,1"
runs="30"

./start_experiment.sh $wall_time_short $algorithms $mutation_operators_RAI $runs $mus $ns $ms $alphas $lambdas "sm"
./start_experiment.sh $wall_time_short $algorithms $mutation_operators_NSWAP $runs $mus $ns "1" $alphas $lambdas "sm"
./start_experiment.sh $wall_time_medium $algorithms $mutation_operators_RAI $runs $mus "50" $ms $alphas $lambdas "50"
./start_experiment.sh $wall_time_medium $algorithms $mutation_operators_NSWAP $runs $mus "50" "1" $alphas $lambdas "50"
./start_experiment.sh $wall_time_long $algorithms $mutation_operators_RAI $runs $mus "100" $ms $alphas $lambdas "100"
./start_experiment.sh $wall_time_long $algorithms $mutation_operators_NSWAP $runs $mus "100" "1" $alphas $lambdas "100"

./start_experiment.sh $wall_time_short "Survivor-Opt" "1RAI" "30" "5,10,25" "15,30" "1,3" "-" "-" ""