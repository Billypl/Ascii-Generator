#pragma once
#include <stdint.h>
#include <vector>
#include <string>

class Image {

private:

	enum ImageType {
		PNG, JPG, BMP
	};
	struct Pixel {
		Pixel(uint8_t R = 0, uint8_t G = 0, uint8_t B = 0);
		uint8_t R, G, B;
	};

	std::vector <Pixel> pixels;
	int w;
	int h;
	int channels;

	//CONSTRUCTORS
public:
	Image(const char* filename); //read from file
	Image(int w, int h, int channels); //new blank
	Image(const Image& img); //copy

	//MANAGING FILES
public:
	void writeImageData(const char* filename);
private:
	void readImageData(const char* filename);
	void printOperationStatus(bool isOperationSuccessfull, const char* filename, std::string operationName);

	//FILTERS
public:
	void grayscaleAvg();
	void grayscaleLum();
	void colorMask(float R, float G, float B);
private:
	bool containsAtLeastThreeChannels();
	
	//CHANGING POSITIONS
public:
	void flipX();
	void flipY();
	void flipRight();

public:
	void reduce(float dstW, float dstH);
private:
	void deducePixelsOrRatio(float& dstH, float& dstW);
	bool validateSize(float sizeW, float sizeH);
	bool isBiggerThanDstSize(int current, int size);
	bool isThePixelToCopy(float& toNext, float ratio);

	//ASCII
public:
	void printAscii(); 
	void printAscii(std::string resultTextName);
private:
	//maps value to the closest value in range (val, base range, range to assing)
	int map_value(int value, int MinBaseRange, int MaxBaseRange, int MinDstRange, int MaxDstRange);
	template <typename Out>
	void print(Out& out);

	//HELP FUNKCTIONS
private:
	ImageType getFileType(const char* filename);
	std::vector <Pixel> formatToPixels(uint8_t* data);
	uint8_t* formatToUint8(std::vector <Pixel>& pixels);
public:
	size_t size();
	int getW();
	int getH();
};