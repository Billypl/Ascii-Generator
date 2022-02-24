#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <math.h>

using namespace std;

//HELP FUNCTIONS
ImageType Image::getFileType(const char* filename)                             
{
	const auto constChar_to_str = [](const char* str) 
	{
		string s_str(str, strlen(str));
		return s_str;
	};

	string filename_s = constChar_to_str(filename);
	string ext = filename_s.substr(filename_s.find('.'));

	if (filename_s.find('.') == std::string::npos) return PNG; //default PNG
	
	if (ext == ".png") return PNG;
	if (ext == ".jpg") return JPG;
	if (ext == ".bmp") return BMP;
}

void Image::formatToPixels(uint8_t* data)
{
	pixels.clear();
	for (int i = 0; i < size*channels; i+=channels)
	{
		Pixel p;
		p.R = data[i];
		p.G = data[i+1];
		p.B = data[i+2];

		pixels.push_back(p);
	}
}
uint8_t* Image::formatToUint8()
{
	uint8_t* data = new uint8_t[pixels.size()*channels];
	for (int i = 0; i < size*channels; i+=channels)
	{
		data[i] = pixels[i / channels].R;
		data[i + 1] = pixels[i / channels].G;
		data[i + 2] = pixels[i / channels].B;
	}
	return data;
}

//CONSTRUCTORS
Image::Image(const char* filename)
{
	if (read(filename))
		cout << "Read " << filename << endl;
	else
	{
		cout << "Failed to read " << filename << endl;
		exit(0);
	}
}
Image::Image(int w, int h, int channels)
	: w(w), h(h), channels(channels)
{
	size = w * h;
	if (!size) return;
	pixels.resize(size);
}
Image::Image(const Image& img)
	: Image(img.w, img.h, img.channels) //!clears values when object returned from flipRight() is being assigned to new object
{
	pixels = img.pixels;
}
Image::~Image()
{
	//if(data != nullptr) this->write("result.png"); //overriting the file
	//stbi_image_free(data);
}

//MANAGING FILES
bool Image::read(const char* filename)
{
	uint8_t* data = nullptr;
	data = stbi_load(filename, &w, &h, &channels, 0);
	size = w * h;
	this->formatToPixels(data);
	
	bool success = data != nullptr;
	delete[] data; data = nullptr;
	
	return success;
}
bool Image::write(const char* filename)
{
	uint8_t* data = this->formatToUint8();
	ImageType type = getFileType(filename);
	int success = 0;

	switch (type) //calls right write function
	{
	case PNG:
		success = stbi_write_png(filename, w, h, channels, data, w * channels);
		break;
	case JPG:
		success = stbi_write_jpg(filename, w, h, channels, data, 100);
		break;
	case BMP:
		success = stbi_write_bmp(filename, w, h, channels, data);
		break;
	}
	stbi_image_free(data);
	return success != 0;
}

//FILTERS
Image& Image::grayscale_avg()
{ // (R+G+B)/3 = total/channels = avg
	if (channels < 3)
		cout << "Image " << this << " has less than 3 channels. It is assumed to be grayscale";
	else
	{
		for (auto& p : pixels)
		{
			int gray = (p.R + p.G + p.B) / 3;
			p.R = p.G = p.B = gray;
		}
	}
	return *this;
}
Image& Image::grayscale_lum()
{
	if (channels < 3)
		cout << "Image " << this << " has less than 3 channels. It is assumed to be grayscale";
	else
	{
		for (auto& p : pixels)
		{
			//formula for RGB value from wiki (human eye perception)
			int gray = (p.R * 0.2126 + p.G * 0.7152 + p.B * 0.0722) / 3;
			p.R = p.G = p.B = gray;
		}
	}
	return *this;
}
Image& Image::colorMask(float R, float G, float B)
{
	if (channels < 3)
		cout << "To use that function, your picture has to have at least 3 channels" << endl;
	else
	{
		for (auto& p : pixels)
		{
			p.R *= R;
			p.G *= G;
			p.B *= B;
		}
	}
	return *this;
}

//POSITIONS
Image& Image::flipX()
{
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w/2; ++x)
			swap(pixels[x + y * w], pixels[(w - 1 - x) + y * w]);

	return* this;
}
Image& Image::flipY()
{
	for (int y = 0; y < h / 2; ++y)
		for (int x = 0; x < w; ++x)
			swap(pixels[x + y * w], pixels[x + (h - y - 1) * w]);

	return *this;
}
Image& Image::flipRight()
{
	if (pixels.empty()) return *this;
	vector <Image::Pixel> pixels_new; pixels_new.resize(size);

	for (int y = 0; y < h; y++)	
		for (int x = 0; x < w; x++)
		{
			//y = x; x = h - y - 1; //=>zamiana w lewo
			int index1 = (w - x - 1) + w * y; 
			int index2 = (h - y - 1) + (w - x - 1) * h; 

			pixels_new[index2] = pixels[index1];
			continue;
		}

	swap(w, h);
	pixels = pixels_new;
	
	return *this;
}

bool Image::deducePixelsOrPercentage(const float ratioW, const float ratioH, float& dstW, float& dstH)
{
	if (ratioW < 0 || ratioH < 0)
		return 0;

	else if (ratioW <= 1 || ratioH <= 1)
	{
		if (ratioW <= 1 && ratioH <= 1)
		{
			dstW = ratioW * w;
			dstH = ratioH * h;
		}
		else if (ratioH <= 1)
			dstH = dstW / w * h * ratioH;
		else if (ratioW <= 1)
			dstW = dstH / h * w * ratioW;
	}
	return 1;
}
Image Image::reduce(float ratioW, float ratioH)
{
	float dstW = ratioW, dstH = ratioH;

	if (!deducePixelsOrPercentage(ratioW, ratioH, dstW, dstH))
	{
		cout << "Image can't have negative size" << endl;
		return *this;
	}


	if (dstW > w || dstH > h)
	{
		cout << "Can't reduce image when target width or/and height are bigger than source" << endl;
		return *this;
	}

	int copiedColumns = 0;
	int copiedLines = 0;

	// the ratio value is used to determine how many lines/columns we need to skip
	float horizontalRatio = dstW / (float)w;
	if (dstH == 0) dstH = h * horizontalRatio;
	float verticalRatio = dstH / (float)h;

	dstW = floor(dstW);
	dstH = floor(dstH);

	vector <Image::Pixel> pixelsS; pixelsS.resize(dstH * dstW);

	// The program copies a pixel whenever one of these values is at least 1
	float toNextHorizontal = 1.0f; // current Horizontal "gap" to the next pixel
	float toNextVertical = 1.0f; // current Vertical "gap" to the next pixel

	// Iterate (from the top of the image to the bottom)
	for (int x = 0; x < w; x++)
	{
		if (copiedColumns == dstW)
			break;
		if (toNextHorizontal >= 1.0f)
		{
			for (int y = 0; y < h; y++)
			{
				if (copiedLines == dstH)
					break;

				// Make sure to only copy the needed pixels of the current column
				if (toNextVertical >= 1.0f)
				{
					pixelsS[copiedColumns + copiedLines * dstW] = pixels[x + y * w];

					copiedLines += 1;
					toNextVertical -= 1.0f;
				}
				toNextVertical += verticalRatio;
			}
			copiedLines = 0;
			copiedColumns += 1;
			toNextHorizontal -= 1.0f;
			toNextVertical = 1.0f;
		}
		toNextHorizontal += horizontalRatio;
	}

	//writing new data info
	h = dstH;
	w = dstW;
	size = w * h;

	//overriding old pixels with new ones and formating it to *data
	pixels = pixelsS;

	return *this;
}

//ASCII
template <typename Out>
void Image::print(Out &out)
{
	//copy to prevent turning orginal into grayscale
	Image THIS = *this;
	THIS.grayscale_avg();

	const string ascii_density = " .:-=+*#%@";

	//maps value to the closest value in range (val, base range, range to assing)
	const auto map_value = [](int value, int min1, int max1, int min2, int max2) {
		return (value - min1) * (max2 - min2) / (max1 - min1) + min2;
	};

	for (int i = 0; i < pixels.size(); i++)
	{
		if (i % w == 0) out << endl;

		int index = map_value(THIS.pixels[i].R, 0, 255, 0, ascii_density.size() - 1);
		char brightness = ascii_density[index];
		out << brightness;
	}
}
void Image::print_ascii()
{
	print(cout);
}
void Image::print_ascii(string resultTextName)
{
	ofstream file; 
	file.open(resultTextName);

	print(file);

	file.close();
}

