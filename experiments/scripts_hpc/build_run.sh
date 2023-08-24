cd ../../
mkdir -p build
cd build
cmake ..
make
OMP_NUM_THREADS=$1 ./Bachelor_Thesis $2 $3 $4 $5 $6 $7 $8 $9 ${10}