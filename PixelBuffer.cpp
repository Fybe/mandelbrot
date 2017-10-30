#include "PixelBuffer.hpp"

// PixelBuffer
//
PixelBuffer::PixelBuffer(uint32_t bufferWidth, uint32_t bufferHeight)
	: bufferWidth(bufferWidth), bufferHeight(bufferHeight)
{}
PixelBuffer::~PixelBuffer()
{}

uint32_t PixelBuffer::GetWidth() const
{
	return bufferWidth;
}
uint32_t PixelBuffer::GetHeight() const
{
	return bufferHeight;
}

void PixelBuffer::WriteToFile(std::string filename) const
{
	png::image<png::rgb_pixel> image(GetWidth(), GetHeight());
	for(uint32_t x = 0; x < GetWidth(); x++)
	{
		for(uint32_t y = 0; y < GetHeight(); y++)
		{
			Pixel const &p = GetPixel(x, y);
			image[y][x] = png::rgb_pixel(p.red, p.green, p.blue);
		}
	}
	
	image.write(filename.c_str());
}


// InternalPixelBuffer
//
InternalPixelBuffer::InternalPixelBuffer(uint32_t bufferWidth, uint32_t bufferHeight)
	: PixelBuffer(bufferWidth, bufferHeight)
{
	buffer = new Pixel*[bufferWidth];

	for(uint32_t x = 0; x < bufferWidth; x++)
	{
		buffer[x] = new Pixel[bufferHeight];
	}
}
InternalPixelBuffer::InternalPixelBuffer(InternalPixelBuffer const &other)
	: PixelBuffer(other.GetWidth(), other.GetHeight())
{
	for(uint32_t x = 0; x < GetWidth(); x++)
	{
		for(uint32_t y = 0; y < GetHeight(); y++)
		{
			buffer[x][y] = other.buffer[x][y];
		}
	}
}
InternalPixelBuffer::~InternalPixelBuffer()
{
	for(uint32_t x = 0; x < this->GetWidth(); x++)
	{
		delete[] buffer[x];
	}

	delete[] buffer;
}

Pixel const &InternalPixelBuffer::GetPixel(uint32_t x, uint32_t y) const
{
	return buffer[x][y];
}
void InternalPixelBuffer::SetPixel(uint32_t x, uint32_t y, Pixel const &pixel)
{
	buffer[x][y] = pixel;
}


// SupersampledPixelBuffer
//
SupersampledPixelBuffer::SupersampledPixelBuffer(uint32_t bufferWidth, uint32_t bufferHeight, float samplingFactor)
	: InternalPixelBuffer(static_cast<uint32_t>(samplingFactor*bufferWidth), static_cast<uint32_t>(samplingFactor*bufferHeight))
	, samplingFactor(samplingFactor)
	, originalBufferWidth(bufferWidth)
	, originalBufferHeight(bufferHeight)
{
}
SupersampledPixelBuffer::~SupersampledPixelBuffer()
{}

float SupersampledPixelBuffer::GetSamplingFactor() const
{
	return samplingFactor;
}
InternalPixelBuffer SupersampledPixelBuffer::GetSampledPixelBuffer() const
{
	InternalPixelBuffer buffer(originalBufferWidth, originalBufferHeight);
	
	// supersampled sections
	for(uint32_t x = 0; x < originalBufferWidth; x++)
	{
		float xs_from = x * samplingFactor;
		float xs_to = (x+1) * samplingFactor;

		for(uint32_t y = 0; y < originalBufferHeight; y++)
		{
			float ys_from = y * samplingFactor;
			float ys_to = (y+1) * samplingFactor;

			float r = 0.f;
			float g = 0.f;
			float b = 0.f;

			for(uint32_t xs = static_cast<uint32_t>(xs_from); xs < xs_to; xs++)
			{
				float x_weight;
				
				if(xs < xs_from)
					x_weight = static_cast<float>(xs_from - xs) / (xs_to - xs_from);
				else if(xs > xs_to)
					x_weight = static_cast<float>(xs - xs_to) / (xs_to - xs_from);
				else
					x_weight = 1.f;

				for(uint32_t ys = static_cast<uint32_t>(ys_from); ys < ys_to; ys++)
				{
					float y_weight;

					if(ys < ys_from)
						y_weight = static_cast<float>(ys_from - ys) / (ys_to - ys_from);
					else if(ys > ys_to)
						y_weight = static_cast<float>(ys - ys_to) / (ys_to - ys_from);
					else
						y_weight = 1.f;

					Pixel const &p = this->GetPixel(xs, ys);
					r += p.red * x_weight * y_weight;
					g += p.green * x_weight * y_weight;
					b += p.blue * x_weight * y_weight;
				}
			}

			Pixel p_sampled(static_cast<uint8_t>(r),
					static_cast<uint8_t>(g),
					static_cast<uint8_t>(b));
			buffer.SetPixel(x, y, p_sampled);
		}
	}

	return buffer;
}
