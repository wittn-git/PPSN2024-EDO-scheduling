#!/bin/bash

#SBATCH --ntasks=1
#SBATCH --mem-per-cpu=150M

#SBATCH --mail-user=dominic.wittner@rwth-aachen.de
#SBATCH --mail-type=ALL

#SBATCH --job-name=JOB_NAME
#SBATCH --output=output_JOB_NAME.txt
#SBATCH --time=WALL_TIME

sh build_run.sh $1 $2 $3 $4 $5 $6 $7 $8 $9 ${10}