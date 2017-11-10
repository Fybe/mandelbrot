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


int main(int argc, char **argv)
{
	cout.precision(2);
	cout.setf(iostream::fixed);

	// parse parameter
	po::positional_options_description p;
	p.add("output", 1);

	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h",						"displays this help message")
		("precision,p",					po::value<size_t>()->default_value(128),				"precision for floating-point calculation in bits")
		("threads,t",					po::value<size_t>()->default_value(thread::hardware_concurrency()),				"number of threads used for rendering")
		("real",						po::value<string>()->default_value("-0.5"),				"point on real axis")
		("imag",						po::value<string>()->default_value("0"),				"point on imaginary axis")
		("iterations,i",				po::value<uint64_t>()->default_value(1ul << 8ul),		"number of iterations")
		("magnitude",					po::value<vector<string>>()->multitoken(),				"zoom level, beginning to ending")
		("speed",						po::value<string>()->default_value("3.0"),				"zoom speed in seconds per magnitude")
		("fps",							po::value<string>()->default_value("30"),				"frames per second for output video")
		("gradient",					po::value<string>(),									"file containing gradient specifications")
		("mode",						po::value<string>()->default_value("single"),			"valid options: single, zoom")
		("output,O",					po::value<string>()->default_value("output/image%05d.png"), "output file, %05d for numbering")
		("size",						po::value<vector<uint32_t>>()->multitoken()->default_value(vector<uint32_t>(), ""),	"width and height in pixels")
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

	uint32_t width, height;
	if(vm["size"].as<vector<uint32_t>>().size() != 2)
	{
		width = 1280u;
		height = 720u;
	}
	else
	{
		width = vm["size"].as<vector<uint32_t>>()[0];
		height = vm["size"].as<vector<uint32_t>>()[1];
	}

	if(vm["mode"].as<string>() == "zoom")
	{
		Float mag_from = vm["magnitude"].as<vector<string>>()[0];
		Float mag_to = vm["magnitude"].as<vector<string>>()[1];
		Float time_per_mag = vm["speed"].as<string>();
		Float fps = vm["fps"].as<string>();
		size_t frames = (fps * (mag_to - mag_from) * time_per_mag).toULong();

		InternalPixelBuffer *buf = new InternalPixelBuffer(width, height);
		for(size_t frame = 0; frame < frames; frame++)
		{
			FractalGenerator gen(buf, &gradient);

			gen.SetPoint(c_real, c_imag);
			Float mag = mag_from + static_cast<Float>(mag_to-mag_from) * frame / frames;
			gen.SetMagnitude(mag);
			gen.SetIterations(vm["iterations"].as<uint64_t>());
			auto beg = chrono::high_resolution_clock::now();
			gen.Render(vm["threads"].as<size_t>());
			auto end = chrono::high_resolution_clock::now();

			char cbuf[100];
			snprintf(cbuf, sizeof(cbuf), vm["output"].as<string>().c_str(), frame);
			buf->WriteToFile(cbuf);

			stringstream ss = stringstream();
			ss << "frame " << frame << " of " << frames << " rendered in " << chrono::duration_cast<chrono::milliseconds>(end-beg).count()/1000.f << "s" << endl;
			cout << ss.str();
			cout.flush();
		}
		delete buf;
	}
	else if(vm["mode"].as<string>() == "single")
	{
		Float mag = vm["magnitude"].as<vector<string>>()[0];

		InternalPixelBuffer *buf = new InternalPixelBuffer(width, height);
		FractalGenerator gen(buf, &gradient);
		gen.SetPoint(c_real, c_imag);
		gen.SetMagnitude(mag);
		gen.SetIterations(vm["iterations"].as<uint64_t>());

		auto beg = chrono::high_resolution_clock::now();
		thread t(&FractalGenerator::Render, &gen, vm["threads"].as<size_t>());
		float prog;
		while((prog = gen.GetProgress()) < 1.f)
		{
			auto now = chrono::high_resolution_clock::now();
			auto dur = chrono::duration_cast<chrono::seconds>(now - beg);

			uint32_t seconds = dur.count() % 60;
			uint32_t minutes = (dur.count() / 60) % 60;
			uint32_t hours   = dur.count() / 3600;

			cout << "\r";
			if(hours > 0)
				cout << setw(3) << hours << "h";
			else
				cout << setw(4) << "";

			if(minutes > 0)
				cout << setw(3) << minutes << "m";
			else
				cout << setw(4) << "";

			if(seconds > 0)
				cout << setw(3) << seconds << "s";
			else
				cout << setw(4) << "";

			cout << " [ " << prog*100.f << "% ] -- ";

			if(prog == 0)
				cout << "never finishing";
			else
			{
				auto dur_left = chrono::duration_cast<chrono::seconds>(dur * 1.f / prog) - dur;
				dur_left = dur_left.count() < 0 ? chrono::seconds(0) : dur_left;

				seconds = dur_left.count() % 60;
				minutes = (dur_left.count() / 60) % 60;
				hours   = dur_left.count() / 3600;

				if(hours > 0)
					cout << setw(3) << hours << "h";
				else
					cout << setw(4) << "";

				if(minutes > 0)
					cout << setw(3) << minutes << "m";
				else
					cout << setw(4) << "";

				if(seconds > 0)
					cout << setw(3) << seconds << "s";
				else
					cout << setw(4) << "";

				cout << " left";
			}
			cout.flush();

			std::this_thread::sleep_for(chrono::milliseconds(500));
		}
		auto end = chrono::high_resolution_clock::now();

		t.join();
		char cbuf[100];
		snprintf(cbuf, sizeof(cbuf), vm["output"].as<string>().c_str(), 0);
		buf->WriteToFile(cbuf);
		delete buf;

		stringstream ss;
		ss = stringstream();
		ss << "frame " << 0 << " of " << 1 << " rendered in " << chrono::duration_cast<chrono::milliseconds>(end-beg).count()/1000.f << "s" << endl;
		cout << endl << ss.str();
		cout.flush();
	}

	return 0;
}
