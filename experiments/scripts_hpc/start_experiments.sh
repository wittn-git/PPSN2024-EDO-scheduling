#!/bin/bash

wall_time="3-00:00:00"
mus="2,10,25,50"
ns="5,10,25,50,100"
ms="1,3,5,10"
alphas="0.2,0.5,1"
lambdas="0.2,0.5,1"
runs="30"

sh start_experiment.sh $wall_time "Mu1-const" "1RAI" $runs $mus $ns $ms $alphas $lambdas
sh start_experiment.sh $wall_time "Mu1-const" "XRAI" $runs $mus $ns $ms $alphas $lambdas
sh start_experiment.sh $wall_time "Mu1-unconst" "1RAI" $runs $mus $ns $ms $alphas $lambdas
sh start_experiment.sh $wall_time "Mu1-unconst" "XRAI" $runs $mus $ns $ms $alphas $lambdas
sh start_experiment.sh $wall_time "Noah" "1RAI" $runs $mus $ns $ms $alphas $lambdas
sh start_experiment.sh $wall_time "Noah" "XRAI" $runs $mus $ns $ms $alphas $lambdas

sh start_experiment.sh $wall_time "Mu1-const" "NSWAP" $runs $mus $ns "1" $alphas $lambdas
sh start_experiment.sh $wall_time "Mu1-unconst" "NSWAP" $runs $mus $ns "1" $alphas $lambdas
sh start_experiment.sh $wall_time "Noah" "NSWAP" $runs $mus $ns "1" $alphas $lambdas

sh start_experiment.sh $wall_time "Survivor-Opt" "1RAI" "50" "5,10,25" "15,30" "1,3" "" ""

# TODO check experiment parameters
# TODO check if experiments work