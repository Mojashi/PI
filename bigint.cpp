#include "bigint.hpp"
#include <vector>
#include <complex>
#include <cassert>

const int BASE_E = 13;
const int BASE = 1 << BASE_E;
const int BASE_MASK = BASE - 1;

using namespace std;
using namespace std::literals::complex_literals;
const cmplx PI2 = 6.28318530717958647692;


constexpr unsigned int bitreverse(unsigned int n)
{
#ifdef __has_builtin
#if __has_builtin(__builtin_bitreverse32)
    return __builtin_bitreverse32(n);
#else
    n = ((n & 0x55555555) << 1) | ((n & 0xAAAAAAAA) >> 1); // 1ビットの交換
    n = ((n & 0x33333333) << 2) | ((n & 0xCCCCCCCC) >> 2);
    n = ((n & 0x0F0F0F0F) << 4) | ((n & 0xF0F0F0F0) >> 4);
    n = ((n & 0x00FF00FF) << 8) | ((n & 0xFF00FF00) >> 8);
    n = (n << 16) | (n >> 16);
    return n;
#endif
#else
    n = ((n & 0x55555555) << 1) | ((n & 0xAAAAAAAA) >> 1); // 1ビットの交換
    n = ((n & 0x33333333) << 2) | ((n & 0xCCCCCCCC) >> 2);
    n = ((n & 0x0F0F0F0F) << 4) | ((n & 0xF0F0F0F0) >> 4);
    n = ((n & 0x00FF00FF) << 8) | ((n & 0xFF00FF00) >> 8);
    n = (n << 16) | (n >> 16);
    return n;
#endif
}

int getFSize(int n){
    int k = 1, l = 0;
    while(k < n) k<<=1,l++;
    return l;
}

void fft(vector<cmplx>& A){
    int k = getFSize(A.size());
    int i,j,l,n = 1 << k;
    vector<cmplx> tbl(n);
    cmplx buf;
    for(int i = 0; n > i; i++){
        int to = bitreverse(i) >> (32-k);
        if(to < i) {
            cmplx buf = A[i];
            A[i] = A[to];
            A[to] = buf;
        }
    }
    
    tbl[0] = 1;
    for(j = 1; k >= j; j++){
        int sn = 1 << j;
        for(i = 0; i < j; i++) 
            tbl[1<<i] = exp(-1i*PI2*cmplx((1<<i))/cmplx(sn));
        for(i = 0; sn/2 > i; i++) {
            if(i!=(i&-i))
                tbl[i] = tbl[i&-i]*tbl[i^(i&-i)];
            
            for(l = 0; n > l; l += sn){
                buf = A[l+i+sn/2] * tbl[i];
                A[l+i] += buf;
                A[l+i+sn/2] = A[l+i] - cmplx(2)*buf;
            }
        }
    }
}

void ifft(vector<cmplx>& ar){
    for(auto& a : ar){
        a = conj(a);
    }
    fft(ar);
    for(auto& a : ar){
        a = conj(a) / cmplx(ar.size());
    }
}

void convolve(const vector<long long>& _A, const vector<long long>& _B, vector<long long>& C){
    const long long *A = &_A[0], *B = &_B[0];
    if(_A.size() < _B.size()) swap(A,B);

    int n = max(_A.size(), _B.size()), m = min(_A.size(), _B.size());
    int l = getFSize(n) + 1, k = 1 << l;
    vector<cmplx> CH(k), CC(k/2);

    for(int i = 0; m > i; i++)
        CH[i] = cmplx(A[i]) + cmplx(B[i])*1i;
    for(int i = m; n > i; i++)
        CH[i] = cmplx(A[i]);

    fft(CH);

    for(int i = 0; k/2 > i; i++){
        cmplx buf = i==0?conj(CH[0]):conj(CH[k-i]);
        cmplx buf2 = conj(CH[k/2-i]);
        // printf("a:%lf,%lf\n", ((CH[i] + buf)/cmplx(2)).real(), ((CH[i] + buf)/cmplx(2)).imag());
        cmplx fr = conj((CH[i] + buf) * (CH[i] - buf) / 4i);
        cmplx bk = conj((CH[k/2+i] + buf2) * (CH[k/2+i] - buf2) / 4i);
        CC[i] = ((fr+bk) +  1i*(fr-bk)*exp(cmplx(-i)*1i*PI2/cmplx(k)))/cmplx(2);
    }
    fft(CC);
    
    for(int i = 0; k/2 > i; i++){
        C[i*2] = 2*round(CC[i].real())/k;
        C[i*2 + 1] = 2*round(CC[i].imag())/k;
    }
}

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
        printf("%d\n", limbs[i]);
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

BigInt BigInt::operator+ (const BigInt& b) const {
    BigInt ret((size_t)0);
    ret.limbs = limbs;
    ret += b;
    return ret;
}