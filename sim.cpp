#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

const double L = 67.0;
const double K_STUDENT = 2.0;
const double PI = std::acos(-1.0);

double mean(const std::vector<double> &v) {
  return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

double randomError(const std::vector<double> &v, double m, double k) {
  double sumSq = 0.0;

  for (double val : v)
    sumSq += (val - m) * (val - m);

  size_t n = v.size();

  return k * std::sqrt(sumSq / (static_cast<double>(n) * (n - 1)));
}

void simulate(size_t n, std::mt19937 &gen) {
  std::uniform_real_distribution<double> angleDist(0.0, 2.0 * PI);

  std::vector<double> lx, ly;
  lx.reserve(n);
  ly.reserve(n);

  for (size_t i = 0; i < n; ++i) {
    double theta = angleDist(gen);
    lx.push_back(L * std::abs(std::cos(theta)));
    ly.push_back(L * std::abs(std::sin(theta)));
  }

  double meanX = mean(lx);
  double meanY = mean(ly);

  double randErrX = randomError(lx, meanX, K_STUDENT);
  double randErrY = randomError(ly, meanY, K_STUDENT);

  double A = L / meanX;
  double B = L / meanY;

  std::cout << std::fixed << std::setprecision(4);
  std::cout << "Numerical simulation: " << n << " ===\n\n";

  std::cout << "lx (projection on X)\n";
  std::cout << "mean lx      = " << meanX << " mm\n";
  std::cout << "random error = " << randErrX << " mm\n\n";

  std::cout << "ly (projection on Y)\n";
  std::cout << "mean ly      = " << meanY << " mm\n";
  std::cout << "random error = " << randErrY << " mm\n\n";

  std::cout << "A = L / mean lx = " << A << "\n";
  std::cout << "B = L / mean ly = " << B << "\n";
  std::cout << "Theory: pi/2   = " << PI / 2.0 << "\n\n";
}

int main() {
  std::random_device rd;
  std::mt19937 gen(rd());

  const size_t trials[] = {30, 50, 100, 500, 1000, 5000, 10000};

  for (size_t n : trials)
    simulate(n, gen);

  return 0;
}