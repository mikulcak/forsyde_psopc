#include "buffers.h"
#include "types.h"
#include "process_block.hpp"
#include "jpeg_writer.hpp"

// #define VERBOSE

#define CONCATENATE_HUFFMAN_STEPS_WCEC
// #define JPEG_WRITER_WCEC

#define CONCATENATE_HUFFMAN_STEPS_SECTION 0
#define JPEG_WRITER_SECTION 1

void concatenate_huffman_steps_func(encoded_block *out)
{
	// encoded_block output_block;

	block_output *current_block_output = &(out->current_block_data);
	init_block_output(current_block_output);

	// concatenate_smaller_blocks_input_type incoming_tuple = inp1[0];
	huffman_encoding_output huffman_out;

#pragma ForSyDe begin concatenate_huffman_steps_func
	// collect all six huffman steps and concatenate them to a complete block output
	// Y[0][0]
	// huffman_out = (std::get < 0 > (incoming_tuple))[0];
	receive_token(4, 15, (void *) &huffman_out);
	write_block_output(current_block_output, &huffman_out);
	// Y[0][1]
	// huffman_out = (std::get < 1 > (incoming_tuple))[0];
	receive_token(6, 15, (void *) &huffman_out);
	write_block_output(current_block_output, &huffman_out);
	// Y[1][0]
	// huffman_out = (std::get < 2 > (incoming_tuple))[0];
	receive_token(8, 15, (void *) &huffman_out);
	write_block_output(current_block_output, &huffman_out);
	// Y[1][1]
	// huffman_out = (std::get < 3 > (incoming_tuple))[0];
	receive_token(10, 15, (void *) &huffman_out);
	write_block_output(current_block_output, &huffman_out);
	// Cb
	// huffman_out = (std::get < 4 > (incoming_tuple))[0];
	receive_token(12, 15, (void *) &huffman_out);
	write_block_output(current_block_output, &huffman_out);
	// Cr
	// huffman_out = (std::get < 5 > (incoming_tuple))[0];
	receive_token(14, 15, (void *) &huffman_out);
	write_block_output(current_block_output, &huffman_out);
#pragma ForSyDe end

	// just get the information through to the JPEG writer
	out->block_id = huffman_out.block_id;
	out->height = huffman_out.height;
	out->width = huffman_out.width;

#ifdef VERBOSE
	printf("concatenate_huffman_steps #%d\n", out->block_id);
#endif

	// out[0] = output_block;
}

void write_jpeg_func(encoded_block *input)
{
	int current_block_id = input->block_id;

	int width = input->width;
	int height = input->height;

	if (current_block_id == -1)
	{
		// initialize jpeg output file
#ifdef VERBOSE
		printf("Initializating JPEG file...\n");
#endif
		initialize_jpeg_file(width, height);

	}
	else if (current_block_id == 156)
	{
		// if the image is completely processed, write the EOI marker and stop the simulation
#ifdef VERBOSE
		printf("Writing EOI...\n");
#endif
		finalize_jpeg_file();

		// finished  = 1;
	}
	else
	{
#ifdef VERBOSE
		printf("Writing block %d...\n", current_block_id);
#endif
		write_jpeg_file(&input->current_block_data);
	}

}


void merge_and_write_blocks_func(encoded_block *incoming_block)
{

#ifdef VERBOSE
	printf("Writing block #%d\n", incoming_block->block_id);
#endif

	write_jpeg_func(incoming_block);
}


int main()
{
	initialize_buffers();
	printf("\n\nHej from " ALT_CPU_NAME "!\n");

	//declare communication variables here

	encoded_block concatenate_huffman_steps_output;

	while (1)
	{

		// printf("concatenate_huffman_steps_func\n");
#ifdef CONCATENATE_HUFFMAN_STEPS_WCEC
		PERF_RESET(PERFORMANCE_COUNTER_CPU_3_BASE);
		PERF_START_MEASURING(PERFORMANCE_COUNTER_CPU_3_BASE);
		PERF_BEGIN (PERFORMANCE_COUNTER_CPU_3_BASE, CONCATENATE_HUFFMAN_STEPS_SECTION);
#endif
		concatenate_huffman_steps_func(&concatenate_huffman_steps_output);
#ifdef CONCATENATE_HUFFMAN_STEPS_WCEC
		PERF_END (PERFORMANCE_COUNTER_CPU_3_BASE, CONCATENATE_HUFFMAN_STEPS_SECTION);
		printf("concatenate_huffman_steps WCEC: %u\n", (unsigned int) perf_get_section_time((void *) PERFORMANCE_COUNTER_CPU_3_BASE, CONCATENATE_HUFFMAN_STEPS_SECTION));
#endif

#ifdef JPEG_WRITER_WCEC
		PERF_RESET(PERFORMANCE_COUNTER_CPU_3_BASE);
		PERF_START_MEASURING(PERFORMANCE_COUNTER_CPU_3_BASE);
		PERF_BEGIN (PERFORMANCE_COUNTER_CPU_3_BASE, JPEG_WRITER_SECTION);
#endif
		merge_and_write_blocks_func(&concatenate_huffman_steps_output);
#ifdef JPEG_WRITER_WCEC
		PERF_END (PERFORMANCE_COUNTER_CPU_3_BASE, JPEG_WRITER_SECTION);
		printf("jpeg_writer WCEC: %u\n", (unsigned int) perf_get_section_time((void *) PERFORMANCE_COUNTER_CPU_3_BASE, JPEG_WRITER_SECTION));
#endif

		//receive_token(sender, receiver, pointer_to_data);
		//call process functions here
		//send_token(sender, receiver, pointer_to_data);
	}
}
