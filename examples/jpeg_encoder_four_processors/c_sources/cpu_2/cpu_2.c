#include "buffers.h"

#include "dct.hpp"
#include "huffman_encode.hpp"
#include "tables.h"
#include "types.h"

#define VERBOSE

void dct_func(smaller_block *out, smaller_block *inp1)
{
	// smaller_block output_block;
	// pass through all picture information to avoid having to create an extra channel for it
	out->is_cb = inp1->is_cb;
	out->is_cr = inp1->is_cr;
	out->block_id = inp1->block_id;
	out->width = inp1->width;
	out->height = inp1->height;

#ifdef VERBOSE
	printf("dct block #%d\n", inp1->block_id);
#endif

#pragma ForSyDe begin dct_func
	dct3(inp1->content, out->content);
#pragma ForSyDe end

	// out[0] = output_block;
}

void huffman_encode_func(huffman_encoding_output *out, smaller_block *inp1)
{
	// huffman_encoding_output huffman_step_output;

	out->width = inp1->width;
	out->height = inp1->height;
	out->block_id = inp1->block_id;

#ifdef VERBOSE
	printf("huffman_encoding block #%d\n", inp1->block_id);
#endif


#pragma ForSyDe begin huffman_encode_func

	if (inp1->is_cb == 1)
	{
		huffman_encode(HUFFMAN_CTX_Cb, (short *) inp1->content, out);
		// printf("Cb: Got %u bits form Huffman step\n", out->bit_position);
	}
	else if (inp1->is_cr == 1)
	{
		huffman_encode(HUFFMAN_CTX_Cr, (short *) inp1->content, out);
		// printf("Cr: Got %u bits form Huffman step\n", out->bit_position);
	}
	else if ((inp1->is_cr == 0) && (inp1->is_cb == 0))
	{
		huffman_encode(HUFFMAN_CTX_Y, (short *) inp1->content, out);
		// printf("Y: Got %u bits form Huffman step\n", out->bit_position);
	}
#pragma ForSyDe end

	// out[0] = huffman_step_output;
}


int main()
{
	initialize_buffers();
	printf("\n\nHej from " ALT_CPU_NAME "!\n");

	//declare communication variables here

	// three dct and huffman_encode steps here
	smaller_block dct_y01_input;
	smaller_block dct_y10_input;
	smaller_block dct_y11_input;

	smaller_block dct_y01_output;
	smaller_block dct_y10_output;
	smaller_block dct_y11_output;

	huffman_encoding_output huffman_y01_output;
	huffman_encoding_output huffman_y10_output;
	huffman_encoding_output huffman_y11_output;


	while (1)
	{

		receive_token(2, 9, &dct_y01_input);
		dct_func(&dct_y01_output, &dct_y01_input);

		receive_token(2, 11, &dct_y10_input);
		dct_func(&dct_y10_output, &dct_y10_input);

		receive_token(2, 13, &dct_y11_input);
		dct_func(&dct_y11_output, &dct_y11_input);

		huffman_encode_func(&huffman_y01_output, &dct_y01_output);
		send_token(10, 15, &huffman_y01_output);

		huffman_encode_func(&huffman_y10_output, &dct_y10_output);
		send_token(12, 15, &huffman_y10_output);

		huffman_encode_func(&huffman_y11_output, &dct_y11_output);
		send_token(14, 15, &huffman_y11_output);

		//receive_token(sender, receiver, pointer_to_data);
		//call process functions here
		//send_token(sender, receiver, pointer_to_data);
	}
}
