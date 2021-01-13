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

int main(){
    
    BigInt a,b;
    a = BigInt(10000)*BigInt(10000)*BigInt(10000)*BigInt(10000)
        a
    

    return 0;
}