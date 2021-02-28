#pragma once
#include <complex>
#include <vector>
#include "multiprec.hpp"
using std::vector;

typedef unsigned long long LIMB;

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
    vector<LIMB> limbs;

    BigInt();
    BigInt(size_t sz);
    BigInt(unsigned long long a);
    BigInt operator* (const BigInt& b) const;
    void operator+= (const BigInt& b);
    void operator-= (const BigInt& b);
    BigInt operator+ (const BigInt& b) const;
    BigInt operator-(const BigInt& b) const;
    bool operator>(const BigInt& b) const;
    bool operator<(const BigInt& b) const;
    Fourier FFT(size_t hn);
    size_t size();
    void print();
    unsigned long long int toULL();
    double toDouble();

    unsigned long long int MSL();
    unsigned long long int LSL();
};