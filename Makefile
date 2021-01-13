pi: main.cpp
	g++ main.cpp -o pi -O3 -pthread -lgmp -I/home/akihiro/vcpkg/packages/gmp_x64-linux/include -L/home/akihiro/vcpkg/packages/gmp_x64-linux/lib

check: checker.cpp
	g++ checker.cpp -o check
