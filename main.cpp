#include <iostream>
#include <list>
#include <cstdint>
#include <vector>
#include <iomanip>
#include <sstream>
#include <thread>
#include <boost/program_options.hpp>
#include "FractalGenerator.hpp"

using namespace std;
namespace po = boost::program_options;


void render_helper(ColorGradient *gradient, Float mag_from, Float mag_to, Float time_per_mag, Float fps, Float real, Float imag, size_t frame_min, size_t frame_max, size_t frame_skip);


int main(int argc, char **argv)
{
	cout.precision(2);
	cout.setf(iostream::fixed);

	// parse parameter
	po::positional_options_description p;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h",						"displays this help message")
		("precision,p",					po::value<size_t>()->default_value(128),				"precision for floating-point calculation in bits")
		("threads,t",					po::value<size_t>()->default_value(thread::hardware_concurrency()),				"number of threads used for rendering")
		("real",						po::value<string>()->default_value("-0.5"),				"point on real axis")
		("imag",						po::value<string>()->default_value("0"),				"point on imaginary axis")
		("magnitude",					po::value<vector<string>>()->multitoken(),				"zoom level, beginning to ending")
		("speed",						po::value<string>()->default_value("3.0"),				"zoom speed in seconds per magnitude")
		("fps",							po::value<string>()->default_value("30"),				"frames per second for output video")
		("gradient",					po::value<string>(),									"file containing gradient specifications")
		("mode",						po::value<string>()->default_value("single"),			"valid options: single, zoom")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
			options(desc).positional(p).run(), vm);
	vm.notify();

	if(vm.count("help"))
	{
		cout << desc << endl;
		return 0;
	}

	MonotonicColorScheme scheme(std::vector<std::pair<float, Pixel>>{
			std::pair<float, Pixel>(   0.0, Pixel(  0,   7, 100)),
			std::pair<float, Pixel>(  0.16, Pixel( 32, 107, 203)),
			std::pair<float, Pixel>(  0.42, Pixel(237, 255, 255)),
			std::pair<float, Pixel>(0.6425, Pixel(255, 170,   0)),
			std::pair<float, Pixel>(0.8575, Pixel(  0,   2,   0)),
			std::pair<float, Pixel>(   1.0, Pixel(  0,   7,   100))
	});
	ColorGradient gradient(&scheme);

	Float::set_default_prec(vm["precision"].as<size_t>());
	
	Float c_real = vm["real"].as<string>();//"-1.985540371654130485531439267191269851811165434636382820704394766801377";
	Float c_imag = vm["imag"].as<string>();//"+0.000000000000000000000000000001565120217211466101983496092509512479178";

	if(vm["mode"].as<string>() == "zoom")
	{
		Float mag_from = vm["magnitude"].as<vector<string>>()[0];
		Float mag_to = vm["magnitude"].as<vector<string>>()[1];
		Float time_per_mag = vm["speed"].as<string>();
		Float fps = vm["fps"].as<string>();
		size_t frames = (fps * (mag_to - mag_from) * time_per_mag).toULong();

		size_t threads = vm["threads"].as<size_t>();
		std::list<std::thread> T;

		for(size_t i = 0; i < threads; i++)
		{
			T.push_back(thread(&render_helper, &gradient, mag_from, mag_to, time_per_mag, fps, c_real, c_imag, i, frames, threads));
		}

		for(auto &t : T)
			t.join();
	}
	else if(vm["mode"].as<string>() == "single")
	{
		Float mag = vm["magnitude"].as<vector<string>>()[0];

		InternalPixelBuffer *buf = new InternalPixelBuffer(1280, 720);
		FractalGenerator gen(buf, &gradient);
		gen.SetPoint(c_real, c_imag);
		gen.SetMagnitude(mag);
		gen.SetIterations(1ul << 8ul);

		auto beg = chrono::high_resolution_clock::now();
		gen.Render();
		auto end = chrono::high_resolution_clock::now();

		stringstream ss;
		ss << "output/image" << setw(6) << setfill('0') << ".png";
		buf->WriteToFile(ss.str());

		ss = stringstream();
		ss << "frame " << 0 << " of " << 1 << " rendered in " << chrono::duration_cast<chrono::milliseconds>(end-beg).count()/1000.f << "s" << endl;
		cout << ss.str();
		cout.flush();
	}

	return 0;
}


void render_helper(ColorGradient *gradient, Float mag_from, Float mag_to, Float time_per_mag, Float fps, Float real, Float imag, size_t frame_min, size_t frame_max, size_t frame_skip)
{
	size_t frames = (fps * (mag_to - mag_from) * time_per_mag).toULong();

	InternalPixelBuffer *buf = new InternalPixelBuffer(1280, 720);
	for(size_t frame = frame_min; frame < frame_max; frame += frame_skip)
	{
		FractalGenerator gen(buf, gradient);

		gen.SetPoint(real, imag);
		Float mag = mag_from + static_cast<Float>(mag_to-mag_from) * frame / frames;
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
