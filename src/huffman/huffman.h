#ifndef _HUFFMAN_H
#define _HUFFMAN_H

#include <utility>
#include <vector>
#include <cstring> // size_t
#include "../types.h"

/// API Publica.
namespace huffman {

/// Codifica la secuencia dada con codificacion de huffman.
template <class T, class iter_t>
std::pair<void*,size_t> encode (iter_t begin, const iter_t& end);

/// Decodifica el bloque dado.
template <class T, class cont_t>
void decode (const void* blob, size_t size, cont_t& cont);

} // namespace huffman end




/// API privada.
#include "HuffmanTree.hpp"
#include <vector>
#include <cstdlib>
#include <string>

namespace huffman {

enum num_type { num_byte, num_word, num_dword };

/// Codifica la secuencia dada con la tabla de Huffman dada.
template <class T, class iter_t, class cont_t>
void encode_seq (iter_t begin, const iter_t& end, const std::map< T,std::vector<bool> >& table, cont_t& cont)
{
	for (; begin != end; ++begin)
	{
		const std::vector<bool>& c = table.find(*begin)->second;
		for (size_t i = 0; i < c.size(); ++i) cont.push_back(c[i]);
	}
}


inline std::string hf_to_string (const std::vector<bool>& code)
{
	std::string s;
	for (size_t i = 0; i < code.size(); ++i) s += (code[i] + 0x30);
	return s;
}


/// Decodifica la secuencia dada con la tabla de Huffman dada.
template <class T, class iter_t, class cont_t>
void decode_seq (const iter_t& begin, const iter_t& end, const std::map< T,std::vector<bool> >& table, cont_t& cont)
{
	HuffmanTree<T> tree(table);
	tree.decode (begin, end, cont);
}


/// Rota el I8 dado 1 bit a la derecha.
U8 rotate_right (U8 c);


/// Escribe un valor de tipo T en el blob dado.
/// Retorna la nueva posicion en el blob.
template <class T>
void* write_num (void* blob, T val)
{
	T* ptr = (T*) blob;
	*ptr = val;
	ptr++;
	return (void*) ptr;
}


/// Lee un valor de tipo T del blob dado.
/// Retorna la nueva posicion en el blob.
template <class T>
const void* read_num(const void* blob, T& val)
{
	T* ptr = (T*) blob;
	val = *ptr;
	ptr++;
	return (void*) ptr;
}


/// Serializa la secuencia de bits dada.
template <class iter_t>
std::pair<void*,size_t> serialise_seq (iter_t begin, size_t n)
{
	U8* seq;
	size_t s = n / 8 + ((n % 8) != 0) + 1;
	
	if (n <= 255)			s += 1;
	else if (n <= 65535)	s += 2;
	else					s += 4;
	
	seq = new U8[s];
	memset (seq, 0, s);
	
	U8* cptr = (U8*) seq;
	
	if (n <= 255)
	{
		*cptr = num_byte;
		cptr++;
		cptr = (U8*) write_num<U8> (cptr, n);
	}
	else if (n <= 65535)
	{
		*cptr = num_word;
		cptr++;
		cptr = (U8*) write_num<U16> (cptr, n);
	}
	else
	{
		*cptr = num_dword;
		cptr++;
		cptr = (U8*) write_num<U32> (cptr, n);
	}
	
	U8 mask = 0x80;
	for (size_t i = 0; i < n; ++i)
	{
		if (*begin) cptr[i/8] |= mask;
		mask = rotate_right (mask);
		++begin;
	}
	
	return std::make_pair (seq, s);
}


/// Deserializa el I8 dado.
template <class cont_t>
void deserialise_I8 (U8 c, U32 n, cont_t& cont)
{
	U8 mask = 0x80;
	for (U32 i = 0; i < n; ++i)
	{
		bool b = !(!(c & mask));
		cont.push_back (b);
		mask = mask >> 1;
	}
}


/// Deserializa el blob dado como una secuencia de bits.
/// Avanza el puntero dado hasta la nueva posicion.
/// Devuelve la cantidad de bits en la secuencia.
template <class cont_t>
U32 deserialise_seq (void** ptr, cont_t& cont)
{
	const U8* cptr = (const U8*) *ptr;
	num_type nt = (num_type) *cptr;
	cptr++;
	
	U32 n;
	if (nt == num_byte)
	{
		U8 nbyte;
		cptr = (const U8*) read_num<U8> (cptr, nbyte);
		n = nbyte;
	}
	else if (nt == num_word)
	{
		U16 nword;
		cptr = (const U8*) read_num<U16> (cptr, nword);
		n = nword;
	}
	else
	{
		U32 ndword;
		cptr = (const U8*) read_num<U32> (cptr, ndword);
		n = ndword;
	}
	
	U32 count = n;
	while (count != 0)
	{
		U32 s = count > 8 ? 8 : count;
		deserialise_I8 (*cptr, s, cont);
		count -= s;
		cptr++;
	}
	
	*ptr = (void*) cptr;
	
	return n;
}


/// Convierte la tabla dada en arrays de alphabeto, bits de codificacion e indices.
template <class T>
void make_arrays (const std::map< T,std::vector<bool> >& table, std::vector<T>& alphabet,
				  std::vector<bool>& alphabits, std::vector<I32>& idxs)
{
	typedef std::map< T,std::vector<bool> > table_t;
	I32 idx = 0;
	BOOST_FOREACH (const typename table_t::value_type& keyval, table)
	{
		alphabet.push_back(keyval.first);
		idxs.push_back(idx);
		for (size_t i = 0; i < keyval.second.size(); ++i) alphabits.push_back(keyval.second[i]);
		idx += keyval.second.size();
	}
	idxs.push_back(idx);
}


/// Convierte los arrays de alphabeto, bits de codificacion e indices dados en una tabla de Huffman.
template <class T>
std::map< T,std::vector<bool> > make_table (const std::vector<T>& alphabet, const std::vector<bool>& alphabits,
											const std::vector<I32>& idxs)
{
	std::map< T,std::vector<bool> > table;
	for (size_t i = 0; i < alphabet.size(); ++i)
	{
		std::vector<bool> bits;
		for (I32 j = idxs[i]; j < idxs[i+1]; ++j) bits.push_back(alphabits[j]);
		table[alphabet[i]] = bits;
	}
	return table;
}


/// Copia la secuencia de enteros dada en el blob dado casteados al tipo T.
template <class T, class iter_t>
void* write_nums (iter_t begin, const iter_t& end, void* blob)
{
	T* ptr = (T*) blob;
	for (; begin != end; ++begin)
	{
		*ptr = (T)(*begin);
		ptr++;
	}
	return (void*) ptr;
}


/// Lee la secuencia de enteros dada del blob dado, representados como valores de tipo T.
template <class T, class cont_t>
void* read_nums (const void* blob, size_t n, cont_t& cont)
{
	const T* ptr = (const T*) blob;
	for (size_t i = 0; i < n; ++i)
	{
		cont.push_back(*ptr);
		ptr++;
	}
	return (void*) ptr;
}


/// Serializa los arrays de alphabeto, bits de codificacion e indices dados.
template <class T>
std::pair<void*,size_t> serialise (std::vector<T>& alphabet, std::vector<bool>& alphabits, std::vector<I32>& idxs)
{
	// Cabecera:
	// [0-3]
	// byte 0:          num_type - El tamanyo del entero que representa el numero de elementos.
	// bytes 1-{2,3,4}: numero de elementos en el alfabeto (tambien es el numero de indices en el vector idxs).
	
	// Serializar los bits del alphabeto.
	std::pair<void*,size_t> serial_alphabits = serialise_seq (alphabits.begin(), alphabits.size());
	
	// Calcular el tipo numerico a utilizar y calcular tamanyo del buffer.
	U32 n = alphabet.size();
	num_type nt;
	size_t s = serial_alphabits.second + sizeof(T) * alphabet.size() + 1;
	if (n <= 255)
	{
		nt = num_byte;
		s = s + idxs.size() + 1;
	}
	else if (n <= 65535)
	{
		nt = num_word;
		s = s + idxs.size() * 2 + 2;
	}
	else
	{
		nt = num_dword;
		s = s + idxs.size() * 4 + 4;
	}
	
	// Crear buffer para los datos.
	// size_t s = serial_alphabits.second + sizeof(T) * alphabet.size() + sizeof(I32) * idxs.size() + sizeof(I32);
	I8* buf = new I8[s];
	
	// Copiar el tamanyo del numero de elementos.
	U8* ntype = (U8*) buf;
	*ntype = nt;
	ntype++;
	
	// Copiar el numero de elementos en el alphabeto.
	I8* nelems = (I8*) ntype;
	switch (nt)
	{
		case num_byte:
			nelems = (I8*) write_num<U8> (nelems, alphabet.size());
			break;
			
		case num_word:
			nelems = (I8*) write_num<U16> (nelems, alphabet.size());
			break;
			
		case num_dword:
			nelems = (I8*) write_num<U32> (nelems, alphabet.size());
			break;
	}
	
	// Copiar el alfabeto en el buffer.
	T* tptr = (T*) nelems;
	memcpy (tptr, &alphabet[0], sizeof(T) * alphabet.size());
	
	// Copiar alphabits serializados.
	I8* cptr = (I8*) tptr + sizeof(T) * alphabet.size();
	memcpy (cptr, serial_alphabits.first, serial_alphabits.second);
	
	// Copiar indices.
	void* vptr = (void*) (cptr + serial_alphabits.second);
	switch (nt)
	{
		case num_byte:
			write_nums<U8> (idxs.begin(), idxs.end(), vptr);
			break;
			
		case num_word:
			write_nums<U16> (idxs.begin(), idxs.end(), vptr);
			break;
		
		case num_dword:
			write_nums<U32> (idxs.begin(), idxs.end(), vptr);
			break;
	}
	
	delete[] (char*)serial_alphabits.first;
	
	return std::make_pair (buf, s);
}


/// Deserializa el blob dado en arrays de alphabeto, bits de codificacion e indices.
/// Retorna el numero de bytes procesados.
template <class T>
I32 deserialise (const void* blob, size_t s, std::vector<T>& alphabet, std::vector<bool>& alphabits,
				  std::vector<I32>& idxs)
{
	// Sacar el tipo numerico del numero de elementos.
	const I8* cptr = (const I8*) blob;
	num_type nt = (num_type) *cptr;
	cptr++;
	
	// Sacar el numero de elementos en el alphabeto (tambien es el numero de indices).
	U32 n;
	switch (nt)
	{
		case num_byte:
			U8 nI8;
			cptr = (const I8*) read_num<U8> (cptr, nI8);
			n = nI8;
			break;
			
		case num_word:
			U16 nword;
			cptr = (const I8*) read_num<U16> (cptr, nword);
			n = nword;
			break;
			
		case num_dword:
			U32 ndword;
			cptr = (const I8*) read_num<U32> (cptr, ndword);
			n = ndword;
			break;
	}
	
	// Sacar el alphabeto.
	const T* tptr = (const T*) cptr;
	for (U32 i = 0; i < n; ++i)
	{
		alphabet.push_back(*tptr);
		tptr++;
	}
	
	// Sacar la secuencia de bits.
	U32 nbits = deserialise_seq ((void**)&tptr, alphabits);
	
	// Sacar los indices.
	const void* vptr = (const void*) tptr;
	switch (nt)
	{
		case num_byte:
			read_nums<U8> (vptr, n+1, idxs);
			return (const I8*) vptr + n+1 - (const I8*) blob;
		
		case num_word:
			read_nums<U16> (vptr, n+1, idxs);
			return (const I8*) vptr + 2 * (n+1) - (const I8*) blob;
		
		case num_dword:
			read_nums<U32> (vptr, n+1, idxs);
			return (const I8*) vptr + 4 * (n+1) - (const I8*) blob;
			
		default:
			break;
	}
	
	throw "deserialise: invalid num type";
}


/// Serializa la tabla de Huffman y la secuencia de bits dados.
template <class T>
std::pair<void*,size_t> serialise (const std::map< T,std::vector<bool> >& table, const std::vector<bool>& code)
{
	std::vector<T>    alphabet;
	std::vector<bool> alphabits;
	std::vector<I32>  idxs;
	make_arrays (table, alphabet, alphabits, idxs);
	
	std::pair<void*,size_t> serial_code = serialise_seq (code.begin(), code.size());
	std::pair<void*,size_t> serial_tree = serialise (alphabet, alphabits, idxs);
	
	size_t s = serial_code.second + serial_tree.second;
	I8* buf = new I8[s];
	
	memcpy (buf, serial_tree.first, serial_tree.second);
	memcpy (buf + serial_tree.second, serial_code.first, serial_code.second);
	
	delete[] (char*)serial_tree.first;
	delete[] (char*)serial_code.first;
	return std::make_pair (buf, s);
}


/// Deserializa el blob dado en una tabla de Huffman y una secuencia de bits.
template <class T>
void deserialise (const void* blob, size_t s,
				  std::map< T,std::vector<bool> >& table, std::vector<bool>& code)
{
	std::vector<T>    alphabet;
	std::vector<bool> alphabits;
	std::vector<I32>  idxs;
	I32 pos = deserialise (blob, s, alphabet, alphabits, idxs);
	table = make_table (alphabet, alphabits, idxs);
	const void* ptr = (const void*) ((const I8*) blob + pos);
	deserialise_seq ((void**)&ptr, code);
}


template <class T, class iter_t>
std::pair<void*,size_t> encode (iter_t begin, const iter_t& end)
{
	HuffmanTree<T> t(begin, end);
	std::map< T,std::vector<bool> > table = t.make_table();
	std::vector<bool> code;
	encode_seq (begin, end, table, code);
	return serialise<T> (table, code);
}


template <class T, class cont_t>
void decode (const void* blob, size_t size, cont_t& cont)
{
	std::map< T,std::vector<bool> > table;
	std::vector<bool> code;
	deserialise (blob, size, table, code);
	decode_seq (code.begin(), code.end(), table, cont);
}

} // namespace huffman end

#endif // _HUFFMAN_H
