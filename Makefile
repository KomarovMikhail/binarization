all:
	g++ main.cpp Image.cpp Image.h -ljpeg -std=c++11
clear:
	rm a.out