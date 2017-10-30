#include <gmpxx.h>
#include <iostream>
#include <list>
#include <cstdint>
#include <vector>
#include <iomanip>
#include <sstream>
#include <thread>
#include "FractalGenerator.hpp"

using namespace std;


void render_helper(ColorGradient *gradient, double mag_from, double mag_to, double time_per_mag, double fps, double real, double imag, size_t frame_min, size_t frame_max, size_t frame_skip);


int main(int argc, char **argv)
{
	cout.precision(4);
	MonotonicColorScheme scheme(std::vector<std::pair<float, Pixel>>{
			std::pair<float, Pixel>(   0.0, Pixel(  0,   7, 100)),
			std::pair<float, Pixel>(  0.16, Pixel( 32, 107, 203)),
			std::pair<float, Pixel>(  0.42, Pixel(237, 255, 255)),
			std::pair<float, Pixel>(0.6425, Pixel(255, 170,   0)),
			std::pair<float, Pixel>(0.8575, Pixel(  0,   2,   0)),
			std::pair<float, Pixel>(   1.0, Pixel(  0,   7,   100))
	});
	ColorGradient gradient(&scheme);

	float mag_from = -1.f;
	float mag_to = 60.f;
	float time_per_mag = 2.5f;
	float fps = 30.f;
	size_t frames = fps * (mag_to - mag_from) * time_per_mag;

	double c_real = -1.985540371654130485531439267191269851811165434636382820704394766801377;
	double c_imag = +0.000000000000000000000000000001565120217211466101983496092509512479178;

	size_t threads = 16;

	std::list<std::thread> T;

	for(size_t i = 0; i < threads; i++)
	{
		T.push_back(thread(&render_helper, &gradient, mag_from, mag_to, time_per_mag, fps, c_real, c_imag, i, frames, threads));
	}

	for(auto &t : T)
		t.join();

	return 0;
}



void render_helper(ColorGradient *gradient, double mag_from, double mag_to, double time_per_mag, double fps, double real, double imag, size_t frame_min, size_t frame_max, size_t frame_skip)
{
	size_t frames = fps * (mag_to - mag_from) * time_per_mag;

	InternalPixelBuffer *buf = new InternalPixelBuffer(1280, 720);
	for(size_t frame = frame_min; frame < frame_max; frame += frame_skip)
	{
		FractalGenerator gen(buf, gradient);

		gen.SetPoint(real, imag);
		double mag = mag_from + static_cast<double>(mag_to-mag_from) * frame / frames;
		gen.SetMagnitude(mag);
		gen.SetIterations(1ul << 8ul);
		auto beg = chrono::high_resolution_clock::now();
		gen.Render();
		auto end = chrono::high_resolution_clock::now();

		stringstream ss;
		ss << "output/image" << setw(6) << setfill('0') << frame << ".png";
		buf->WriteToFile(ss.str());

		ss = stringstream();
		ss << "frame " << frame << " of " << frames << " rendered in " << chrono::duration_cast<chrono::milliseconds>(end-beg).count()/1000.f << "s" << endl;
		cout << ss.str();
		cout.flush();
	}
	delete buf;
}
