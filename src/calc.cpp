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
const double INSTR_ERR = 0.5;
const double INSTR_ERR_L = 0.5;

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

double indirectError(double Q, double L, double dL, double meanL,
                     double dMeanL) {
  double relL = dL / L;
  double relMean = dMeanL / meanL;
  return Q * std::sqrt(relL * relL + relMean * relMean);
}

int main() {
  std::ifstream file("data.csv");

  if (!file.is_open()) {
    std::cerr << "Failed to open data.csv\n";
    return 1;
  }

  std::vector<double> lx, ly;
  std::string line;

  std::getline(file, line);

  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    std::stringstream ss(line);
    std::string xStr, yStr;

    if (std::getline(ss, xStr, ',') && std::getline(ss, yStr, ',')) {
      lx.push_back(std::stod(xStr));
      ly.push_back(std::stod(yStr));
    }
  }

  if (lx.empty() || lx.size() != ly.size()) {
    std::cerr << "Invalid data\n";
    return 1;
  }

  double meanX = mean(lx);
  double meanY = mean(ly);

  double randErrX = randomError(lx, meanX, K_STUDENT);
  double randErrY = randomError(ly, meanY, K_STUDENT);

  double fullErrX = fullError(randErrX, INSTR_ERR);
  double fullErrY = fullError(randErrY, INSTR_ERR);

  double A = L / meanX;
  double B = L / meanY;

  double errA = indirectError(A, L, INSTR_ERR_L, meanX, fullErrX);
  double errB = indirectError(B, L, INSTR_ERR_L, meanY, fullErrY);

  std::cout << std::fixed << std::setprecision(4);
  std::cout << "Measurements: " << lx.size() << "\n\n";

  std::cout << "lx (projection on X)\n";
  std::cout << "mean lx      = " << meanX << " mm\n";
  std::cout << "random error = " << randErrX << " mm\n";
  std::cout << "total error  = " << fullErrX << " mm\n\n";

  std::cout << "ly (projection on Y)\n";
  std::cout << "mean ly      = " << meanY << " mm\n";
  std::cout << "random error = " << randErrY << " mm\n";
  std::cout << "total error  = " << fullErrY << " mm\n\n";

  std::cout << "L = " << L << " +- " << INSTR_ERR_L << " mm\n\n";

  std::cout << "A = L / mean lx = " << A << " +- " << errA << "\n";
  std::cout << "B = L / mean ly = " << B << " +- " << errB << "\n";

  return 0;
}