
#ifndef _GHT_H_
#define _GHT_H_

#include "compr/compr.h"

COMPRESSION_NAMESPACE_BEGIN


// Estructura metrica que permite calcular eficientemente el elemento
// mas cercano a uno dado. El tipo de elemento que contiene debe implementar
// el operador-, el cual da la distancia en valor absoluto entre dos elementos.
template <typename T>
class GHT
{
	struct nodo {
		nodo *der, *izq;
		T elem;

		// Este nodo fue el i-esimo que entro en el GHT
		size_t i;

		nodo() {}
		nodo(	nodo *derp, nodo *izqp, const T &elemp, size_t ip) :
				der(derp), izq(izqp), elem(elemp), i(ip) {}
	};

	// La raiz ficiticia para la optimizacion de guardar solo un elemento en cada nodo
	nodo *ficticialRoot;
	nodo *root;

	size_t count;

	/*! Pre: count > 1
	 *
	 *	\param x		Elemento a insertar 
	 *	\param arb		Arbol en el que insertar
	 *	\param ant		Puntero al elemento padre de este subarbol dentro del nodo padre
	 */
	// Coste log(N)*C donde N es el numero de elementos presentes en el GHT y
	// C el coste de una comparación de elementos
	void insertar_rec(const T &x, nodo *&arb, double distx_padre) {
		if (!arb) {
			arb = new nodo(0, 0, x, count);
		}
		else {
			double distIzq = arb->elem - x;
			if (distx_padre < distIzq)	insertar_rec(x, arb->der, distx_padre);
			else						insertar_rec(x, arb->izq, distIzq);
		}
	}

	/*! Pre: count > 1
	 *
	 *	\param x			Elemento a buscar 
	 *	\param arb			Arbol en el que buscar
	 *	\param i[out]		Indice del elemento que se retorna
	 *  \param r[out]		Distancia al elemento mas cercano encontrado
	 *	\param nn[out]		Elemento mas cercano a x
	 *	\param distpadre	Distancia al nodo padre
	 */
	// Coste lineal respecto al numero de elementos en el GHT. Es decir, en caso peor se compara x
	// con todos los elementos del GHT.
	// En caso medio, se hacen log(N) comparaciones.
	const void mas_cercano_rec(const T &x, nodo *arb, int &i, double &r, T &nn, double distpadre) const {
		if (arb) {

			double dpq = x - arb->elem;

			if (dpq <= r) {
				i = arb->i;
				r = dpq;
				nn = arb->elem;
			}

			if (dpq <= distpadre) {
				mas_cercano_rec(x, arb->izq, i, r, nn, dpq);
				if (dpq + r > distpadre - r)
					mas_cercano_rec(x, arb->der, i, r, nn, distpadre);
			} else {
				mas_cercano_rec(x, arb->der, i, r, nn, dpq);
				if (dpq - r > distpadre + r)
					mas_cercano_rec(x, arb->izq, i, r, nn, distpadre);
			}
		}
	}

public:

	GHT() : count(0)
	{
	}

	~GHT()
	{
		erase_nodes(ficticialRoot);
	}

	// Elimina recursivamente un nodo y todos sus hijos
	void erase_nodes(nodo *n)
	{
		if (n) {
			erase_nodes(n->der);
			erase_nodes(n->izq);
			delete(n);
		}
	}

	/// Devuelve el elemento mas cercano y en i deja su posicion en orden de entrada (0 si entro el primero, etc.)
	/// esto es util cuando el cliente necesita mapear los elementos que va encontrando mas cerca segun los fue insertando
	/// r es la distancia a la que se encuentra dicho elemento mas cercano
	// Coste lineal respecto al numero de elementos en el GHT. Es decir, en caso peor se compara x
	// con todos los elementos del GHT.
	// En caso medio, se hacen log(N) comparaciones.
	const void mas_cercano(const T &x, int &i, double &r, T &nn) const
	{
		r = ficticialRoot->elem - x;
		nn = ficticialRoot->elem;
		i = 0;
		if (count > 1) mas_cercano_rec(x, root, i, r, nn, r);
	}

	/// Devuelve el elemento mas cercano a x
	/// r es la distancia a la que se encuentra dicho elemento mas cercano
	// Coste lineal respecto al numero de elementos en el GHT. Es decir, en caso peor se compara x
	// con todos los elementos del GHT.
	// En caso medio, se hacen log(N) comparaciones.
	const void mas_cercano(const T &x, double &r, T &nn) const
	{
		mas_cercano(x, int(), r, nn);
	}

	// Numero de elementos en el GHT
	size_t size() const { return count; }

	// Inserta el elemento x en el GHT
	// Coste log(N)*C donde N es el numero de elementos presentes en el GHT y
	// C el coste de una comparación de elementos
	void insertar(const T &x)
	{
		if (count < 2) {
			if (count == 0) ficticialRoot = new nodo(0, 0, x, 0);
			else { 
				root = new nodo(0, 0, x, 1); 
				ficticialRoot->izq = root; 
			}
		}
		else insertar_rec(x, root, ficticialRoot->elem - x);

		count++;
	}
};

COMPRESSION_NAMESPACE_END

#endif //  _GHT_H_

