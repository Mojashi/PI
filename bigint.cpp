#include "bigint.hpp"
#include <vector>
#include <complex>
#include <cassert>

using namespace std;

Fourier::Fourier(vector<cmplx> _limbs):limbs(_limbs){}
Fourier::Fourier(int sz):limbs(sz){}
BigInt Fourier::IFFT(){
    BigInt ret(limbs.size());
    vector<cmplx> buf = limbs;
    ifft(buf);
    for(int i = 0; limbs.size() > i; i++){
        ret.limbs[i] = round(buf[i].real());
    }
    ret.normalize();
    return ret;
}

void Fourier::operator*=(const Fourier& b){
    assert(limbs.size() == b.limbs.size());
    for(int i = 0; limbs.size() > i; i++){
        limbs[i] *= b.limbs[i];
    }
}

Fourier Fourier::operator*(const Fourier& b) const {
    Fourier ret(limbs.size());
    assert(limbs.size() == b.limbs.size());
    for(int i = 0; limbs.size() > i; i++){
        ret.limbs[i] = limbs[i] * b.limbs[i];
    }
    return ret;
}

BigInt::BigInt() : limbs(1){}
BigInt::BigInt(size_t sz) : limbs(sz){}
BigInt::BigInt(long long a){
    if(a == 0) limbs.push_back(0);
    while(a > 0){
        limbs.push_back(a % BASE);
        a /= BASE;
    }
}
BigInt BigInt::operator* (const BigInt& b) const {
    size_t n = max(limbs.size(), b.limbs.size());
    BigInt c(size_t(1 << (getFSize(n) + 1)));
    convolve(limbs, b.limbs, c.limbs);
    c.normalize();
    return c;
}
Fourier BigInt::FFT(size_t hn){
    Fourier f(1 << (getFSize(max(hn, limbs.size())) + 1));
    for(int i = 0; limbs.size() > i; i++)
        f.limbs[i] = limbs[i];
    fft(f.limbs);
    return f;
}
void BigInt::print(){
    long long int sum = 0, b = 1;
    for(int i = 0; limbs.size() > i; i++){
        printf("%d ", limbs[i]);
        sum += b * limbs[i];
        b *= BASE;
    }
    printf("%lld\n", sum);
}
void Fourier::print(){
    for(int i = 0; limbs.size() > i; i++){
        printf("%lf,%lf\n", limbs[i].real(), limbs[i].imag());
    }
}

void BigInt::normalize(){
    long long carry = 0;
    for(int i = 0; limbs.size() > i; i++){
        limbs[i] += carry;
        carry = 0;
        if(limbs[i] >= BASE){
            carry = limbs[i] >> BASE_E;
            limbs[i] &= BASE_MASK;
        }
    }
    while(carry>0){
        limbs.push_back(carry & BASE_MASK);
        carry >>= BASE_E;
    }
}

size_t BigInt::size() {
    return limbs.size();
}

void BigInt::operator+= (const BigInt& b){
    if(limbs.size() < b.limbs.size())
        limbs.resize(b.limbs.size());
    long long carry = 0;
    for(int i = 0; b.limbs.size() > i; i++){
        limbs[i] += b.limbs[i] + carry;
        carry = 0;
        if(limbs[i] >= BASE){
            carry = limbs[i] >> BASE_E;
            limbs[i] &= BASE_MASK;
        }
    }
    while(carry>0){
        limbs.push_back(carry & BASE_MASK);
        carry >>= BASE_E;
    }
}

void BigInt::operator-= (const BigInt& b){
    if(limbs.size() < b.limbs.size())
        limbs.resize(b.limbs.size());
    long long carry = 0;
    for(int i = 0; b.limbs.size() > i; i++){
        limbs[i] -= b.limbs[i] - carry;
        carry = 0;
        if(limbs[i] < 0){
            carry = 1;
            limbs[i] += BASE;
        }
    }
    assert(carry == 0);
}

BigInt BigInt::operator+ (const BigInt& b) const {
    BigInt ret((size_t)0);
    ret.limbs = limbs;
    ret += b;
    return ret;
}

bool BigInt::operator>(const BigInt& b)const {
    for(int i = max(limbs.size(), b.limbs.size()) - 1; 0 <= i; i--){
        int an = limbs.size() > i ? limbs[i] : 0;
        int bn = b.limbs.size() > i ? b.limbs[i] : 0;
        if(an > bn) return true;
        if(bn > an) return false;
    }
    return false;
}

bool BigInt::operator<(const BigInt& b) const {
    for(int i = max(limbs.size(), b.limbs.size()) - 1; 0 <= i; i--){
        int an = limbs.size() > i ? limbs[i] : 0;
        int bn = b.limbs.size() > i ? b.limbs[i] : 0;
        if(bn > an) return true;
        if(an > bn) return false;
    }
    return false;
}

BigInt BigInt::operator- (const BigInt& b) const {
    BigInt ret((size_t)0);
    ret.limbs = limbs;
    ret -= b;
    return ret;
}