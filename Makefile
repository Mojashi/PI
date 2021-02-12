bigint.o: bigint.cpp 
	g++ bigint.cpp -c -std=c++17 -O3

pi: main.cpp bigint.o
	g++ main.cpp bigint.o -o pi -O3 -std=c++17 -pthread -lgmp -I/home/akihiro/vcpkg/packages/gmp_x64-linux/include -L/home/akihiro/vcpkg/packages/gmp_x64-linux/lib

check: checker.cpp
	g++ checker.cpp -o check
