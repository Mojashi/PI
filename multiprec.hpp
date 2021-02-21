#pragma once
#include <vector>
#include <complex>
using std::vector;
using cmplx = std::complex<double>;

const int BASE_E = 13;
const int BASE = 1 << BASE_E;
const int BASE_MASK = BASE - 1;

int getFSize(int n);
void fft(vector<cmplx>& A);
void ifft(vector<cmplx>& ar);
void convolve(const vector<long long>& _A, const vector<long long>& _B, vector<long long>& C);