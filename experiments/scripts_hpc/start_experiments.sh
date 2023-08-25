#!/bin/bash

ns="5,10,25,50,100"
mus="2,10,25,50"
ms="1,3,5,10"
runs="30"

wall_time="7-00:00:00"

alphas="0.95 0.8 0.5"
lambdas="0 0.25 1" 

./start_experiment.sh $wall_time "Mu1-const" "1RAI" $runs $mus $ns $ms "$alphas" "-"
./start_experiment.sh $wall_time "Mu1-const" "XRAI" $runs $mus $ns $ms "$alphas" "$lambdas"
./start_experiment.sh $wall_time "Mu1-const" "NSWAP" $runs $mus $ns "1" "$alphas" "-"

./start_experiment.sh $wall_time "Mu1-unconst" "1RAI" $runs $mus $ns $ms "-" "-"
./start_experiment.sh $wall_time "Mu1-unconst" "XRAI" $runs $mus $ns $ms "-" "$lambdas"
./start_experiment.sh $wall_time "Mu1-unconst" "NSWAP" $runs $mus $ns "1" "-" "-"

./start_experiment.sh $wall_time "NOAH" "1RAI" $runs $mus $ns $ms "-" "-"
./start_experiment.sh $wall_time "NOAH" "XRAI" $runs $mus $ns $ms "-" "$lambdas"
./start_experiment.sh $wall_time "NOAH" "NSWAP" $runs $mus $ns "1" "-" "-"

./start_experiment.sh "0-01:00:00" "Survivor-Opt" "1RAI" "30" "10,25" "40" "1" "-" "-"