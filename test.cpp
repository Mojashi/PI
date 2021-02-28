#include "bigint.hpp"
#include "bigfloat.hpp"
#include <complex>
#include <algorithm>
#include <iostream>
#include <assert.h>
using namespace std;

void testInt(){

    BigInt a(100000ULL),b(3149ULL), c(12311ULL);
    int hn = max({a.limbs.size(),b.limbs.size(),c.limbs.size()});
    Fourier fa = a.FFT(hn),fb = b.FFT(hn),fc = c.FFT(hn);
    BigInt ba = (fa * fb).IFFT(),bb = (fa * fc).IFFT();
    ba.print();
    bb.print();
    (a * b).print();
    (a + b).print();
}

void testFloat(){
    BigInt a(12000ULL),b(3ULL);

    assert(a > b);
    assert(!(a < a));
    (a-b).print();

    BigFloat af(a), bf(b);
    cout << invsqrt(bf, 10).toDouble() << endl;

    cout << af.toDouble() << endl;
    cout << bf.reciprocal(10).toDouble() << endl;
}

int main(){
    testFloat();
}