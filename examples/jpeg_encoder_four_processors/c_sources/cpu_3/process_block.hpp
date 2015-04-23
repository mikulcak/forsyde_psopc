/*
 * File:   process_block.hpp
 * Author: marcus
 *
 * Created on March 28, 2013, 5:50 PM
 */

#ifndef PROCESS_BLOCK_HPP
#define  PROCESS_BLOCK_HPP

//extern huffman_t huffman_ctx[3];

#define  HUFFMAN_CTX_Y  &huffman_ctx[0]
#define  HUFFMAN_CTX_Cb &huffman_ctx[1]
#define  HUFFMAN_CTX_Cr &huffman_ctx[2]

void print_block_output(block_output *block)
{
	printf("\t%u\n", block->bit_position);
	unsigned int i;
	for (i = 0; i < 48; i++)
	{
		if ((block->content[i] == 0) & (((i + 1) * 32) < block->bit_position))
		{
			printf("error\n");
		}
		printf("\t%u ... %u: %x\n", ((i + 1) * 32) - 1, i * 32, block->content[i]);
	}
}

void write_block_output(block_output *output, huffman_encoding_output *huffman_output)
{
#ifdef VERBOSE
	printf("Adding %u bits to the buffer of block %u\n", huffman_output->bit_position, huffman_output->block_id);
#endif
	// print_block_output(output);

	unsigned int i, current_position, test_count = 0;

	current_position = huffman_output->bit_position;

	// first shift whole words to the left
	while (current_position >= 32)
	{
		for (i = 47; i > 0; i--)
		{
			output->content[i] = output->content[i - 1];
		}
		current_position -= 32;
		test_count++;
	}

	for (i = 0; i < test_count; i++)
	{
		output->content[i] = 0;
	}
	if (current_position != 0)
	{
		// then shift the remaining individual bits
		unsigned int shift_amount_inversed = 32 - current_position;

		for (i = 47; i > 0; i--)
		{
			output->content[i] = (output->content[i - 1] >> shift_amount_inversed) | (output->content[i] << current_position);
		}

		output->content[0] = output->content[0] << current_position;
	}

	// now that all bits are shifted, concatenate both arrays
	for (i = 0; i < 8; i++)
	{
		output->content[i] = output->content[i] | huffman_output->content[i];
	}

	output->bit_position += huffman_output->bit_position;

	// print_block_output(output);
}

void init_block_output(block_output *output)
{
	output->bit_position = 0;
	memset(output->content, 0, 48 * sizeof(int));
}

#endif   /* PROCESS_BLOCK_HPP */

