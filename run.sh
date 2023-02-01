DIRECTORY=./DataSet

#make clean; make
./fm -f iris.txt -m 2 #CPU OMP
./fm -f iris.txt -m 1 #CPU
./fm -f iris.txt -m 0 #GPU

#python ./Matplot/ExecutionTime/one.py ./Results/Benchmark/ ./Matplot/ExecutionTime/test.jpg
