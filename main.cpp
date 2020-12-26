#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <iostream>
#include <boost/multiprecision/gmp.hpp>
#include <thread>
#include <vector>
#include <future>

using namespace std;
namespace mp = boost::multiprecision;

const int N = 10000000;
const int n = N/14;

using BigFloat = mp::number<mp::gmp_float<N>>;

using BigInt = mp::mpz_int;

const BigInt A =13591409 ,B=545140134, C=640320;
const BigInt CT = C*C*C;

BigInt fac(int k) {
    BigInt ret = 1;
    for(int i = 1; k >= i; i++){
        ret *= i;
    }
    return ret;
}


BigFloat P(int k){
    BigInt si = (k % 2 ? -1 : 1) * fac(6 * k) * (A + B * k);
    BigInt bo = fac(3 * k) * mp::pow(fac(k), 3) * pow(C, 3 * k);

    return BigFloat(si) / BigFloat(bo);
}


BigFloat AP(long long int k){
    static BigInt si = 0;
    static BigInt bo = 0;
    static BigInt ABK = A;
    if(k == 0) {
        si = A;
        bo = 1;
    }else {
        bo *= k*k*k*(CT*ABK);
        ABK += B;
        si *= -24 * (6 * k -5)*(2 *k - 1) * (6 * k -1) * ABK;
    }

    return BigFloat(si) / BigFloat(bo);
}

struct M {
    BigInt X,Y,Z;
};

const BigInt CT24 = C*C*C/24;
inline BigInt calcX(long long k){ 
    if(k == 0) return 1;
    return CT24*k*k*k;
}

inline BigInt calcY(long long k){ 
    return A+B*k;
}

inline BigInt calcZ(long long k){ 
    if(k == n-1) return 0;
    return -1*BigInt(6*k+1)*(2*k + 1)*(6 * k + 5);
    //(6k+1)(6k+2)(6k+3)(6k+4)(6k+5)(6k+6)
}

inline M mul(const M &lm, const M& rm){
    return {lm.X * rm.X, lm.Z*rm.Y + lm.Y*rm.X, lm.Z*rm.Z};
}

M calcM(int l, int r){
    if(r == l) {
        return {1,0,1};
    }
    if(r - l == 1) {
        return {calcX(l),calcY(l),calcZ(l)};
    }
    
    int mid = (l + r) / 2;
    M lm = calcM(l, mid), rm = calcM(mid, r);
    return mul(lm, rm);
}

M threadedM(int l, int r, int tnum){
    vector<future<M>> fus;
    int len = (r-l) / tnum, c = l;
    for(int i = 0; tnum > i; i++){
        fus.push_back(async(&calcM, c, c + len));
        c += len;
    }
    M ret = {1,0,1};
    for(auto &fu : fus){
        ret = mul(ret, fu.get());
    }
    return ret;
}

int main (){
    BigFloat p;
    M m = calcM(0, n);
    p = sqrt(BigFloat(CT))*m.X/12/m.Y;
    cout << setprecision(N) << p << endl;
    return 0;
}
