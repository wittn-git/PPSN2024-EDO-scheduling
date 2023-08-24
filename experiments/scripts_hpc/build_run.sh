cd ../../
mkdir -p build
cd build
cmake ..
make
OMP_NUM_THREADS=4 ./Bachelor_Thesis $1 $2 $3 $4 $5 $6 $7 $8 $9