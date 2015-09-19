#ifndef _HUFFMAN_TREE_HPP
#define _HUFFMAN_TREE_HPP

#include "HuffmanNode.hpp"
#include <boost/foreach.hpp>
#include <map>
#include <queue>
#include <vector>

#include <cstdio>

namespace huffman {

template <class T, class iter_t>
node<T>* from_sequence (iter_t begin, const iter_t& end);

template <class T>
class HuffmanTree
{
	node<T>* root;
	
	HuffmanTree (const HuffmanTree<T>&);
	HuffmanTree& operator= (const HuffmanTree<T>&);
	
public:

	typedef std::map< T,std::vector<bool> > table;
	
	/// Construye un arbol de Huffman a partir de la secuencia de datos dada.
	/// O(nlogn)
	template <class iter_t>
	HuffmanTree (iter_t begin, const iter_t& end) : root(from_sequence<T>(begin, end)) {}
	
	/// Construye un arbol de Huffman a partir de la tabla de Huffman dada.
	/// O(nlogn)
	HuffmanTree (const std::map< T,std::vector<bool> >& table);
	
	~HuffmanTree () { delete root; }
	
	/// Crea la tabla de Huffman del arbol.
	/// O(nlogn)
	table make_table () const;
	
	/// Decodifica la secuencia de bits dada.
	template <class bits_iter_t, class data_cont_t>
	void decode (bits_iter_t begin, const bits_iter_t& end, data_cont_t& data);
};




/// Construye un mapa de frequencias.
/// O(nlogn)
template <class T, class iter_t>
std::map<T,int> compute_frequencies (iter_t begin, const iter_t& end)
{
	std::map<T,int> freqs;
	for (; begin != end; ++begin) freqs[*begin]++;
	return freqs;
}


/// Funcion para comparar dos nodos utilizando su frecuencia.
/// O(1)
template <class T>
struct nodecmp
{
	bool operator() (const std::pair<node<T>*,int>& n1, const std::pair<node<T>*,int>& n2) const
	{
		return n1.second > n2.second;
	}
};


/// Construye un arbol de Huffman a partir de la secuencia de datos dada.
/// O(nlogn)
template <class T, class iter_t>
node<T>* from_sequence (iter_t begin, const iter_t& end)
{
	// O(nlogn)
	typedef std::map<T,int> frequency_map;
	frequency_map freqs = compute_frequencies<T>(begin, end);
	
	typedef std::pair<node<T>*,int> qelem;
	typedef std::priority_queue< qelem, std::vector<qelem>, nodecmp<T> > nodequeue;
	nodequeue q;
	
	// Create a leaf for every symbol and put it in the queue.
	// O(nlogn)
	BOOST_FOREACH (const typename frequency_map::value_type& keyval, freqs)
	{
		node<T>* n = new node<T>(new T(keyval.first));
		qelem p = std::make_pair(n, keyval.second);
		q.push(p);
	}
	
	// O(nlogn)
	while (q.size() > 1)
	{
		qelem p1 = q.top();
		q.pop();
		qelem p2 = q.top();
		q.pop();
		
		node<T>* n = new node<T>(0, p1.first, p2.first);
		qelem p = std::make_pair(n, p1.second + p2.second);
		q.push(p);
	}
	
	return q.top().first;
}


/// Crea el hijo izquierdo del nodo dado si no existe.
template <class T>
node<T>* make_left (node<T>* n)
{
	if (n->left) return n->left;
	else		 return n->left = new node<T>();
}


/// Crea el hijo derecho del nodo dado si no existe.
template <class T>
node<T>* make_right (node<T>* n)
{
	if (n->right) return n->right;
	else		  return n->right = new node<T>();
}


/// Crea un camino en el arbol con raiz n para colocar el elemento elem dada la secuencia path.
template <class T>
void make_path (node<T>* n, const T& elem, const std::vector<bool>& path)
{
	for (size_t i = 0; i < path.size(); ++i)
	{
		if (path[i] == 0)	n = make_left(n);
		else				n = make_right(n);
	}
	n->elem = new T(elem);
}


/// Construye un arbol de Huffman a partir de la tabla de Huffman dada.
/// O(nlogn)
template <class T>
HuffmanTree<T>::HuffmanTree (const std::map< T,std::vector<bool> >& table)
{
	typedef std::map< T,std::vector<bool> > table_t;
	root = new node<T>;
	// theta(n)
	BOOST_FOREACH (const typename table_t::value_type& keyval, table)
	{
		// O(logn)
		make_path(root, keyval.first, keyval.second);
	}
}


/// Crea la tabla de Huffman del arbol con raiz n.
/// O(nlogn)
template <class T>
void build_table (std::map< T,std::vector<bool> >& table, std::vector<bool>& code, const node<T>* const n)
{
	if (n->is_leaf())
	{
		table[*n->elem] = code; // O(logn)
	}
	else
	{
		code.push_back(0);
		if (n->left)
		{
			build_table(table, code, n->left);
		}
		if (n->right)
		{
			code.pop_back();
			code.push_back(1);
			build_table(table, code, n->right);
		}
		code.pop_back();
	}
}


/// Crea la tabla de Huffman del arbol.
/// O(nlogn)
template <class T>
std::map< T,std::vector<bool> > HuffmanTree<T>::make_table () const
{
	std::map< T, std::vector<bool> > table;
	std::vector<bool> code;
	build_table(table, code, root);
	return table;
}


/// Decodifica la secuencia de bits dada.
/// O(log^2 n)
template <class T> template <class bits_iter_t, class data_cont_t>
void HuffmanTree<T>::decode (bits_iter_t begin, const bits_iter_t& end, data_cont_t& data)
{
	node<T>* n = root;
	for (; begin != end; ++begin)
	{
		if (*begin) n = n->right;
		else        n = n->left;
		
		if (n->is_leaf())
		{
			data.push_back(*n->elem);
			n = root;
		}
	}
}

} // namespace huffman end

#endif // _HUFFMAN_TREE_HPP
