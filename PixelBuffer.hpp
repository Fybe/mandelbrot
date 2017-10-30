#ifndef PIXELBUFFER_HPP
#define PIXELBUFFER_HPP
#include <cstdint>
#include <string>
#include <png++/png.hpp>
#include "Pixel.hpp"


class PixelBuffer
{
public:
	PixelBuffer(uint32_t bufferWidth, uint32_t bufferHeight);
	virtual ~PixelBuffer();

	virtual PixelBuffer *clone() const = 0;

	virtual Pixel const &GetPixel(uint32_t x, uint32_t y) const = 0;
	virtual void SetPixel(uint32_t x, uint32_t y, Pixel const &pixel) = 0;

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	void WriteToFile(std::string filename) const;

private:
	uint32_t bufferWidth;
	uint32_t bufferHeight;
};


class InternalPixelBuffer
	: public PixelBuffer
{
public:
	InternalPixelBuffer(uint32_t bufferWidth, uint32_t bufferHeight);
	InternalPixelBuffer(InternalPixelBuffer const &other);
	virtual ~InternalPixelBuffer();

	PixelBuffer *clone() const override;

	Pixel const &GetPixel(uint32_t x, uint32_t y) const override;
	void SetPixel(uint32_t x, uint32_t y, Pixel const &pixel) override;

private:
	Pixel **buffer;
	
};


class SupersampledPixelBuffer
	: public InternalPixelBuffer
{
public:
	SupersampledPixelBuffer(uint32_t bufferWidth, uint32_t bufferHeight, float samplingFactor);
	SupersampledPixelBuffer(SupersampledPixelBuffer const &other);
	virtual ~SupersampledPixelBuffer();

	PixelBuffer *clone() const override;

	float GetSamplingFactor() const;
	InternalPixelBuffer GetSampledPixelBuffer() const;

private:
	float samplingFactor;
	uint32_t originalBufferWidth;
	uint32_t originalBufferHeight;
};


#endif
