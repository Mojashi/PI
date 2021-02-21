#include "bigint.hpp"
#include "bigfloat.hpp"
#include <complex>
#include <algorithm>
#include <iostream>
#include <assert.h>
using namespace std;

void testInt(){

    BigInt a(100000LL),b(3149LL), c(12311LL);
    int hn = max({a.limbs.size(),b.limbs.size(),c.limbs.size()});
    Fourier fa = a.FFT(hn),fb = b.FFT(hn),fc = c.FFT(hn);
    BigInt ba = (fa * fb).IFFT(),bb = (fa * fc).IFFT();
    ba.print();
    bb.print();
    (a * b).print();
    (a + b).print();
}

void testFloat(){
    BigInt a(100000LL),b(3149LL);

    assert(a > b);
    assert(!(a < a));
    (a-b).print();

    BigFloat af(a), bf(b);

    cout << af.toDouble() << endl;
    cout << af.reciprocal(10).toDouble() << endl;
}

int main(){
    testFloat();
}