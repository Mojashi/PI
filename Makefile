CXX = g++-8

CFLAGS = -O0 -g -std=c++17 -D _GLIBCXX_DEBUG -D _GLIBCXX_DEBUG_PEDANTIC -D_DEBUG

ifeq ($(BUILD),release)
	CFLAGS = -O3 -std=c++17
endif

multiprec.o: multiprec.hpp multiprec.cpp 
	$(CXX) multiprec.cpp -c $(CFLAGS)

bigint.o: multiprec.hpp bigint.cpp bigint.hpp multiprec.hpp
	$(CXX) bigint.cpp -c $(CFLAGS)
bigfloat.o: multiprec.hpp bigfloat.cpp bigfloat.hpp multiprec.hpp
	$(CXX) bigfloat.cpp -c $(CFLAGS)

main.o: main.cpp
	$(CXX) main.cpp -c $(CFLAGS)

test.o: test.cpp
	$(CXX) test.cpp -c $(CFLAGS)

pi: main.o bigint.o bigfloat.o multiprec.o
	$(CXX) main.o bigint.o bigfloat.o multiprec.o -o pi $(CFLAGS) -pthread

test: test.o bigint.o bigfloat.o multiprec.o
	$(CXX) test.o bigint.o bigfloat.o multiprec.o -o test $(CFLAGS)

check: checker.cpp
	$(CXX) checker.cpp -o check

clean:
	rm -f *.o test pi