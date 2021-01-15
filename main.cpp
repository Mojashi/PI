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

#define SAVE_DEPTH 2

using namespace std;
namespace mp = boost::multiprecision;

const int N = 100000000;
const int n = N / 14;

using BigFloat = mp::number<mp::gmp_float<N>>;

using BigInt = mp::mpz_int;

const BigInt A = 13591409, B = 545140134, C = 640320;
const BigInt CT = C * C * C;

inline void seperate(const mpz_t& x, mpz_t& hx, mpz_t& lx, int bitlen){
    mpz_tdiv_q_2exp(hx, x, bitlen);
    mpz_tdiv_r_2exp(lx, x, bitlen);
}

void multiply(const mpz_t& x, const mpz_t& y, mpz_t& res, int tnum){
    int xk = x->_mp_size;
    int yk = y->_mp_size;

    int bitlen = max(xk,yk) * GMP_LIMB_BITS / 2;
    
    mpz_t hx,lx, hy, ly;
    mpz_init(hx);mpz_init(lx);mpz_init(hy);mpz_init(ly);
    seperate(x, hx, lx, bitlen);
    seperate(y, hy, ly, bitlen);
    
    mpz_t z2,z1;

    mpz_t by;
    auto by_f = std::thread([&] {
        mpz_init(by);   
        mpz_init(z1);
        mpz_sub(z1, hx, lx);
        mpz_sub(by, hy, ly);
        mpz_mul(by, z1, by);
    });
    auto z2_t = std::thread([&]{
    mpz_init(z2);
    mpz_mul(z2, hx, hy);
    mpz_mul_2exp(z2, z2, bitlen*2);
    });

    z2_t.join();
    mpz_mul(res, lx, ly);

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

void mul_wrap(const BigInt& a, const BigInt& b, BigInt& res, int tnum){
    if(tnum >= 2){
        mpz_t buf;
        mpz_init(buf);

        multiply(a.backend().data(), b.backend().data(), buf, tnum);
        res.assign(buf);
        mpz_clear(buf);
    }
    else {
        res = a * b;
    }
}

class M
{
public:
    BigInt X, Y, Z;
};

const BigInt CT24 = C * C * C / 24;
inline BigInt calcX(long long k)
{
    if (k == 0)
        return 1;
    return k * k * CT24 * k;
}

inline BigInt calcY(long long k)
{
    return A + B * k;
}

inline BigInt calcZ(long long k)
{
    if (k == n - 1)
        return 0;
    return -1 * (6 * k + 1) * BigInt(2 * k + 1) * (6 * k + 5);
    //(6k+1)(6k+2)(6k+3)(6k+4)(6k+5)(6k+6)
}

inline void mul(const M &lm, const M &rm, M &m, const int tnum)
{
    if(tnum == 0){
        m.X = lm.X * rm.X;
        m.Y = lm.Z * rm.Y + lm.Y * rm.X;
        m.Z = lm.Z * rm.Z;
    }
    else if(tnum == 1){
        auto tY = thread([&]{m.Y = lm.Z * rm.Y + lm.Y * rm.X;});
        m.X = lm.X * rm.X;
        m.Z = lm.Z * rm.Z;
        tY.join();
    }
    else if(tnum == 2){
        auto tX = thread([&]{m.X = lm.X * rm.X;});
        auto tY = thread([&]{m.Y = lm.Z * rm.Y + lm.Y * rm.X;});
        m.Z = lm.Z * rm.Z;
        tX.join();tY.join();
    }
    else if (tnum >= 3)
    {
        auto tX = thread([&] { mul_wrap(lm.X, rm.X, m.X, min(2, tnum -3)); });
        BigInt Y2;
        auto tY1 = thread([&] { mul_wrap(lm.Z, rm.Y, m.Y, min(2, tnum - 3 - 2)); });
        auto tY2 = thread([&] { mul_wrap(lm.Y, rm.X, Y2, min(2, tnum  - 3 - 4)); });
        mul_wrap(lm.Z, rm.Z, m.Z, min(2, tnum - 3 - 6));
        tX.join();
        tY1.join();
        tY2.join();
        m.Y += Y2;
    }
}

void calcM(int l, int r, M &m, int depth = 0, int tn = 0)
{
    if (r == l)
    {
        m = {1, 0, 1};
    }
    else if (r - l == 1)
    {
        m = {calcX(l), calcY(l), calcZ(l)};
    }
    else
    {
        int mid = (l + r) / 2;
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
    // if(depth == SAVE_DEPTH){
    //     ofstream ofs("saves/"+to_string(N)+"_"+to_string(l)+"_"+to_string(r) + ".txt");
    //     ofs << m.X.str() << "\n" << m.Y.str()  << "\n" << m.Z.str() << "\n";
    //     ofs.close();
    //     printf("saved:%d-%d\n", l,r);
    // }
}

int main(int argc, char *argv[])
{
    BigFloat p;

    int tnum = thread::hardware_concurrency();
    if (argc > 1)
        tnum = atoi(argv[1]);
    cerr << N << endl
         << tnum << endl;
    M m;
    calcM(0, n, m,0, tnum - 1);
    cerr << "M" << endl;
    p = (sqrt(BigFloat(CT)) / 12 / m.Y) * m.X;
    cerr << "T" << endl;
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