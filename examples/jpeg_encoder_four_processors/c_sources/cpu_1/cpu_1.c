#include "buffers.h"

#include "dct.hpp"
#include "huffman_encode.hpp"
#include "tables.h"
#include "types.h"

// #define VERBOSE

// #define DCT_WCEC
#define HUFFMAN_WCEC

#define DCT_SECTION 0
#define HUFFMAN_SECTION 1

void dct_func(smaller_block *out, smaller_block *inp1)
{
	// smaller_block output_block;
	// pass through all picture information to avoid having to create an extra channel for it
	out->is_cb = inp1->is_cb;
	out->is_cr = inp1->is_cr;
	out->block_id = inp1->block_id;
	out->width = inp1->width;
	out->height = inp1->height;

	// int i;
	// for (i = 0; i < sizeof(smaller_block), i++)
	// {
	// printf("%d\n", *(inp1 + i));
	// }



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
	smaller_block dct_cr_input;
	smaller_block dct_cb_input;
	smaller_block dct_y00_input;

	smaller_block dct_cr_output;
	smaller_block dct_cb_output;
	smaller_block dct_y00_output;

	huffman_encoding_output huffman_cr_output;
	huffman_encoding_output huffman_cb_output;
	huffman_encoding_output huffman_y00_output;

	while (1)
	{

		receive_token(2, 3, &dct_cr_input);


#ifdef DCT_WCEC
		PERF_RESET(PERFORMANCE_COUNTER_CPU_1_BASE);
		PERF_START_MEASURING(PERFORMANCE_COUNTER_CPU_1_BASE);
		PERF_BEGIN (PERFORMANCE_COUNTER_CPU_1_BASE, DCT_SECTION);
#endif
		dct_func(&dct_cr_output, &dct_cr_input);
#ifdef DCT_WCEC
		PERF_END (PERFORMANCE_COUNTER_CPU_1_BASE, DCT_SECTION);
		printf("dct WCEC: %u\n", (unsigned int) perf_get_section_time((void *) PERFORMANCE_COUNTER_CPU_1_BASE, DCT_SECTION));
#endif

		receive_token(2, 5, &dct_cb_input);
		dct_func(&dct_cb_output, &dct_cb_input);

		receive_token(2, 7, &dct_y00_input);
		dct_func(&dct_y00_output, &dct_y00_input);

#ifdef HUFFMAN_WCEC
		PERF_RESET(PERFORMANCE_COUNTER_CPU_1_BASE);
		PERF_START_MEASURING(PERFORMANCE_COUNTER_CPU_1_BASE);
		PERF_BEGIN (PERFORMANCE_COUNTER_CPU_1_BASE, HUFFMAN_SECTION);
#endif

		huffman_encode_func(&huffman_cr_output, &dct_cr_output);
#ifdef HUFFMAN_WCEC
		PERF_END (PERFORMANCE_COUNTER_CPU_1_BASE, HUFFMAN_SECTION);
		printf("huffman_encode WCEC: %u\n", (unsigned int) perf_get_section_time((void *) PERFORMANCE_COUNTER_CPU_1_BASE, HUFFMAN_SECTION));
#endif
		send_token(4, 15, &huffman_cr_output);

		huffman_encode_func(&huffman_cb_output, &dct_cb_output);
		send_token(6, 15, &huffman_cb_output);

		huffman_encode_func(&huffman_y00_output, &dct_y00_output);
		send_token(8, 15, &huffman_y00_output);

		//receive_token(sender, receiver, pointer_to_data);
		//call process functions here
		//send_token(sender, receiver, pointer_to_data);
	}
}
