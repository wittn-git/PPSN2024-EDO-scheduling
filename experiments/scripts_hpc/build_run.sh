cd ../../
mkdir -p build
cd build
cmake ..
make
omp_set_num_threads=40 ./Bachelor_Thesis $1 $2 $3 $4 $5 $6 $7 $8 $9