#include "bigint.hpp"
#include "bigfloat.hpp"
#include <complex>
#include <algorithm>
#include <iostream>
#include <assert.h>
using namespace std;

void testInt(){
    unsigned long long ad = 4333123123123ULL,bd =31491323322212ULL;
    BigInt a(ad), b(bd);
    cout << (double)ad * bd << endl;
    cout << (a * b).toDouble() << endl;

    BigFloat af(ad * 1.0), bf(bd * 1.0);
    cout << (af * bf).toDouble() << endl;
}

void testFloat(){
    BigInt a(12000ULL),b(3ULL);

    assert(a > b);
    assert(!(a < a));
    (a-b).print();

    BigFloat af(12347474710.0), bf(123038383883.0);
    BigFloat afrac(af.fraction), bfrac(bf.fraction);
    cout << afrac.toDouble() << endl;
    cout << bfrac.toDouble() << endl;
    cout << BigFloat(af.fraction * bf.fraction).toDouble() << endl;
    cout << af.toDouble() << endl;
    cout << bf.toDouble() << endl;

    cout << (af * bf).toDouble() << endl;
    
}

int main(){
    BigFloat f(__inf());
    f.shrink();
    f.print();
    cout <<f.toDouble() << endl;
}