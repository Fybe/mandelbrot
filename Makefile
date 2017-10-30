CC=g++
LD=g++
CC_ARGS=-std=c++17 -Wall -Wextra -Wno-unused-parameter -I/usr/include/libpng16
LD_ARGS=-lpng16


.PHONY: all
all: mandelbrot

mandelbrot: main.o PixelBuffer.o
	$(LD) $(LD_ARGS) -o $@ $^

main.o: main.cpp
	$(CC) $(CC_ARGS) -o $@ -c $< 

PixelBuffer.o: PixelBuffer.cpp PixelBuffer.hpp
	$(CC) $(CC_ARGS) -o $@ -c $<


clean:
	@rm main.o
	@rm PixelBuffer.o
	@rm mandelbrot
