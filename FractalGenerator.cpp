#include "FractalGenerator.hpp"

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

void FractalGenerator::Render(size_t threads)
{
	std::list<std::thread> threadList;
	for(size_t i = 0; i < threads; i++)
	{
		uint32_t xmin = i * renderTarget->GetWidth() / threads;
		uint32_t xmax = (i+1) * renderTarget->GetWidth() / threads;

		threadList.push_back(std::thread(&FractalGenerator::RenderPartially, this, xmin, xmax));
	}

	for(auto &T : threadList)
	{
		T.join();
	}
	//Float real_width = pow(10, -magnitude);
	//Float x_min = c_real - real_width * 0.5;
	//Float x_max = c_real + real_width * 0.5;

	//Float real_height = real_width * renderTarget->GetHeight() / renderTarget->GetWidth();
	//Float y_min = c_imag + real_height * 0.5;
	//Float y_max = c_imag - real_height * 0.5;

	//for(uint32_t x = 0; x < renderTarget->GetWidth(); x++)
	//{
	//	Float re = map(0, renderTarget->GetWidth(), x_min, x_max, x);
	//	for(uint32_t y = 0; y < renderTarget->GetHeight(); y++)
	//	{
	//		Float im = map(0, renderTarget->GetHeight(), y_min, y_max, y);

	//		Float real = re;
	//		Float imag = im;
	//		
	//		Float rr = 0;
	//		Float ii = 0;
	//		Float ri = 0;
	//		// mandelbrot test per pixel
	//		uint64_t i;
	//		for(i = 0; i < iterations; i++)
	//		{
	//			rr = real * real;
	//			ii = imag * imag;
	//			ri = real * imag;

	//			real = rr - ii + re;
	//			imag = 2*ri + im;

	//			if(rr + ii > 4)
	//				break;
	//		}

	//		if(i == iterations && rr + ii <= 4)
	//			renderTarget->SetPixel(x, y, Pixel(0,0,0));
	//		else
	//			renderTarget->SetPixel(x, y, colorGenerator->ToPixel(real.toDouble(), imag.toDouble(), i));

	//		progress = static_cast<float>(x * renderTarget->GetHeight() + y) / (renderTarget->GetWidth() * renderTarget->GetHeight());
	//	}
	//	progress = static_cast<float>((x+1) * renderTarget->GetHeight()) / (renderTarget->GetWidth() * renderTarget->GetHeight());
	//}
	mut.lock();
	progress = 1.f;
	mut.unlock();
}
void FractalGenerator::RenderPartially(uint32_t xmin, uint32_t xmax)
{
	Float real_width = pow(10, -magnitude);
	Float x_min = c_real - real_width * 0.5;
	Float x_max = c_real + real_width * 0.5;

	Float real_height = real_width * renderTarget->GetHeight() / renderTarget->GetWidth();
	Float y_min = c_imag + real_height * 0.5;
	Float y_max = c_imag - real_height * 0.5;

	for(uint32_t x = xmin; x < xmax; x++)
	{
		Float re = map(0, renderTarget->GetWidth(), x_min, x_max, x);
		for(uint32_t y = 0; y < renderTarget->GetHeight(); y++)
		{
			Float im = map(0, renderTarget->GetHeight(), y_min, y_max, y);

			Float real = re;
			Float imag = im;
			
			Float rr = 0;
			Float ii = 0;
			Float ri = 0;
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
				renderTarget->SetPixel(x, y, colorGenerator->ToPixel(real.toDouble(), imag.toDouble(), i));

			mut.lock();
			progress += 1.f / (renderTarget->GetHeight() * renderTarget->GetWidth());
			//progress = static_cast<float>(x * renderTarget->GetHeight() + y) / (renderTarget->GetWidth() * renderTarget->GetHeight());
			mut.unlock();
		}
	}
}

void FractalGenerator::SetRenderTarget(PixelBuffer *renderTarget)
{
	this->renderTarget = renderTarget;
}
void FractalGenerator::SetPoint(Float c_real, Float c_imag)
{
	this->c_real = c_real;
	this->c_imag = c_imag;
}
void FractalGenerator::SetMagnitude(Float magnitude)
{
	this->magnitude = magnitude;
}
void FractalGenerator::SetIterations(uint64_t iterations)
{
	this->iterations = iterations;
}

Float FractalGenerator::GetReal() const
{
	return c_real;
}
Float FractalGenerator::GetImag() const
{
	return c_imag;
}
Float FractalGenerator::GetMagnitude() const
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

Float FractalGenerator::map(Float f0, Float f1, Float t0, Float t1, Float x)
{
	return t0 + (x - f0) / (f1 - f0) * (t1 - t0);
}
