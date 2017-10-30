#ifndef COLORSCHEME_HPP
#define COLORSCHEME_HPP
#include "Pixel.hpp"

class ColorScheme
{
public:
	virtual Pixel GetColor(float value) const = 0;
};

class MonotoneInterpolationColorScheme
	: public ColorScheme
{
}

#endif
