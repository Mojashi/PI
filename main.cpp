#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <iostream>
#include <boost/multiprecision/gmp.hpp>

using namespace std;
namespace mp = boost::multiprecision;

const int N = 10000;

using BigFloat = mp::number<mp::gmp_float<N>>;

using BigInt = mp::mpz_int;

const BigInt A =13591409 ,B=545140134, C=640320;

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
    static BigInt CT = C*C*C;
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


int main (){
    BigFloat p = 0, CF =BigFloat(12) / mp::sqrt(BigFloat(C)*BigFloat(C)*BigFloat(C));

    for(int i = 0; N /14 > i; i++){
        cerr << i << endl;
        p +=  CF * AP(i);
    }

    cout << setprecision(N) << 1/p << endl;
    return 0;
}
