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


unsigned long long int extractBit(double v, int st, int en) {
    unsigned long long int uv = *(unsigned long long int*)(&v);
    return (uv << (64-en)) >> (64-en + st);
}

BigFloat::BigFloat(double a){
    sign = extractBit(a, 63, 64);
    int origexp = (extractBit(a, 52, 63) - 1023 - 52);
    int sa = (origexp % BASE_E + BASE_E) % BASE_E;
    int nexp = origexp - sa;
    exponent = nexp / BASE_E;
    fraction = BigInt((extractBit(a, 0, 52) | (1LL << 52)) << sa);
}

BigFloat BigFloat::reciprocal(unsigned long long int digit){
    BigFloat r(1 / this->toDouble());
    BigFloat one(BigInt(1ULL));
    for(unsigned long long int i = 50; digit > i; i *= 2){
        BigFloat bufr = r;
        r = (r * (*this) - one) * bufr;
        r = bufr - r;
    }
    return r;
}

BigFloat BigFloat::operator*(const BigFloat& b){
    BigFloat ret(b.fraction * this->fraction, b.exponent + this->exponent);
    ret.shrink();
    return ret;
}

BigFloat BigFloat::operator+(BigFloat& b){
    long long int nexponent = min(exponent, b.exponent);
    changeExponent(nexponent);
    b.changeExponent(nexponent);

    BigFloat ret(0.0);
    if(b.sign == sign) {
        ret = BigFloat(fraction + b.fraction, nexponent, sign);
    } else {
        if(b.fraction < fraction){
            ret = BigFloat(fraction - b.fraction, nexponent, !b.sign);
        }
        else {
            ret = BigFloat(b.fraction - fraction, nexponent, b.sign);
        }
    }
    ret.shrink();
    return ret;
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
        vector<LIMB> buf(diff);
        fraction.limbs.insert(fraction.limbs.begin(), buf.begin(), buf.end());
    } else if(diff < 0){
        diff = max(0, -diff);
        fraction.limbs = vector<LIMB>(fraction.limbs.begin() + diff, fraction.limbs.end());
    }
}

void BigFloat::negate(){
    sign = !sign;
}

void BigFloat::print(){
    fraction.print();
}

double BigFloat::toDouble(){
    double ret = 0;
    // double base = pow(BASE, exponent + fraction.limbs.size() - 1);
    for(int i = 0; fraction.limbs.size() > i; i++) {
        if((exponent + i) * BASE_E < -50 ||(exponent + i) * BASE_E > 50 || fraction.limbs[i] == 0) continue;
        double base = pow(BASE, exponent + i);
        ret += fraction.limbs[i] * base;
        // base /= BASE;
    }
    return ret;
}

BigFloat invsqrt(BigFloat& b, unsigned long long digit){
    BigFloat r(1 / sqrt(b.toDouble()));
    BigFloat one(BigInt(1ULL));
    BigFloat invtwo = BigFloat(BigInt(2ULL << (BASE_E-1)),-1);
    for(unsigned long long int i = 50; digit > i; i *= 2){
        cout << r.toDouble() << endl;
        cout << r.fraction.size() << endl;
        cout << r.fraction.MSL() << endl;
        BigFloat bufr = r;
        bufr = (r * r * b - one) * r * invtwo;
        r = r-bufr;
    }
    return r;
}

void BigFloat::shrink(){
    int lsl = fraction.LSL(), msl = fraction.MSL();

    exponent += lsl;
    fraction.limbs = vector<LIMB>(fraction.limbs.begin() + lsl, fraction.limbs.begin() + msl + 1);
}