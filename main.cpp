//AsciiGenerator v. 1.05
#include "Image.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
	Image img("image.jpg");
	img.reduce(100, 0.5); //3th use (see func delcaration) 
	img.print_ascii();
	img.print_ascii("result.txt");

	return 0;
}
