#pragma once
#include <vector>
#include <complex>
using std::vector;
using cmplx = std::complex<double>;

const int BASE_E = 12;
const int BASE = 1 << BASE_E;
const int BASE_MASK = BASE - 1;

int getFSize(int n);
void fft(vector<cmplx>& A);
void ifft(vector<cmplx>& ar);
template<typename T>
void convolve(const vector<T>& _A, const vector<T>& _B, vector<T>& C);
