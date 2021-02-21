multiprec.o: multiprec.hpp multiprec.cpp 
	g++ multiprec.cpp -c -std=c++17 -O3

bigint.o: multiprec.hpp bigint.cpp
	g++ bigint.cpp -c -std=c++17 -O3
bigfloat.o: multiprec.hpp bigfloat.cpp 
	g++ bigfloat.cpp -c -std=c++17 -O3

main.o: main.cpp
	g++ main.cpp -c -std=c++17 -O3

test.o: test.cpp
	g++ test.cpp -c -std=c++17 -O3

pi: main.o bigint.o bigfloat.o multiprec.o
	g++ main.o bigint.o bigfloat.o multiprec.o -o pi -O3 -std=c++17 -pthread -lgmp -I/home/akihiro/vcpkg/packages/gmp_x64-linux/include -L/home/akihiro/vcpkg/packages/gmp_x64-linux/lib

test: test.o bigint.o bigfloat.o multiprec.o
	g++ test.o bigint.o bigfloat.o multiprec.o -o test -O3 -std=c++17

check: checker.cpp
	g++ checker.cpp -o check
