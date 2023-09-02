#!/bin/bash

start_job() {
    JOB_NAME=$2_$3_a${8//./}_l${9//./}
    OUTPUT_FILE=output_$JOB_NAME.csv
    sed "s/JOB_NAME/$JOB_NAME/g; s/WALL_TIME/$1/g" job_template.sh > temp_job.sh
    sbatch temp_job.sh $2 $3 $OUTPUT_FILE $4 $5 $6 $7 $8 $9
    sleep 10
    rm temp_job.sh
}

if [ "$#" -ne 9 ]; then
    echo "Usage: $0 <walltime> <algorithm> <mutation_operator> <runs> <mus> <ns> <ms> <alphas> <lambdas>"
    exit 1
fi

for alpha in $8; do
    for lambda in $9; do
        start_job $1 $2 $3 $4 $5 $6 $7 $alpha $lambda
    done
done