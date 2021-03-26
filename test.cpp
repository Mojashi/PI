#include "bigint.hpp"
#include "bigfloat.hpp"
#include <complex>
#include <algorithm>
#include <iostream>
#include <assert.h>
#include <boost/multiprecision/cpp_int.hpp>

namespace mp = boost::multiprecision;
using namespace std;

void testInt(){
    unsigned long long ad = 4333123123123ULL,bd =31491323322212ULL;
    BigInt a(ad), b(bd);
    a = a * b * a * a * b * b * b;
    a = a * a * a;
    a = a * a * a;
    a = a * a * a;
    a = a * a * a;
    a = a * a * a;
    a = a * a * a;
    a = a * a * a;
    b = a * b * b * b * b * b * b * b;

    BigInt c = a * b;
    cout << c.toDouble() << endl;
}

void testFloat(){
    BigFloat af(7.0), bf(123038383883.0);
    cout << bf.reciprocal(10000).toDouble() << endl;
    cout << invsqrt(af, 10000).toDouble() << endl;
}

int main(){
    testFloat();
}