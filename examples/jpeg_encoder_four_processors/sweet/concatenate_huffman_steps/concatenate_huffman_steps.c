/*
   concatenate_huffman_steps.c:
      the function concatenating the six individual huffman_encode steps,
      wrapped together in a single ForSyDe process
*/

#include "types.h"

huffman_encoding_output input;

void write_block_output(block_output *output, huffman_encoding_output *huffman_output)
{
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
}

int main()
{
	block_output output;
	int i;
	for (i = 0; i < 6; i++)
	{
		write_block_output(&output, &input);
	}
	return 1;
}

