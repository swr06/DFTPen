#include <iostream>
#include <complex>
#include <vector>

#include "Sample.h"

namespace Simulation {

	class DFT2D {

	public :


		void CreateTransform(const std::vector<Sample>& Samples, int Ranking);
		void InverseTransform(std::vector<Sample>& OutputSamples, int SampCount);

	private :

		std::vector<std::complex<double>> DFTX;
		std::vector<std::complex<double>> DFTY;
		int CurrentRank;
	};
}