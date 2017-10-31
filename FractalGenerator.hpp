#ifndef FRACTALGENERATOR_HPP
#define FRACTALGENERATOR_HPP
#include <mutex>
#include <cmath>
#include <list>
#include <thread>
#include "mpreal.h"
#include "PixelBuffer.hpp"
#include "ColorGenerator.hpp"

using namespace mpfr;
typedef mpreal Float;

class FractalGenerator
{
public:
	FractalGenerator(PixelBuffer *renderTarget, ColorGenerator const *colorGenerator);
	~FractalGenerator();

	void Render(size_t threads = 1);

	void SetRenderTarget(PixelBuffer *renderTarget);
	void SetPoint(Float c_real, Float c_imag);
	void SetMagnitude(Float magnitude);
	void SetIterations(uint64_t iterations);

	Float GetReal() const;
	Float GetImag() const;
	Float GetMagnitude() const;
	uint64_t GetIterations() const;
	PixelBuffer const &GetPixelBuffer() const;

	float GetProgress();

private:
	PixelBuffer *renderTarget;
	ColorGenerator *colorGenerator;
	Float c_real;
	Float c_imag;
	Float magnitude;
	uint64_t iterations;

	float progress;
	std::mutex mut;

	void RenderPartially(uint32_t xmin, uint32_t xmax);
	static Float map(Float f0, Float f1, Float t0, Float t1, Float x);
};

#endif
