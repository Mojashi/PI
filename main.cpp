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

#define SAVE_DEPTH 2

using namespace std;
namespace mp = boost::multiprecision;

const int N = 100000000;
const int n = N / 14;

using BigFloat = mp::number<mp::gmp_float<N>>;

using BigInt = mp::mpz_int;

const BigInt A = 13591409, B = 545140134, C = 640320;
const BigInt CT = C * C * C;

BigInt fac(int k)
{
    BigInt ret = 1;
    for (int i = 1; k >= i; i++)
    {
        ret *= i;
    }
    return ret;
}

BigFloat P(int k)
{
    BigInt si = (k % 2 ? -1 : 1) * fac(6 * k) * (A + B * k);
    BigInt bo = fac(3 * k) * mp::pow(fac(k), 3) * pow(C, 3 * k);

    return BigFloat(si) / BigFloat(bo);
}

BigFloat AP(long long int k)
{
    static BigInt si = 0;
    static BigInt bo = 0;
    static BigInt ABK = A;
    if (k == 0)
    {
        si = A;
        bo = 1;
    }
    else
    {
        bo *= k * k * k * (CT * ABK);
        ABK += B;
        si *= -24 * (6 * k - 5) * (2 * k - 1) * (6 * k - 1) * ABK;
    }

    return BigFloat(si) / BigFloat(bo);
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

inline void mul(const M &lm, const M &rm, M &m)
{
    m.X = lm.X * rm.X;
    m.Y = lm.Z * rm.Y + lm.Y * rm.X;
    m.Z = lm.Z * rm.Z;
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
        mul(lm, rm, m);
    }
    if(depth == SAVE_DEPTH){
        ofstream ofs("saves/"+to_string(N)+"_"+to_string(l)+"_"+to_string(r) + ".txt");
        ofs << m.X.str() << "\n" << m.Y.str()  << "\n" << m.Z.str() << "\n";
        ofs.close();
        printf("saved:%d-%d\n", l,r);
    }
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

// int main (int argc, char* argv[]){
//     vector<int> ketas;

//     for(int i = 0; 100000  > i; i++){
//         M a = calcM(i,i + 1);
//         cout << a.X.str().length() <<" "<< a.Y.str().length()<< " " << a.Z.str().length() << endl;
//     }
// }