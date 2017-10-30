#ifndef COLORSCHEME_HPP
#define COLORSCHEME_HPP
#include <initializer_list>
#include <cstdint>
#include "Pixel.hpp"
#include "Spline.hpp"

class ColorScheme
{
public:
	ColorScheme() {}
	virtual ~ColorScheme() {}

	virtual Pixel GetColor(float value) const = 0;
	virtual ColorScheme *clone() const = 0;
};

class MonotonicColorScheme
	: public ColorScheme
{
public:
	MonotonicColorScheme();
	MonotonicColorScheme(std::vector<std::pair<float, Pixel>> nodes);
	virtual ~MonotonicColorScheme();

	Pixel GetColor(float value) const override;

	ColorScheme *clone() const override;
	
private:
	HermiteSpline rSpline;
	HermiteSpline gSpline;
	HermiteSpline bSpline;

};

#endif
