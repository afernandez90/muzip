
#ifndef _ZIPFUNCS_H_
#define _ZIPFUNCS_H_

#include "ppm/ppm.h"
#include "compr/compr.h"
#include "types.h"
#include <utility>

COMPRESSION_NAMESPACE_BEGIN

const unsigned default_divisor_for_p_and_q = 64;

// Estructura para encapsular los datos que van al disco
struct rgb {
	U8 r, g, b;
};

// Comprime la imagen dada y devuelve un blob binario con el archivo muzip
// Coste en caso peor: O(N^2)
// Coste en caso medio = N log(N) + Delta donde Delta es un parametro que depende de p, q y alpha.
// N es el numero de pixeles de la imagen "img".
std::pair<void*,size_t> muzip(const PPM& img, double alpha, unsigned p, unsigned q);

/*! Paso final de la descompresion mu-zip
 *
 *	\return Imagen PPM	resultante de la descompresion
 *	\param	input[in]	Archivo muzip a descomprimir
 *	\param	fileSize	Tamano del archivo input
 */
// Coste lineal respecto al tamaño del archivo comprimido
PPM muunzip(const U8* input, size_t fileSize);

COMPRESSION_NAMESPACE_END

#endif // _ZIPFUNCS_H_
