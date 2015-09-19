#include "huffman.h"
#include <cstdlib>
#include <cstdio>

using namespace huffman;

U8 huffman::rotate_right (U8 c)
{
	U8 b = c & 1;
	return (c >> 1) | (b << 7);
}

