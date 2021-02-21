#include "multiprec.hpp"
#include <algorithm>

using namespace std::literals::complex_literals;
const cmplx PI2 = 6.28318530717958647692;
using namespace std;

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