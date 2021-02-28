CXX = g++
CFLAGS = -O0 -g -std=c++17 -D _GLIBCXX_DEBUG -D _GLIBCXX_DEBUG_PEDANTIC

multiprec.o: multiprec.hpp multiprec.cpp 
	g++ multiprec.cpp -c $(CFLAGS)

bigint.o: multiprec.hpp bigint.cpp bigint.hpp multiprec.hpp
	g++ bigint.cpp -c $(CFLAGS)
bigfloat.o: multiprec.hpp bigfloat.cpp bigfloat.hpp multiprec.hpp
	g++ bigfloat.cpp -c $(CFLAGS)

main.o: main.cpp
	g++ main.cpp -c $(CFLAGS)

test.o: test.cpp
	g++ test.cpp -c $(CFLAGS)

pi: main.o bigint.o bigfloat.o multiprec.o
	g++ main.o bigint.o bigfloat.o multiprec.o -o pi $(CFLAGS) -pthread -lgmp -I/home/akihiro/vcpkg/packages/gmp_x64-linux/include -L/home/akihiro/vcpkg/packages/gmp_x64-linux/lib

test: test.o bigint.o bigfloat.o multiprec.o
	g++ test.o bigint.o bigfloat.o multiprec.o -o test $(CFLAGS)

check: checker.cpp
	g++ checker.cpp -o check

clean:
	rm -f *.o test pi