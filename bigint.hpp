#pragma once
#include <complex>
#include <vector>
using std::vector;

using cmplx = std::complex<double>;

class BigInt;

class Fourier {
public:
    vector<cmplx> limbs;
    Fourier(vector<cmplx> _limbs);
    Fourier(int sz);
    void operator*=(const Fourier& b);
    Fourier operator*(const Fourier& b) const ;
    BigInt IFFT();
    void print();
};

class BigInt {
public:
    void normalize();
    vector<long long> limbs;
    BigInt();
    BigInt(size_t sz);
    BigInt(long long a);
    BigInt operator* (const BigInt& b) const;
    void operator+= (const BigInt& b);
    BigInt operator+ (const BigInt& b) const;
    Fourier FFT(size_t hn);
    size_t size();
    void print();
};