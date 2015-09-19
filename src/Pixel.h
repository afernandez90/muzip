
#ifndef _PIXEL_H_
#define _PIXEL_H_

#include "types.h"
#include "ppm/ppm.h"
#include <cmath>

// El tipo pixel actua de adaptador entre la matriz que manipula por bloques y el fichero ppm
class Pixel
{
	PPM _img;
	int i, j;
	
public:

	Pixel() {}
	Pixel(PPM imagen, int ii, int jj) : _img(imagen), i(ii), j(jj) {}
	
	// Asigna la referencia de la imagen a la que pertenece el pixel y su posicion
	void setImageAndPos(PPM image, int ii, int jj) {
		_img = image;
		i = ii;
		j = jj;
	}

	// Getters de los campos
	U8 r() const { return _img.r(i,j); }
	U8 g() const { return _img.g(i,j); }
	U8 b() const { return _img.b(i,j); }

	// Setter
	void setValues(U8 r, U8 g, U8 b)
	{
		_img.set_r(i, j, r);
		_img.set_g(i, j, g);
		_img.set_b(i, j, b);
	}

	// Distancia entre pixeles
	double operator-(const Pixel& p) const
	{ 
		return	(	std::abs(r()-p.r()) +  
					std::abs(g()-p.g()) +
					std::abs(b()-p.b())		) / 3.0;
	}

	Pixel& operator=(const Pixel& p)
	{
		setValues(p.r(), p.g(), p.b());

		return *this;
	}
};

#endif // _PIXEL_H_
