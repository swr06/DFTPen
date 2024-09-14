#include "DFT.h"

namespace Simulation {

	void GenerateDFT(const std::vector<Sample>& Samples, std::vector<std::complex<double>>& DFT, int Ranks, int Idx) {
		DFT.clear();
		DFT.resize(Ranks);

		int N = Samples.size();

		for (int k = 0; k < Ranks; k++) {
			double Omega = (2.0 * 3.14159265359 * double(k)) / (double(N));

			std::complex<double> C(0, 0);

			for (int i = 0; i < N; i++) {

				double Angle = Omega * double(i);
				double Xi = Samples[i].Position[Idx];
				C += std::complex<double>(Xi * cos(Angle), -Xi * sin(Angle));
			}

			DFT[k] = C;
		}
	}

	void DFT2D::CreateTransform(const std::vector<Sample>& Samples, int Ranking)
	{
		CurrentRank = Ranking;
		GenerateDFT(Samples, DFTX, Ranking, 0);
		GenerateDFT(Samples, DFTY, Ranking, 1);
	}

	void DFT2D::InverseTransform(std::vector<Sample>& OutputSamples, int SampCount)
	{
		double OneOverN = 1. / double(SampCount);
		double NormalizationFactor = 1. / double(SampCount);
		double ScalingFactor = double(SampCount) / double(CurrentRank);
		OutputSamples.resize(SampCount);

		for (int i = 0; i < SampCount; i++) {
			std::complex<double> C(0, 0);
			for (int k = 0; k < CurrentRank; k++) {
				double Omega = (2.0 * 3.14159265359 * double(k) * OneOverN);
				double Angle = Omega * double(i);
				C += std::complex<double>(cos(Angle), sin(Angle)) * DFTX[k];
			}

			C *= NormalizationFactor;
			OutputSamples[i].Position.x = C.real();
		}

		for (int i = 0; i < SampCount; i++) {
			std::complex<double> C(0, 0);
			for (int k = 0; k < CurrentRank; k++) {
				double Omega = (2.0 * 3.14159265359 * double(k) * OneOverN);
				double Angle = Omega * double(i);
				C += std::complex<double>(cos(Angle), sin(Angle)) * DFTY[k];
			}

			C *= NormalizationFactor;
			OutputSamples[i].Position.y = C.real();
			OutputSamples[i].Active = true;
		}
	}

}