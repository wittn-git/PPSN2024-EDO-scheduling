#!/bin/bash

#SBATCH --account=thes1549

#SBATCH --ntasks=1
#SBATCH --cpus-per-task=48
#SBATCH --mem-per-cpu=50M

#SBATCH --job-name=JOB_NAME
#SBATCH --output=output_JOB_NAME.txt
#SBATCH --time=WALL_TIME

sh build_run.sh $1 $2 $3 $4 $5 $6 $7 $8 $9