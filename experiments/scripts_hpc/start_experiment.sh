#!/bin/bash

mkdir -p results
sed "s/JOB_NAME/$2/g; s/WALL_TIME/$1/g" job_template.sh > temp_job.sh
sbatch temp_job.slurm $3 $4 $5 $6 $7 $8
rm temp_job.slurm