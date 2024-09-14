#include "DFT.h"

namespace Simulation {
	std::complex<double> CreateCoefficient(const std::vector<Sample>& samples, int rank, bool Axis) {

		const double Pi = 3.141592653589793238462;
		int N = samples.size();
		double Omega = 2. * Pi * rank / double(N);

		std::complex<double> c(0.0, 0.0);

		double Xm = double(!Axis);
		double Ym = double(Axis);

		for (int i = 0; i < N; i++) {
			double n = double(i);
			c.real(c.real() + (Xm*samples[i].Position[Axis]*cos(n * Omega)));
			c.imag(c.imag() + (Ym*samples[i].Position[Axis]*sin(n * Omega) * -1.0));
		}
		return c;
	}

	void CreateDFT(std::vector<std::complex<double>>& Coefficients, int Ranks, const std::vector<Sample>& Samples, bool Axis)
	{
		Coefficients.resize(2*Ranks);
		for (int i = 0; i < Ranks; i++) {
			double R = i + 1;
			Coefficients.push_back(CreateCoefficient(Samples, double(R),Axis));
			if (i > 0) {
				Coefficients.push_back(CreateCoefficient(Samples, double(-R),Axis));
			}
		}
	}
}