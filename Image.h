#pragma once
#include <string>
#include <stdint.h>
#include <vector>
using namespace std;



class Image {

private:

	enum ImageType {
		PNG, JPG, BMP
	};
	struct Pixel {
		Pixel(uint8_t R = 0, uint8_t G = 0, uint8_t B = 0);
		uint8_t R, G, B;
	};

	vector <Pixel> pixels;
	int w;
	int h;
	int channels;

	ImageType getFileType(const char* filename);
	vector <Pixel> formatToPixels(uint8_t* data);
	uint8_t* formatToUint8(vector <Pixel>& pixels);
	size_t size();

public:
	Image(const char* filename); //read from file
	Image(int w, int h, int channels); //new blank
	Image(const Image& img); //copy
	
	bool readImageData(const char* filename);
	bool writeImageData(const char* filename);

	void grayscale_avg();
	void grayscale_lum();
	void colorMask(float R, float G, float B);
	
	void flipX();
	void flipY();
	void flipRight();

private:
	bool deducePixelsOrPercentage(const float ratioW, const float ratioH, float& dstH, float& dstW);
public:
	//There are 3 ways to use this function
	// 1. Pixels size:
	// - Pass just new width SIZE, height will be deduced
	//	        i.e. image 1000x500 ==> reduce(100) ==> image 100x50
	// - Pass both new width and height SIZE (full control over streching the image) 
	// 2. Ratio size:
	// - Pass new RATIO (applied for width AND height)
	// - Pass both new width and height RATIO 
	// 
	// 3.  BONUS: Pixel + ratio of pixels
	// Pass one SIZE and one RATIO - ratio parameter will become the ratio of the SIZE argument
	//	         i.e. image 1000x500 ==> reduce(100, 0.5) ==> image 100x25
	void reduce(float dstW, float dstH = 0.0);

private:
	template <typename Out>
	void print(Out &out);
public:
	//print ascii version to a console
	void print_ascii(); 
	//print ascii version to a text file
	void print_ascii(string resultTextName);
};