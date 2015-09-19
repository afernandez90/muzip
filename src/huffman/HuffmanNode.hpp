#ifndef _HUFFMAN_NODE_HPP
#define _HUFFMAN_NODE_HPP

namespace huffman {

template <class T>
struct node
{
	T* elem;
	node* left;
	node* right;
	
	node (T* e = 0, node<T>* l = 0, node<T>* r = 0) : elem(e), left(l), right(r) {}
	
	~node ()
	{
		if (left)  delete left;
		if (right) delete right;
		if (elem)  delete elem;
	}
	
	bool is_leaf () const { return left == 0 && right == 0; }
};

}

#endif // _HUFFMAN_NODE_HPP
