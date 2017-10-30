#ifndef FRACTALGENERATOR_HPP
#define FRACTALGENERATOR_HPP
#include <mutex>
#include "PixelBuffer.hpp"
#include "ColorGenerator.hpp"


class FractalGenerator
{
public:
	FractalGenerator(PixelBuffer *renderTarget, ColorGenerator const *colorGenerator);
	~FractalGenerator();

	void Render();

	void SetRenderTarget(PixelBuffer *renderTarget);
	void SetPoint(double c_real, double c_imag);
	void SetMagnitude(double magnitude);
	void SetIterations(uint64_t iterations);

	double GetReal() const;
	double GetImag() const;
	double GetMagnitude() const;
	uint64_t GetIterations() const;
	PixelBuffer const &GetPixelBuffer() const;

	float GetProgress();

private:
	PixelBuffer *renderTarget;
	ColorGenerator *colorGenerator;
	double c_real;
	double c_imag;
	double magnitude;
	uint64_t iterations;

	float progress;
	std::mutex mut;

	static double map(double f0, double f1, double t0, double t1, double x);
};

#endif
