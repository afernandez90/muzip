
#ifndef _BLOQUE_H_
#define _BLOQUE_H_

#include "dist/bloqdist.hpp"
#include "Matriz.hpp"

// Clase "wrapper" para aislar el GHT de la estructura de bloques
template <typename T>
class Bloque
{
	// Matriz a la que pertenece el bloque
	const Matriz<T> *mat;

	// Identificador del bloque dentro de la matriz
	size_t _id;

public:

	Bloque() {}
	Bloque(const Matriz<T> *m, size_t bloqid) : _id(bloqid), mat(m) {}

	size_t id() { return _id; }

	// Distancia entre bloques
	double operator-(const Bloque &b) const {
		return dist::bloqdist(*mat, _id, b._id);
	}

	Bloque& operator=(const Bloque &b) {
		mat = b.mat;
		_id = b._id;
		return *this;
	}
};

#endif
