#include "zipfuncs.h"
#include "Matriz.hpp"
#include "compr/GHT.hpp"
#include "Bloque.h"
#include "Pixel.h"
#include "huffman/huffman.h"
#include "../types.h"
#include <vector>

COMPRESSION_NAMESPACE_BEGIN

std::pair<void*,size_t> muzip(const PPM& img, double alpha, unsigned p, unsigned q)
{	
	using namespace std;

	// Ponemos valores por defecto si no se indican en los parametros en p y q
	if (p == -1) p = 8;
	if (q == -1) q = 8;
	
	// Encapsulando pixels
	Pixel *pixImg = new Pixel[img.width() * img.height()];
	for (I32 i = 0; i < img.height(); ++i) {
		for (I32 j = 0; j < img.width(); ++j) {
			pixImg[i * img.width() + j].setImageAndPos(img, i, j);
		}
	}
	
	// Encapsulamos la imagen en una matriz accesible por bloques
	Matriz<Pixel> m(pixImg, img.height(), img.width(), p, q);
	
	// Vector de bloques de pixeles de tamano pq resultantes de la compresión
	vector<Pixel*> vp;
	
	// Array para guardar los MN/pq indices de los bloques que componen la imagen comprimida
	U32 *bloques = new U32[m.size()];

	// Se inserta el primer elemento en el GHT i en el resultado
	bloques[0] = 0;
	vp.push_back(&m(0,0,0));
	
	GHT< Bloque<Pixel> > ght;
	ght.insertar(Bloque<Pixel>(&m, 0));

	// Para cada bloque de la matriz...
	for (U32 i = 1; i < m.size(); ++i) {
		
		Bloque<Pixel> actual = Bloque<Pixel>(&m, i);
		
		I32 indiceDelMasCercano;
		double distanciaAlMasCercano;

		// Cojemos el bloque mas cercano actual
		Bloque<Pixel> b;	
		ght.mas_cercano(actual, indiceDelMasCercano, distanciaAlMasCercano, b);

		// Si la distancia entre el mas cerca es menor que alfa, se comprime
		if (distanciaAlMasCercano < alpha)  bloques[i] = indiceDelMasCercano;
		else {
			// Si no, se añade al conjunto de compresion
			ght.insertar(Bloque<Pixel>(&m, i));
			bloques[i] = vp.size();
			vp.push_back(&m(i,0,0));
		}
	}
	
	// Compactamos los bloques resultantes de la compresión para que queden contiguos en memoria
	// y queden listos para almacenar en disco
	vector< vector< vector<rgb> > > bloqdata(vp.size(), vector< vector<rgb> >(p, vector<rgb>(q)));
	for (U32 i = 0; i < vp.size(); ++i) {
		for (U32 j = 0; j < p; ++j) {
			for (U32 k = 0; k < q; ++k) {
				bloqdata[i][j][k].r = vp[i][j * m.M() + k].r();
				bloqdata[i][j][k].g = vp[i][j * m.M() + k].g();
				bloqdata[i][j][k].b = vp[i][j * m.M() + k].b();
			}
		}
	}

	// En la variable "bloques" tenemos los MN/pq indices de los bloques que conforman la imagen comprimida
	// La variable bloqdata es un vector que contiene los datos de cada bloque que hay que guardar
	// También hay que guardar en disco los valores de N, M, p y q

	// Huffman y guardar en disco
	pair<void*,size_t> huffman_blob = huffman::encode<U32>(bloques, bloques + m.size());
	
	size_t muzip_size = 4 + huffman_blob.second + 4*4 + bloqdata.size() * p * q * 3;
	I8* muzip_blob = new I8[muzip_size];
	
	U32* uptr = (U32*) muzip_blob;
	*uptr++ = huffman_blob.second;
	
	memcpy(uptr, huffman_blob.first, huffman_blob.second);
	
	uptr = (U32*) (muzip_blob + 4 + huffman_blob.second);
	*uptr++ = p;
	*uptr++ = q;
	*uptr++ = img.width();
	*uptr++ = img.height();
	
	U8* cptr = (U8*) uptr;
	
	for (U32 i = 0; i < vp.size(); ++i) {
		for (U32 j = 0; j < p; ++j) {
			for (U32 k = 0; k < q; ++k) {
				*cptr++ = bloqdata[i][j][k].r;
				*cptr++ = bloqdata[i][j][k].g;
				*cptr++ = bloqdata[i][j][k].b;
			}
		}
	}
	
	delete[] bloques;
	delete[] pixImg;
	
	return make_pair(muzip_blob, muzip_size);
}


PPM muunzip(const U8* input, size_t fileSize)
{
	using namespace std;

	U32* uptr = (U32*) input;
	U32 s = *uptr;
	uptr++;
	
	vector<U32> bloques;
	huffman::decode<U32>((void*)uptr, s, bloques);
	
	size_t N, M, p, q, nBlocks;
	
	uptr = (U32*) (input + 4 + s);
	p = *uptr++;
	q = *uptr++;
	M = *uptr++;
	N = *uptr++;

	nBlocks = (fileSize - s - 4) / (3*p*q);

	// Leemos los bloques del archivo
	vector< vector< vector<rgb> > > bloqdata(nBlocks, vector< vector<rgb> >(p, vector<rgb>(q)));

	U8* cptr = (U8*) uptr;

	for (U32 i = 0; i < bloqdata.size(); ++i) {
		for (U32 j = 0; j < p; ++j) {
			for (U32 k = 0; k < q; ++k) {
				bloqdata[i][j][k].r = *cptr++;
				bloqdata[i][j][k].g = *cptr++;
				bloqdata[i][j][k].b = *cptr++;
			}
		}
	}

	// Creamos una imagen nueva para guardar lo que descomprimimos, junto con las estructuras que la manejan
	PPM unzippedPPM(N, M);
	Pixel *pixUnzipImg = new Pixel[N * M];
	for (U32 i = 0; i < N; ++i) {
		for (U32 j = 0; j < M; ++j) {
			pixUnzipImg[i * M + j].setImageAndPos(unzippedPPM, i, j);
		}
	}
	
	Matriz<Pixel> imagenFinal(pixUnzipImg, N, M, p, q);

	// Tabla para guardar los datos descomprimidos sin estructurar
	rgb *decompressedData = new rgb[N*M];
	Matriz<rgb> munzip(decompressedData, N, M, p, q);

	// Descomprimimos
	for (U32 i = 0; i < imagenFinal.size(); ++i) {
		for (U32 j = 0; j < p; ++j) {
			for (U32 k = 0; k < q; ++k) {
				munzip(i,j,k) = bloqdata[ bloques[i] ][j][k];
			}
		}
	}

	// Compactamos en una imagen ppm
	for (U32 i = 0; i < N; ++i) {
		for (U32 j = 0; j < M; ++j) {
			rgb current = decompressedData[i * M + j];
			imagenFinal(i, j).setValues(current.r, current.g, current.b);
		}
	}

	delete[] decompressedData;
	delete[] pixUnzipImg;

	return unzippedPPM;
}

COMPRESSION_NAMESPACE_END
