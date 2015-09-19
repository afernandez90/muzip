#include "ppm/io.h"
#include <fstream>
#include <string>

PPM io::read_ppm (const char* filename)
{
	using std::string;
	using std::fstream;
	
	fstream is(filename, fstream::in | fstream::binary);
	
	string header;
	int w, h, range;
	is >> header >> w >> h >> range;
	is.get();
	
	if (!(header == "P6")) throw "bad header";
	if (is.fail()) throw "bad format";
	
	unsigned char* data = new unsigned char[w*h*3];
	for (int i = 0; i < w*h*3; ++i) data[i] = is.get();
	
	is.close();
	
	if (is.fail())
	{
		delete[] data;
		throw "bad data";
	}
	
	return PPM(h, w, data);
}


void io::write_ppm (const PPM& image, const char* filename)
{
	using std::fstream;
	fstream os(filename, fstream::out | fstream::binary);
	
	int w = image.width();
	int h = image.height();
	
	os << "P6"; os.put(0xa); os << w; os.put(0x20); os << h;
	os.put(0xa); os << "255"; os.put(0xa);
	
	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			os << image.r(i,j) << image.g(i,j) << image.b(i,j);
		}
	}
	
	if (!os.good()) throw "write error";
	os.close();
}
