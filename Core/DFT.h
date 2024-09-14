#include <iostream>
#include <complex>
#include <vector>

#include "Sample.h"

namespace Simulation {

	void CreateDFT(std::vector<std::complex<double>>& Coefficients, int Ranks, const std::vector<Sample>& Samples, bool Axis);

	
	struct DFT2D {

		std::vector<std::complex<double>> DFTX;
		std::vector<std::complex<double>> DFTY;

		void Reserve(int N) {
			DFTX.reserve(N);
			DFTY.reserve(N);
		}

		void Create(const std::vector<Sample>& Samples) {
			CreateDFT(DFTX, Samples.size(), Samples, 0);
			CreateDFT(DFTY, Samples.size(), Samples, 1);
		}

		void InverseDFT(std::vector<Sample>& Output) {

		}

	};
}