
#ifndef _BLOQDIST_H_
#define _BLOQDIST_H_

#include "Matriz.hpp"

#define DIST_NAMESPACE_BEGIN	namespace dist {
#define DIST_NAMESPACE_END		}

DIST_NAMESPACE_BEGIN

// Distancia entre los bloques a y b en la matriz m. La distancia se calcula
// haciendo la media aritmetica igualmente ponderada para todos los elementos del bloque
template <typename T>
double bloqdist(const Matriz<T> &m, size_t a, size_t b)
{
	double dist = 0.0;

	for (int i = 0; i < m.p(); ++i) {
		for (int j = 0; j < m.q(); ++j) {
			dist += m(a, i, j) - m(b, i, j);
		}
	}

	return dist;
}

DIST_NAMESPACE_END

#endif // _BLOQDIST_H_
