#include "bigint.hpp"
#include <complex>
#include <algorithm>
using namespace std;
int main(){
    BigInt a(100000LL),b(3149LL), c(12311LL);
    int hn = max({a.limbs.size(),b.limbs.size(),c.limbs.size()});
    Fourier fa = a.FFT(hn),fb = b.FFT(hn),fc = c.FFT(hn);
    BigInt ba = (fa * fb).IFFT(),bb = (fa * fc).IFFT();
    ba.print();
    bb.print();
    (a * b).print();
    (a + b).print();
}