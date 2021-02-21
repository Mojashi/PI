#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <chrono>
#include <complex>
#include "bigint.hpp"

using namespace std;
using ll = long long;
const ll N = 1000000;
const ll n = N / 14;

const ll A = 13591409, B = 545140134, C = 640320;
const ll CT = C * C * C;

class M{
public:
    BigInt X, Y, Z;
};

const auto CT24 = BigInt(C * C * C / 24);
inline void calcX(const long long k, const BigInt& bk, BigInt& X)
{
    if (k == 0) X = BigInt(1LL);
    else {
        X = bk * bk * bk * CT24;
    }
}


const BigInt AB(A * B);
inline void calcY(const BigInt& bk, BigInt& Y)
{
    Y = bk * AB;
}

inline void calcZ(long long k, BigInt& Z)
{
    if (k == n - 1) Z = BigInt(0LL);
    else {
        Z = BigInt(6 * k + 1) * BigInt(2*k + 1) * BigInt(-(6*k + 5));
        // -1 * (6 * k + 1) * BigInt(2 * k + 1) * (6 * k + 5);
    }
    //(6k+1)(6k+2)(6k+3)(6k+4)(6k+5)(6k+6)
}

inline void mul(const M &lm, const M &rm, M &m, const int tnum)
{
    m.X = lm.X * rm.X;
    m.Y = lm.Z * rm.Y + lm.Y * rm.X;
    m.Z = lm.Z * rm.Z;
}

void calcM(ll l, ll r, M &m, int depth = 0, int tn = 0)
{
    if (r == l)
    {
        m.X = BigInt(1LL);
        m.Y = BigInt(0LL);
        m.Z = BigInt(1LL);
    }
    else if (r - l == 1)
    {
        BigInt bk(l);
        calcX(l, bk, m.X); calcY(bk, m.Y); calcZ(l, m.Z);
    }
    else
    {
        ll mid = (l + r) / 2;
        M lm, rm;
        if (tn > 0)
        {
            thread th(&calcM, mid, r, std::ref(rm), depth + 1, tn - 1 - (tn - 1) / 2);
            calcM(l, mid, lm, depth + 1, (tn - 1) / 2);
            th.join();
        }
        else
        {
            calcM(mid, r, rm, depth + 1);
            calcM(l, mid, lm, depth + 1);
        }
        mul(lm, rm, m, tn);
    }
}

chrono::system_clock::time_point stopwatch;
void reset_stopwatch(){
    stopwatch = chrono::system_clock::now();
}

double get_elapsed_time(){
    return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - stopwatch).count();
}

int main(int argc, char *argv[])
{
    // BigFloat p;

    int tnum = thread::hardware_concurrency();
    if (argc > 1)
        tnum = atoi(argv[1]);
    cerr << N << endl
         << tnum << endl;

    reset_stopwatch();
    M m;
    calcM(0, n, m,0, tnum - 1);
    cerr << "calcM:" << get_elapsed_time()/1000 << "sec" << endl;
    reset_stopwatch();
    // p = (mp::sqrt(BigFloat(CT))* mp::mpz_int(m.X)) / (12 * mp::mpz_int(m.Y));
    // cerr << "calcP:" << get_elapsed_time()/1000 << "sec" << endl;
    // cout << setprecision(N) << p << endl;

    return 0;
}

// int main(){
//     mpz_t x, y;
//     mpz_init(x);
//     mpz_init(y);
//     mpz_set_str(x, "12312387371824714151626364421214421421421421421214127371662764818287236", 10);
//     mpz_set_str(y, "-123415162636127371662764811213123412837128466126471264812418287236", 10);


//     for(int i = 0; 100 > i; i++){
//         mpz_t res;
//         mpz_init(res);
//         mpz_random(x, 10);
//         mpz_random(y, 1);
//         BigInt bx(x),by(y);
//         cout << bx << endl << by << endl;
//         multiply(x,y,res, 1);
//         cout << bx * by - res << endl;
//         mpz_clear(res);
//     }
// }