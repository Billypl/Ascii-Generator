#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "Image.h"
#include <fstream>
#include <iostream>
#include <math.h>

using namespace std;

//CONSTRUCTORS

Image::Image(const char* filename)
{
	readImageData(filename);
}

Image::Image(int w, int h, int channels)
	: w(w), h(h), channels(channels)
{
	if (size() > 0)
		pixels.resize(size());
}

Image::Image(const Image& img)
	: Image(img.w, img.h, img.channels)
{
	pixels = img.pixels;
}


Image::Pixel::Pixel(uint8_t R, uint8_t G, uint8_t B) : R(R), G(G), B(B) {};

//MANAGING FILES

void Image::readImageData(const char* filename)
{
	uint8_t* data = nullptr;
	data = stbi_load(filename, &w, &h, &channels, 0);
	pixels = formatToPixels(data);

	bool isReadSuccessfull = data != nullptr;
	delete[] data; 
	data = nullptr;
	printOperationStatus(isReadSuccessfull, filename, "read");
}

void Image::writeImageData(const char* filename)
{
	uint8_t* data = formatToUint8(pixels);
	ImageType type = getFileType(filename);
	bool isWriteSuccessfull = 0;

	switch (type) //calls right 'write' function
	{
	case PNG:
		isWriteSuccessfull = stbi_write_png(filename, w, h, channels, data, w * channels);
		break;
	case JPG:
		isWriteSuccessfull = stbi_write_jpg(filename, w, h, channels, data, 100);
		break;
	case BMP:
		isWriteSuccessfull = stbi_write_bmp(filename, w, h, channels, data);
		break;
	}

	printOperationStatus(isWriteSuccessfull, filename, "write");
	stbi_image_free(data);
}

void Image::printOperationStatus(bool isOperationSuccessfull, const char* filename, string operationName)
{
	if (isOperationSuccessfull)
	{
		string FORorTO = operationName == "read" ? "from" : "to";
		operationName[0] = toupper(operationName[0]);

		cout << operationName << " " << FORorTO << " " << filename << " successfull" << endl;
	}
	else
	{
		cout << "Failed to " << operationName<< " " << filename << endl;
		exit(0);
	}
}

//FILTERS

void Image::grayscaleAvg()
{ 
	if (!containsAtLeastThreeChannels()) return;
	for (auto& p : pixels)
	{
		// (R+G+B)/3 = total/channels = avg
		uint8_t grayscaledPixel = (p.R + p.G + p.B) / 3;
		p.R = p.G = p.B = grayscaledPixel;
	}
}

void Image::grayscaleLum()
{
	if (!containsAtLeastThreeChannels()) return;
	for (auto& p : pixels)
	{
		//formula for RGB value from wiki (human eye perception)
		int gray = (p.R * 0.2126 + p.G * 0.7152 + p.B * 0.0722) / 3;
		p.R = p.G = p.B = gray;
	}
}

void Image::colorMask(float R, float G, float B)
{
	if (!containsAtLeastThreeChannels()) return;
	for (auto& p : pixels)
	{
		p.R *= R;
		p.G *= G;
		p.B *= B;
	}
}

bool Image::containsAtLeastThreeChannels()
{
	if (channels < 3)
	{
		cout << "To use that function, your picture has to have at least 3 channels" << endl;
		return false;
	}
	else 
		return true;
}

//CHANGING POSITIONS

void Image::flipX()
{
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w/2; ++x)
			swap(pixels[x + y * w], pixels[(w - 1 - x) + y * w]);
}

void Image::flipY()
{
	for (int y = 0; y < h / 2; ++y)
		for (int x = 0; x < w; ++x)
			swap(pixels[x + y * w], pixels[x + (h - y - 1) * w]);
}

void Image::flipRight()
{
	vector <Image::Pixel> pixelsNew; 
	pixelsNew.resize(size());

	for (int y = 0; y < h; y++)	
		for (int x = 0; x < w; x++)
		{			
			int index1 = (w - x - 1) + w * y; 
			int index2 = (h - y - 1) + (w - x - 1) * h; 

			pixelsNew[index2] = pixels[index1];
		}

	swap(w, h);
	pixels = pixelsNew;
}


void Image::reduce(float sizeW, float sizeH)
{
	if (!validateSize(sizeW, sizeH)) 
		return;

	deducePixelsOrRatio(sizeW, sizeH);

	float horizontalRatio = sizeW / (float)w;
	float verticalRatio = sizeH / (float)h;

	sizeW = floor(sizeW);
	sizeH = floor(sizeH);

	vector <Image::Pixel> pixelsS(sizeH * sizeW);

	// The program copies a pixel whenever one of these values is at least 1 //(gap to the next pixel)
	float toNextHorizontal = 1;
	float toNextVertical = 1; 

	// Iterate (from the top to the bottom)
	for (int x = 0; x < w; x++)
	{
		int currentLine = x * horizontalRatio;
		if (isBiggerThanDstSize(currentLine, sizeW))
			break;
		if (!isThePixelToCopy(toNextHorizontal, horizontalRatio))
			continue;

		for (int y = 0; y < h; y++)
		{
			int currentColumn = y * verticalRatio;
			if (isBiggerThanDstSize(currentColumn, sizeH))
				break;
			if (!isThePixelToCopy(toNextVertical, verticalRatio))
				continue;

			pixelsS[(int)(currentLine) + (int)(currentColumn) * sizeW] = pixels[x + y * w];
			toNextVertical += verticalRatio - 1.0f;
		}
		toNextVertical = 1.0f;
		toNextHorizontal += horizontalRatio - 1.0f;
	}

	h = sizeH;
	w = sizeW;
	pixels = pixelsS;
}

bool Image::validateSize(float sizeW, float sizeH)
{
	if (sizeW <= 0 || sizeH <= 0)
	{
		cout << "Image can't have negative size" << endl;
		return false;
	}
	if (sizeW > w || sizeH > h)
	{
		cout << "Can't reduce image when target width or/and height are bigger than source" << endl;
		return false;
	}
	return true;
}

void Image::deducePixelsOrRatio(float& dstW, float& dstH)
{
	if (dstW <= 1) dstW *= w;
	if (dstH <= 1) dstH *= h;
}

bool Image::isBiggerThanDstSize(int current, int size)
{
	return current >= size;
}

bool Image::isThePixelToCopy(float& toNext, float ratio)
{
	if (toNext < 1.0f)
	{
		toNext += ratio;
		return false;
	}
	return true;
}

//ASCII

template <typename Out>
void Image::print(Out &out)
{
	//copy to prevent turning orginal into grayscale
	Image THIS = *this;
	THIS.grayscaleAvg();

	const string ascii_density = " .:-=+*#%@";

	for (int i = 0; i < pixels.size(); i++)
	{
		if (i % w == 0) out << endl;

		int index = map_value(THIS.pixels[i].R, 0, 255, 0, ascii_density.size() - 1);
		char brightness = ascii_density[index];
		out << brightness;
	}
}

void Image::printAscii()
{
	print(cout);
}

void Image::printAscii(string resultTextFilename)
{
	ofstream file; 
	file.open(resultTextFilename);

	print(file);

	file.close();
}

int Image::map_value (int value, int MinBaseRange, int MaxBaseRange, int MinDstRange, int MaxDstRange) {
	return (value - MinBaseRange) * (MaxDstRange - MinDstRange) / (MaxBaseRange - MinBaseRange) + MinDstRange;
};

//HELP FUNCTIONS

Image::ImageType Image::getFileType(const char* filename)
{
	string filename_s(filename, strlen(filename));
	string ext = filename_s.substr(filename_s.find('.'));

	if (filename_s.find('.') == string::npos) return PNG; //default PNG

	if (ext == ".png") return PNG;
	if (ext == ".jpg") return JPG;
	if (ext == ".bmp") return BMP;
}

vector <Image::Pixel> Image::formatToPixels(uint8_t* data)
{
	vector <Pixel> pixelsNew;
	pixelsNew.reserve(size());
	for (int i = 0; i < size() * channels; i += channels)
	{
		uint8_t R = data[i], G = data[i + 1], B = data[i + 2];
		Pixel pixel(R, B, G);
		pixelsNew.push_back(pixel);
	}
	return pixelsNew;
}

uint8_t* Image::formatToUint8(vector <Pixel>& pixels)
{
	uint8_t* data = new uint8_t[pixels.size() * channels];
	for (int i = 0; i < size() * channels; i += channels)
	{
		data[i] = pixels[i / channels].R;
		data[i + 1] = pixels[i / channels].G;
		data[i + 2] = pixels[i / channels].B;
	}
	return data;
}

size_t Image::size() { return w * h; }

int Image::getW() { return w; }

int Image::getH() { return h; }

