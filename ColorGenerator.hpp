#ifndef COLORGENERATOR_HPP
#define COLORGENERATOR_HPP
#include "ColorScheme.hpp"
#include "Pixel.hpp"


class ColorGenerator
{
public:
	virtual Pixel ToPixel(double real, double imag, uint64_t iteration) const = 0;
};


class ColorGradient
	: public ColorGenerator
{
public:
	ColorGradient(ColorScheme const *colorScheme);
	~ColorGradient();

	Pixel ToPixel(double real, double imag, uint64_t iteration) const override;

private:
	ColorScheme *colorScheme;
};

class DiscreteColorGradient
	: public ColorGenerator
{
public:
	DiscreteColorGradient(ColorScheme const *colorScheme, uint32_t paletteSize);
	~DiscreteColorGradient();

	Pixel ToPixel(double real, double imag, uint64_t iteration) const override;

private:
	uint32_t paletteSize;
	Pixel *colors;
};

#endif
