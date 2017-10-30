#include "FractalGenerator.hpp"
#include <cmath>
#include <iostream>
#include <iomanip>

FractalGenerator::FractalGenerator(PixelBuffer *renderTarget, ColorGenerator const *colorGenerator)
	: renderTarget(renderTarget), colorGenerator(colorGenerator->clone())
{
	c_real = -0.5;
	c_imag = 0;
	magnitude = -0.7;
	iterations = 100u;
	progress = 0.f;
}
FractalGenerator::~FractalGenerator()
{}

void FractalGenerator::Render()
{
	double real_width = std::pow(10, -magnitude);
	double x_min = c_real - real_width * 0.5;
	double x_max = c_real + real_width * 0.5;

	double real_height = real_width * renderTarget->GetHeight() / renderTarget->GetWidth();
	double y_min = c_imag + real_height * 0.5;
	double y_max = c_imag - real_height * 0.5;

	for(uint32_t x = 0; x < renderTarget->GetWidth(); x++)
	{
		double re = map(0, renderTarget->GetWidth(), x_min, x_max, x);
		for(uint32_t y = 0; y < renderTarget->GetHeight(); y++)
		{
			double im = map(0, renderTarget->GetHeight(), y_min, y_max, y);

			double real = re;
			double imag = im;
			
			double rr;
			double ii;
			double ri;
			// mandelbrot test per pixel
			uint64_t i;
			for(i = 0; i < iterations; i++)
			{
				rr = real * real;
				ii = imag * imag;
				ri = real * imag;

				real = rr - ii + re;
				imag = 2*ri + im;

				if(rr + ii > 4)
					break;
			}

			if(i == iterations && rr + ii <= 4)
				renderTarget->SetPixel(x, y, Pixel(0,0,0));
			else
				renderTarget->SetPixel(x, y, colorGenerator->ToPixel(real, imag, i));

			progress = static_cast<float>(x * renderTarget->GetWidth() + y) / (renderTarget->GetWidth() * renderTarget->GetHeight());
		}
	}

	progress = 1.f;
}

void FractalGenerator::SetRenderTarget(PixelBuffer *renderTarget)
{
	this->renderTarget = renderTarget;
}
void FractalGenerator::SetPoint(double c_real, double c_imag)
{
	this->c_real = c_real;
	this->c_imag = c_imag;
}
void FractalGenerator::SetMagnitude(double magnitude)
{
	this->magnitude = magnitude;
}
void FractalGenerator::SetIterations(uint64_t iterations)
{
	this->iterations = iterations;
}

double FractalGenerator::GetReal() const
{
	return c_real;
}
double FractalGenerator::GetImag() const
{
	return c_imag;
}
double FractalGenerator::GetMagnitude() const
{
	return magnitude;
}
uint64_t FractalGenerator::GetIterations() const
{
	return iterations;
}
PixelBuffer const &FractalGenerator::GetPixelBuffer() const
{
	return *renderTarget;
}

float FractalGenerator::GetProgress()
{
	mut.lock();
	float p = progress;
	mut.unlock();
	return p;
}

double FractalGenerator::map(double f0, double f1, double t0, double t1, double x)
{
	return t0 + (x - f0) / (f1 - f0) * (t1 - t0);
}
