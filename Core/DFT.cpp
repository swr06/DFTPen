#include "DFT.h"

namespace Simulation {

	void DFT2D::CreateTransform(const std::vector<Sample>& Samples, int Ranking)
	{
		CurrentRank = Ranking;

		{
			DFTX.clear();
			DFTX.resize(CurrentRank);

			int N = Samples.size();

			for (int k = 0; k < CurrentRank; k++) {
				double Omega = (2.0 * 3.14159265359 * double(k)) / (double(N));

				std::complex<double> C(0, 0);

				for (int i = 0; i < N; i++) {

					double Angle = Omega * double(i);
					double Xi = Samples[i].Position[0];

					C += std::complex<double>(Xi * cos(Angle), -1.0 * Xi * sin(Angle));
				}

				DFTX[k] = C;
			}
		}

		{
			DFTY.clear();
			DFTY.resize(CurrentRank);

			int N = Samples.size();

			for (int k = 0; k < CurrentRank; k++) {
				double Omega = (2.0 * 3.14159265359 * double(k)) / (double(N));

				std::complex<double> C(0, 0);

				for (int i = 0; i < N; i++) {

					double Angle = Omega * double(i);
					double Xi = Samples[i].Position[1];

					C += std::complex<double>(Xi * cos(Angle), -1.0 * Xi * sin(Angle));
				}

				DFTY[k] = C;
			}
		}
	}

	void DFT2D::InverseTransform(std::vector<Sample>& OutputSamples, int SampCount)
	{
		double OneOverN = 1. / double(SampCount);
		double NormalizationFactor = 1. / double(SampCount);
		double ScalingFactor = 1.0f;
		OutputSamples.resize(SampCount);

		for (int i = 0; i < SampCount; i++) {
			std::complex<double> C(0, 0);
			for (int k = 0; k < CurrentRank; k++) {
				double Omega = (2.0 * 3.14159265359 * double(k) * OneOverN);
				double Angle = Omega * double(i);
				C += std::complex<double>(cos(Angle), sin(Angle)) * DFTX[k];
			}

			C *= NormalizationFactor;
			OutputSamples[i].Position.x = C.real() * ScalingFactor;
		}

		for (int i = 0; i < SampCount; i++) {
			std::complex<double> C(0, 0);
			for (int k = 0; k < CurrentRank; k++) {
				double Omega = (2.0 * 3.14159265359 * double(k) * OneOverN);
				double Angle = Omega * double(i);
				C += std::complex<double>(cos(Angle), sin(Angle)) * DFTY[k];
			}

			C *= NormalizationFactor;
			OutputSamples[i].Position.y = C.real() * ScalingFactor;
			OutputSamples[i].Active = true;
		}
	}

}