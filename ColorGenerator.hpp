#ifndef COLORGENERATOR_HPP
#define COLORGENERATOR_HPP
#include "ColorScheme.hpp"
#include "Pixel.hpp"


class ColorGenerator
{
public:
	ColorGenerator() {}
	virtual ~ColorGenerator() {}
	virtual Pixel ToPixel(double real, double imag, uint64_t iteration) const = 0;
	virtual ColorGenerator *clone() const = 0;
};


class ColorGradient
	: public ColorGenerator
{
public:
	ColorGradient(ColorScheme const *colorScheme);
	ColorGradient(ColorGradient const &other);
	virtual ~ColorGradient();

	Pixel ToPixel(double real, double imag, uint64_t iteration) const override;
	ColorGenerator *clone() const override;

private:
	ColorScheme *colorScheme;
};

class DiscreteColorGradient
	: public ColorGenerator
{
public:
	DiscreteColorGradient(ColorScheme const *colorScheme, uint32_t paletteSize);
	DiscreteColorGradient(DiscreteColorGradient const &other);
	virtual ~DiscreteColorGradient();

	Pixel ToPixel(double real, double imag, uint64_t iteration) const override;
	ColorGenerator *clone() const override;

private:
	uint32_t paletteSize;
	Pixel *colors;
};

#endif
