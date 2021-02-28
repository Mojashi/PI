#pragma once
#include "bigint.hpp"

#define POSITIVE false
#define NEGATIVE true
    
class BigFloat{
public:
    bool sign = POSITIVE; //true is positive
    long long int exponent = 0;
    BigInt fraction;

    BigFloat(double a);
    BigFloat(BigInt a, long long int _exponent = 0, bool _sign = POSITIVE);
    BigFloat operator*(const BigFloat& b);
    BigFloat operator+(BigFloat& b);
    BigFloat operator-=(int b);
    BigFloat operator-(BigFloat& b);
    void changeExponent(long long int nexponent);
    void negate();
    void print();
    double toDouble();
    BigFloat reciprocal(unsigned long long int digit);
    void shrink();
};

BigFloat invsqrt(BigFloat& b, unsigned long long digit);
double testDiv(BigFloat& a, BigFloat& b);