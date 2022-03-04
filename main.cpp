//AsciiGenerator v. 1.06
#include "Image.h"
#include <iostream>
#include <string>

using namespace std;

float calcSize(int pixels, int orginalSize, int ratio)
{
	return (float)pixels / (float)orginalSize * 0.5;
}

int main()
{
	Image img("image.jpg");

	int width = 100;
	img.reduce(width, calcSize(width, img.getW(), 0.5));

	img.printAscii();
	img.printAscii("result.txt");

	return 0;
}
