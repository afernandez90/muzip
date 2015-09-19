#ifndef _MATRIZ_H
#define _MATRIZ_H

template <typename T>
class Matriz
{
	int _N, _M, _p, _q;

	T *_data;

	size_t _size;

	/*! Tabla que contiene el offset al inicio de cada bloque */
	unsigned *_bloqTab;

	typedef T&			reference;
	typedef const T&	const_reference;

public:

	/*! Construye una matriz de N filas y M columnas con bloques de p filas y q columnas.
	 *	Pre: NM mod pq = 0.
	 *
	 *	\param data Buffer que representa la matriz de forma contigua en memoria
	 */
	Matriz(T *data, int N, int M, int p, int q) : _M(M), _N(N), _p(p), _q(q), _data(data)
	{
		int ncb = M / q; // Numero de columnas de bloques
		int nfb = N / p; // Numero de filas de bloques

		_size = nfb * ncb;

		_bloqTab = new unsigned[_size];

		for (int i = 0; i < nfb; ++i) {
			_bloqTab[i * ncb] = M * p * i; 
			for (int j = i * ncb + 1; j < (i+1) * ncb; ++j) { _bloqTab[j] = _bloqTab[j - 1] + q; }
		}
	}

	/*! Pre: bloque pertenece al rango [0..size()-1]
	 *
	 *	\param j	Columna
	 *	\param i	Fila
	 *	\return		Referencia constante a pixel (i,j) del bloque especificado.
	 */
	const_reference operator()(int bloque, int i, int j) const
	{
		return _data[_bloqTab[bloque] + i*_M + j];
	}

	/*! Pre: bloque pertenece al rango [0..size()-1]
	 *
	 *	\param j	Columna
	 *	\param i	Fila
	 *	\return		Referencia al pixel (i,j) del bloque especificado.
	 */
	reference operator()(int bloque, int i, int j)
	{
		return _data[_bloqTab[bloque] + i*_M + j];
	}

	/*! Pre: bloque pertenece al rango [0..size()-1]
	 *
	 *	\param j	Columna
	 *	\param i	Fila
	 *	\return		Referencia constante al pixel (i,j) de la matriz
	 */
	const_reference operator()(int i, int j) const
	{
		return _data[i*_M + j];
	}

	/*! Pre: bloque pertenece al rango [0..size()-1]
	 *
	 *	\param j	Columna
	 *	\param i	Fila
	 *	\return		Referencia al pixel (i,j) de la matriz
	 */
	reference operator()(int i, int j)
	{
		return _data[i*_M + j];
	}

	/*! \return Numero de bloques de la matriz */
	inline size_t size() const { return _size; }

	// Consultoras de los campos
	inline int N() const { return _N; }
	inline int M() const { return _M; }
	inline int p() const { return _p; }
	inline int q() const { return _q; }

	~Matriz() { delete[] _bloqTab; }
};

#endif  // _MATRIZ_H_
