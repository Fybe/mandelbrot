CC=g++
LD=g++
CC_ARGS=-std=c++17 -Wall -Wextra -Wno-unused-parameter -I/usr/include/libpng16 -O3
LD_ARGS=-pthread -lpng16 -lgmp -lmpfr -lboost_program_options


.PHONY: all clean
all: mandelbrot

mandelbrot: main.o PixelBuffer.o ColorScheme.o Spline.o ColorGenerator.o FractalGenerator.o
	$(LD) $(LD_ARGS) -o $@ $^

main.o: main.cpp
	$(CC) $(CC_ARGS) -o $@ -c $< 

PixelBuffer.o: PixelBuffer.cpp PixelBuffer.hpp
	$(CC) $(CC_ARGS) -o $@ -c $<

ColorScheme.o: ColorScheme.cpp ColorScheme.hpp
	$(CC) $(CC_ARGS) -o $@ -c $<

Spline.o: Spline.cpp Spline.hpp
	$(CC) $(CC_ARGS) -o $@ -c $<

ColorGenerator.o: ColorGenerator.cpp ColorGenerator.hpp
	$(CC) $(CC_ARGS) -o $@ -c $<

FractalGenerator.o: FractalGenerator.cpp FractalGenerator.hpp
	$(CC) $(CC_ARGS) -o $@ -c $<


clean:
	@rm *.o
	@rm mandelbrot
