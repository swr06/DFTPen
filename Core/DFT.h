#include <iostream>
#include <complex>
#include <vector>

#include "Sample.h"

namespace Simulation {

	void CreateDFT(std::vector<std::complex<double>>& Coefficients, int Ranks, const std::vector<Sample>& Samples, bool Axis);
}