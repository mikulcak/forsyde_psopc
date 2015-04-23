#include "huffman_encode.h"
#include "tables.h"
#include "types.h"

#define	HUFFMAN_CTX_Y	&huffman_ctx[0]
#define	HUFFMAN_CTX_Cb	&huffman_ctx[1]
#define	HUFFMAN_CTX_Cr	&huffman_ctx[2]


short content_to_be_encoded[8][8];


int main()
{

	huffman_encoding_output output;

	huffman_encode(HUFFMAN_CTX_Cb, (short *) content_to_be_encoded, &output);
	return 1;
}

