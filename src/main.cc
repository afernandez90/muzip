
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include "Pixel.h"
#include "ppm/io.h"
#include "ppm/ppm.h"
#include "Matriz.hpp"
#include "compr/zipfuncs.h"
#include "types.h"

using namespace std;

double alpha = 100.0;

void zip(const char *image, const char *out, double alpha, unsigned p, unsigned q);
void unzip(const char *in, const char *out);

int main(int argc, char **argv)
{
	if (argc < 2 || argc > 6) {
		cout << "Usage: " << argv[0] << " <input file> [output file] [p] [q] [alpha]" << endl;
		exit(1);
	}

	unsigned p, q;
	p = q = -1;

	if (argc > 5) alpha = atof(argv[5]);
	if (argc > 4) q = atoi(argv[4]);
	if (argc > 3) p = atoi(argv[3]);

	// Cierto si el programa debe comprimir, falso en caso contrario.
	bool compress = false;

	// Nombre del archivo de salida
	string outputfn; 

	// Determinando accion a llevar a cabo +
	// Para determinar el nombre del archivo de salida, se coje el mismo y se cambia de extension

	string infm	= argv[1];	// Nombre del archivo de entrada

	if (infm.substr(infm.find_last_of(".")) == ".mz")
		outputfn = infm.substr(0, infm.find_last_of(".")) + ".ppm";
	else  {// Assuming PPM
		outputfn = infm.substr(0, infm.find_last_of(".")) + ".mz";
		compress = true;
	}

	// En caso de que se explicita el nombre del archivo de salida, se sustituye
	if (argc > 2) outputfn = argv[2];

	if (compress) {	// Iniciando compresión de imagen PPM
		zip(argv[1], outputfn.c_str(), alpha, p, q);
	}
	else { // Iniciando descompresión de imagen PPM
		unzip(argv[1], outputfn.c_str());
	}
}

void zip(const char *image, const char *out, double alpha, unsigned p, unsigned q)
{
	// Leemos imagen
	PPM img = io::read_ppm(image);

	// Ejecutamos la compresion
	pair<void*, size_t> muzip_blob = compr::muzip(img, alpha, p, q);
	
	// Escribir blob en archivo
	fstream f(out, fstream::out | fstream::binary);
	f.write((const char*)muzip_blob.first, muzip_blob.second);
	f.close();
	
	delete[] muzip_blob.first;
}

void unzip(const char *in, const char *out)
{
	fstream f(in, fstream::in | fstream::binary);
	f.seekg(0, fstream::end);
	size_t s = (size_t) f.tellg();
	U8* data = new U8[s];
	f.seekg(0, fstream::beg);
	f.read((char*)data, s);
	f.close();

	PPM result = compr::muunzip(data, s);

	io::write_ppm(result, out);
	
	delete[] data;
}
