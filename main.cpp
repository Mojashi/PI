#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <iostream>
#include <boost/multiprecision/gmp.hpp>
#include <thread>
#include <vector>
#include <future>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <gmp.h>
#include <chrono>

#define SAVE_DEPTH 2

using namespace std;
namespace mp = boost::multiprecision;
using ll = long long;
const ll N = 100000000;
const ll n = N / 14;

using BigInt = mp::mpz_int;
using BigFloat = mp::number<mp::gmp_float<N>>;

const ll A = 13591409, B = 545140134, C = 640320;
const ll CT = C * C * C;

inline void seperate(const mpz_t& x, mpz_t& hx, mpz_t& lx, int bitlen){
    mpz_tdiv_q_2exp(hx, x, bitlen);
    mpz_tdiv_r_2exp(lx, x, bitlen);
}

void multiply(const mpz_t& x, const mpz_t& y, mpz_t& res, int tnum = 0){
    if(tnum < 2){
        mpz_mul(res, x, y);
        return;
    }

    int xk = x->_mp_size;
    int yk = y->_mp_size;

    int bitlen = max(xk,yk) * GMP_LIMB_BITS / 2;
    
    mpz_t hx,lx, hy, ly;
    mpz_init(hx);mpz_init(lx);mpz_init(hy);mpz_init(ly);
    seperate(x, hx, lx, bitlen);
    seperate(y, hy, ly, bitlen);
    
    mpz_t z2,z1,z0;

    mpz_t by;
    tnum -= 2;

    auto by_f = std::thread([&] {
        mpz_init(by);   
        mpz_init(z1);
        mpz_sub(z1, hx, lx);
        mpz_sub(by, hy, ly);
        multiply(z1, by, by, tnum/3);
        // mpz_mul(by, z1, by);

    });
    auto z2_t = std::thread([&]{
        mpz_init(z2);
        // mpz_mul(z2, hx, hy);
        multiply(hx,hy, z2, tnum/3);
        mpz_mul_2exp(z2, z2, bitlen*2);
    });

    multiply(lx, ly, res, tnum-(tnum/3)*2);
    // mpz_mul(res, lx, ly);

    z2_t.join();
    by_f.join();
    mpz_add(z1, z2, res);
    mpz_sub(z1, z1, by);
    mpz_mul_2exp(z1, z1, bitlen);

    mpz_add(res, res, z1);
    mpz_add(res, res, z2);

    mpz_clear(z2);
    mpz_clear(z1);
    mpz_clear(by);
    mpz_clear(hx);
    mpz_clear(lx);
    mpz_clear(hy);
    mpz_clear(ly);
}   

// void mul_wrap(const mpz_t& a, const BigInt& b, BigInt& res, int tnum){
//     if(tnum >= 2){
//         mpz_t buf;
//         mpz_init(buf);

//         multiply(a.backend().data(), b.backend().data(), buf, tnum);
//         res.assign(buf);
//         mpz_clear(buf);
//     }
//     else {
//         res = a * b;
//     }
// }

class M
{
public:
    mpz_t X, Y, Z;
    M(){mpz_init(X);mpz_init(Y);mpz_init(Z);}
    ~M(){mpz_clear(X);mpz_clear(Y);mpz_clear(Z);}
};

const ll CT24 = C * C * C / 24;
inline void calcX(long long k, mpz_t X)
{
    if (k == 0) mpz_set_si(X, 1);
    else {
        mpz_set_si(X, k);
        mpz_mul_si(X,X,k);
        mpz_mul_si(X,X,k);
        mpz_mul_si(X,X,CT24);
    }
}

inline void calcY(long long k, mpz_t Y)
{
    mpz_set_si(Y, B);
    mpz_mul_si(Y, Y, k);
    mpz_add_ui(Y, Y, A);
}

inline void calcZ(long long k, mpz_t Z)
{
    if (k == n - 1) mpz_set_si(Z, 0);
    else {
        mpz_set_si(Z, 6 * k + 1);
        mpz_mul_si(Z, Z, 2*k + 1);
        mpz_mul_si(Z, Z, -(6*k + 5));
        // -1 * (6 * k + 1) * BigInt(2 * k + 1) * (6 * k + 5);
    }
    //(6k+1)(6k+2)(6k+3)(6k+4)(6k+5)(6k+6)
}

inline void mul(const M &lm, const M &rm, M &m, const int tnum)
{
    if(tnum == 0){
        multiply(lm.X, rm.X, m.X);
        multiply(lm.Z, rm.Z, m.Z);
        mpz_t buf;
        mpz_init(buf);
        multiply(lm.Y, rm.X, m.Y);
        multiply(lm.Z, rm.Y, buf);
        mpz_add(m.Y, m.Y, buf);
        mpz_clear(buf);
    }
    else if(tnum == 1){
        auto tY = thread([&]{
            mpz_t buf;
            mpz_init(buf);
            multiply(lm.Y, rm.X, m.Y);
            multiply(lm.Z, rm.Y, buf);
            mpz_add(m.Y, m.Y, buf);
            mpz_clear(buf);
        });
        multiply(lm.X, rm.X, m.X);
        multiply(lm.Z, rm.Z, m.Z);
        tY.join();
    }
    else if(tnum == 2){
        auto tX = thread([&]{multiply(lm.X, rm.X, m.X);});
        auto tY = thread([&]{
            mpz_t buf;
            mpz_init(buf);
            multiply(lm.Y, rm.X, m.Y);
            multiply(lm.Z, rm.Y, buf);
            mpz_add(m.Y, m.Y, buf);
            mpz_clear(buf);
        });
        multiply(lm.Z, rm.Z, m.Z);
        tX.join();tY.join();
    }
    else if (tnum >= 3)
    {
        int rtnum = tnum-3;
        auto tX = thread([&] { multiply(lm.X, rm.X, m.X, min(2, rtnum/4)); });
        mpz_t Y2;
        mpz_init(Y2);
        auto tY1 = thread([&] { multiply(lm.Z, rm.Y, m.Y, min(2, rtnum/4)); });
        auto tY2 = thread([&] { multiply(lm.Y, rm.X, Y2, min(2, rtnum/4)); });
        multiply(lm.Z, rm.Z, m.Z, min(2, rtnum - (rtnum/4)*3));
        tY1.join();
        tY2.join();
        mpz_add(m.Y, m.Y, Y2);
        mpz_clear(Y2);
        tX.join();
    }
}

void calcM(ll l, ll r, M &m, int depth = 0, int tn = 0)
{
    if (r == l)
    {
        mpz_set_si(m.X, 1);
        mpz_set_si(m.Y, 0);
        mpz_set_si(m.Z, 1);
    }
    else if (r - l == 1)
    {
        calcX(l, m.X); calcY(l, m.Y); calcZ(l, m.Z);
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
        // if(tn != -1) cerr << tn << "end" << endl;
        // if(tn != -1){
        //     BigInt d = mp::gcd(lm.Z, rm.X);
        //     lm.Z = lm.Z/d;
        //     rm.X = rm.X/d;
        // }
        mul(lm, rm, m, tn);
    }

    // if(depth == 3){
    //     cout << l << " " << r << ":" << < m.Z.backend().data()->_mp_size << endl;
    // }
    // if(depth == SAVE_DEPTH){
    //     ofstream ofs("saves/"+to_string(N)+"_"+to_string(l)+"_"+to_string(r) + ".txt");
    //     ofs << m.X.str() << "\n" << m.Y.str()  << "\n" << m.Z.str() << "\n";
    //     ofs.close();
    //     printf("saved:%d-%d\n", l,r);
    // }
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
    BigFloat p;

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
    p = (mp::sqrt(BigFloat(CT))* mp::mpz_int(m.X)) / (12 * mp::mpz_int(m.Y));
    cerr << "calcP:" << get_elapsed_time()/1000 << "sec" << endl;
    cout << setprecision(N) << p << endl;

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