#include "ColorGenerator.hpp"
#include <cmath>

#define ONE_OVER_LOG2 3.32192809488736234787031942948939017

ColorGradient::ColorGradient(ColorScheme const *colorScheme)
	: colorScheme(colorScheme->clone())
{
}
ColorGradient::ColorGradient(ColorGradient const &other)
	: colorScheme(other.colorScheme->clone())
{
}
ColorGradient::~ColorGradient()
{
	delete colorScheme;
}
Pixel ColorGradient::ToPixel(double real, double imag, uint64_t iteration) const
{
	//float size = real * real + imag * imag;
	//float smoothed = std::log(0.5 * std::log(size) * ONE_OVER_LOG2) * ONE_OVER_LOG2;
	//float colorValue = std::fmod(iteration - smoothed, 1);
	return colorScheme->GetColor((iteration % 96) / 95.f);
}
ColorGenerator *ColorGradient::clone() const
{
	return new ColorGradient(*this);
}

DiscreteColorGradient::DiscreteColorGradient(ColorScheme const *colorScheme, uint32_t paletteSize)
	: paletteSize(paletteSize)
{
	colors = new Pixel[paletteSize];
	for(uint32_t i = 0; i < paletteSize; i++)
	{
		colors[i] = colorScheme->GetColor(static_cast<float>(i) / paletteSize);
	}
}
DiscreteColorGradient::DiscreteColorGradient(DiscreteColorGradient const &other)
{
	paletteSize = other.paletteSize;
	colors = new Pixel[paletteSize];
	for(size_t i = 0; i < paletteSize; i++)
		colors[i] = other.colors[i];
}
DiscreteColorGradient::~DiscreteColorGradient()
{
	delete[] colors;
}

Pixel DiscreteColorGradient::ToPixel(double real, double imag, uint64_t iteration) const
{
	float size = real * real + imag * imag;
	float smoothed = std::log(0.5 * std::log(size) / std::log(2)) / std::log(2);
	uint32_t colorValue = static_cast<uint32_t>((iteration + 1 - smoothed) * paletteSize) % paletteSize;
	return colors[colorValue];
}
ColorGenerator *DiscreteColorGradient::clone() const
{
	return new DiscreteColorGradient(*this);
}
