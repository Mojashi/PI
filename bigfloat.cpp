#include "bigfloat.hpp"
#include "bigint.hpp"
#include <algorithm>
#include <iostream>
using namespace std;

BigFloat::BigFloat(BigInt a, long long int _exponent, bool _sign){
    fraction = a;
    exponent = _exponent;
    sign = _sign;
}

BigFloat BigFloat::reciprocal(unsigned long long int digit){
    BigFloat r = *this;
    BigFloat one(BigInt(1LL));
    for(unsigned long long int i = 1; digit > i; i *= 2){
        BigFloat bufr = r;
        r = (r * (*this) - one) * bufr;
        r = bufr - r;
    }
    return r;
}

BigFloat BigFloat::operator*(const BigFloat& b){
    return BigFloat(b.fraction * this->fraction, b.exponent + this->exponent);
}

BigFloat BigFloat::operator+(BigFloat& b){
    long long int nexponent = min(exponent, b.exponent);
    changeExponent(nexponent);
    b.changeExponent(nexponent);

    if(b.sign == sign) {
        return BigFloat(fraction + b.fraction, nexponent, sign);
    } else {
        if(b.fraction < fraction){
            return BigFloat(fraction - b.fraction, nexponent, !b.sign);
        }
        else {
            return BigFloat(b.fraction - fraction, nexponent, b.sign);
        }
    }
}

BigFloat BigFloat::operator-(BigFloat& b){
    b.negate();
    BigFloat ret = (*this) + b;
    b.negate();
    return ret;
}


void BigFloat::changeExponent(long long int nexponent){
    int diff = exponent - nexponent;
    exponent = nexponent;
    if(diff > 0){
        vector<long long> buf(diff);
        fraction.limbs.insert(fraction.limbs.begin(), buf.begin(), buf.end());
    } else if(diff < 0){
        diff = max(0, -diff);
        fraction.limbs = vector<long long>(fraction.limbs.begin() + diff, fraction.limbs.end());
    }
}

void BigFloat::negate(){
    sign = !sign;
}

void BigFloat::print(){
    std::cout << exponent << std::endl;
    fraction.print();
}

double BigFloat::toDouble(){
    double ret = 0;
    double base = pow(BASE, exponent);
    for(auto a : fraction.limbs){
        ret += base * a;
        base *= BASE;
    }
    return ret;
}