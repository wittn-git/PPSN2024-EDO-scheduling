#!/bin/bash

parse_array() {
    IFS=',' read -ra array <<< "$1"
    echo ${array[@]}
}

start_job() {
    JOB_NAME=$2_$3_mu${4//,/-}_n${5//,/-}_m${6//,/-}_a${8//./}_l${9//./}_${10}
    OUTPUT_FILE=output_$JOB_NAME.csv
    sed "s/JOB_NAME/$JOB_NAME/g; s/WALL_TIME/$1/g" job_template.sh > temp_job.sh
    sbatch temp_job.sh $2 $3 $OUTPUT_FILE $4 $5 $6 $7 $8 $9
    rm temp_job.sh
}

if [ "$#" -ne 10 ]; then
    echo "Usage: $0 <walltime> <algorithm> <mutation_operator> <runs> <mus> <ns> <ms> <alphas> <lambdas> <output suffix>"
    exit 1
fi

alphas=$(parse_array $8)
lambdas=$(parse_array $9)

for alpha in ${alphas[@]}; do
    for lambda in ${lambdas[@]}; do
        start_job $1 $2 $3 $4 $5 $6 $7 $alpha $lambda ${10}
    done
done