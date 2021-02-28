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
#include "bigfloat.hpp"

using namespace std;
using ll = long long;
const ll N = 500;
const ll n = (N / 14) + (N/14) % 2;

const ll A = 13591409, B = 545140134, C = 640320;
const ll CT = C * C * C;

class M{
public:
    BigInt X, Y, Z;
};

const auto CT24 = BigInt(C * C * C / 24ULL);
inline void calcX(const long long k, const BigInt& bk, BigInt& X)
{
    if (k == 0) X = BigInt(1ULL);
    else {
        X = bk * bk * bk * CT24;
    }
}


const BigInt AB(A * 1ULL);
const BigInt BB(B * 1ULL);
inline void calcY(const BigInt& bk, BigInt& Y)
{
    Y = bk * BB + AB;
}

inline void calcZ(long long k, BigInt& Z)
{
    if (k == n - 1) Z = BigInt(0ULL);
    else {
        Z = BigInt(6 * k + 1ULL) * BigInt(2*k + 1ULL) * BigInt(6*k + 5ULL);
        // -1 * (6 * k + 1) * BigInt(2 * k + 1) * (6 * k + 5);
    }
    //(6k+1)(6k+2)(6k+3)(6k+4)(6k+5)(6k+6)
}

inline void mul(const M &lm, const M &rm, M &m, const int tnum, bool neg)
{
    m.X = lm.X * rm.X;
    m.Y = lm.Y * rm.X + lm.Z * rm.Y;
    m.Z = lm.Z * rm.Z;
}

void calcM(ll l, ll r, M &m, int depth = 0, int tn = 0)
{
    if (r == l)
    {
        m.X = BigInt(1ULL);
        m.Y = BigInt(0ULL);
        m.Z = BigInt(1ULL);
    }
    else if (r - l == 1)
    {
        BigInt bk((unsigned long long)l);
        calcX(l, bk, m.X); calcY(bk, m.Y); calcZ(l, m.Z);
    }
    else
    {
        ll mid = (l + r) / 2;
        if(r - l != 2)
            mid += mid % 2;
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
        mul(lm, rm, m, tn, r-l == 2);
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
    int tnum = thread::hardware_concurrency();
    if (argc > 1)
        tnum = atoi(argv[1]);
    cerr << N << endl
         << tnum << endl;

    reset_stopwatch();
    M m;
    calcM(0, n, m,0, 0);
    cerr << "calcM:" << get_elapsed_time()/1000 << "sec" << endl;
    reset_stopwatch();

    BigFloat of = BigFloat(10005);
    of = invsqrt(of, N*4);
    BigFloat ans = BigFloat(BigInt(4270934400ULL) * m.X) * of * BigFloat(m.Y).reciprocal(N * 4);
    cerr << "calcP:" << get_elapsed_time()/1000 << "sec" << endl;
    cout << ans.toDouble() << endl;
    // ans.print();

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