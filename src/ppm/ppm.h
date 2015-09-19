#ifndef _PPM_H_
#define _PPM_H_

#include "types.h"
#include <boost/shared_array.hpp>

/// Estructura para almacenar una imagen ppm.
class PPM
{
	int w;
	int h;
	boost::shared_array<U8> data;
	
public:

	PPM() {}

	/// Construye un ppm de dimensiones widthxheight y datos d.
	PPM (int height, int width, U8* d) :
		w(width), h(height), data(boost::shared_array<U8>(d)) {}
		
	/// Construye un ppm de dimensiones widthxheight.
	PPM (int height, int width) :
		w(width), h(height), data(boost::shared_array<U8>(new U8[w*h*3])) {}
	
	/// Devuelve el componente r del pixel (i,j).
	U8 r (int i, int j) const { return data[(i*w + j)*3]; }
	
	/// Devuelve el componente g del pixel (i,j).
	U8 g (int i, int j) const { return data[(i*w + j)*3 + 1]; }
	
	/// Devuelve el componente b del pixel (i,j).
	U8 b (int i, int j) const { return data[(i*w + j)*3 + 2]; }
	
	/// Devuelve la anchura en pixels de la imagen.
	int width() const { return w; }
	
	/// Devuelve la altura en pixels de la imagen.
	int height() const { return h; }
	
	/// Modifica el componente r del pixel (i,j).
	void set_r (int i, int j, U8 val) { data[(i*w + j)*3] = val; }
	
	/// Modifica el componente g del pixel (i,j).
	void set_g (int i, int j, U8 val) { data[(i*w + j)*3 + 1] = val; }
	
	/// Modifica el componente b del pixel (i,j).
	void set_b (int i, int j, U8 val) { data[(i*w + j)*3 + 2] = val; }
};

#endif // _PPM_H_
