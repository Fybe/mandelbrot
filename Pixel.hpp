#ifndef PIXEL_HPP
#define PIXEL_HPP

struct Pixel
{
	Pixel(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0)
		: red(red), green(green), blue(blue)
	{}

	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

#endif
