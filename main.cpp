#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

const double L = 67.0;
const double K_STUDENT = 2.0;
const double INSTR_ERR = 1.0;

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

double fullError(double randErr, double instrErr) {
  return std::sqrt(randErr * randErr + instrErr * instrErr);
}

int main() {
  std::ifstream file("data.csv");

  if (!file.is_open()) {
    std::cerr << "Failed to open file\n";
    return 1;
  }

  std::vector<double> xs;
  std::vector<double> ys;

  std::string line;

  std::getline(file, line);

  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string xStr, yStr;

    if (std::getline(ss, xStr, ',') && std::getline(ss, yStr, ',')) {
      xs.push_back(std::stod(xStr));
      ys.push_back(std::stod(yStr));
    }
  }

  if (xs.empty() || ys.empty() || xs.size() != ys.size()) {
    std::cerr << "Invalid data\n";
    return 1;
  }

  double meanX = mean(xs);
  double meanY = mean(ys);

  double randErrX = randomError(xs, meanX, K_STUDENT);
  double randErrY = randomError(ys, meanY, K_STUDENT);

  double fullErrX = fullError(randErrX, INSTR_ERR);
  double fullErrY = fullError(randErrY, INSTR_ERR);

  double A = L / meanX;
  double B = L / meanY;

  double errA = A * (fullErrX / meanX);
  double errB = B * (fullErrY / meanY);

  std::cout << std::fixed << std::setprecision(4);
  std::cout << "Measurements: " << xs.size() << "\n\n";

  std::cout << "lx (projection on X)\n";
  std::cout << "mean lx      = " << meanX << " mm\n";
  std::cout << "random error = " << randErrX << " mm\n";
  std::cout << "total error  = " << fullErrX << " mm\n\n";

  std::cout << "ly (projection on Y)\n";
  std::cout << "mean ly      = " << meanY << " mm\n";
  std::cout << "random error = " << randErrY << " mm\n";
  std::cout << "total error  = " << fullErrY << " mm\n\n";

  std::cout << "Ratios\n";
  std::cout << "L = " << L << " mm\n";
  std::cout << "A = L / mean lx = " << A << " +- " << errA << "\n";
  std::cout << "B = L / mean ly = " << B << " +- " << errB << "\n";

  return 0;
}