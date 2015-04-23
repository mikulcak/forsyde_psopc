/* 
 * File:   huffman_encode.hpp
 * Author: marcus
 *
 * Created on March 28, 2013, 5:52 PM
 */

#ifndef HUFFMAN_ENCODE_HPP
#define	HUFFMAN_ENCODE_HPP

#include "types.h"
#include "tables.h"
//#include "process_block.hpp"

#define QTAB_DIV	512

unsigned int block_number;
unsigned int overall_sum_of_bits = 0;

/******************************************************************************
 **  quantize
 **  --------------------------------------------------------------------------
 **  DCT coefficients quantization to discard weak high frequencies.
 **  To avoid division function uses quantization coeffs amplified by 2^10 and
 **  then shifts the product by 10 bits to the right.
 **  To make this operation a bit faster some tricks are used but it is just
 **    return round(data[i]/qt0[i]);
 **  
 **  ARGUMENTS:
 **      data    - DCT freq. component;
 **      qt      - quantization coefficient;
 **
 **  RETURN: quantized freq. component.
 ******************************************************************************/
short quantize(const short data, const unsigned char qt)
{
	return (data * qt + QTAB_DIV - ((unsigned) data >> 31)) >> 10;
}

/******************************************************************************
 **  huffman_bits
 **  --------------------------------------------------------------------------
 **  Converts amplitude into the representation suitable for Huffman encoder.
 **  
 **  ARGUMENTS:
 **      value    - DCT amplitude;
 **
 **  RETURN: huffman bits
 ******************************************************************************/
unsigned huffman_bits(const short value)
{
	return value - ((unsigned) value >> 31);
}

/******************************************************************************
 **  huffman_magnitude
 **  --------------------------------------------------------------------------
 **  Calculates magnitude of an amplitude - the number of bits that are enough
 **  to represent given value.
 **  
 **  ARGUMENTS:
 **      value    - DCT amplitude;
 **
 **  RETURN: magnitude
 ******************************************************************************/
unsigned huffman_magnitude(const short value)
{
	unsigned x = (value < 0) ? -value : value;
	unsigned m = 0;

	while (x >> m) ++m;

	return m;
}

void print_huffman_output(huffman_encoding_output* huffman_output)
{
	printf("\t%u\n", huffman_output->bit_position);
	unsigned int i;
	for (i = 0; i < 8; i++)
	{
		printf("\t%x\n", huffman_output->content[i]);
	}
}

void write_huffman_output(huffman_encoding_output* output, unsigned int bits_to_write, unsigned int number_of_bits_to_write)
{
#ifdef VERBOSE
	printf("Adding %u bits to the buffer of block %u\n", number_of_bits_to_write, block_number);
#endif

	unsigned int i;

	unsigned int shift_amount_inversed = 32 - number_of_bits_to_write;

	bits_to_write = (bits_to_write << shift_amount_inversed) >> shift_amount_inversed;

//	printf("Adding %u bits to the buffer: %x\n", number_of_bits_to_write, bits_to_write);


	for (i = 7; i > 0; i--)
	{
		output->content[i] = (output->content[i - 1] >> shift_amount_inversed) | (output->content[i] << number_of_bits_to_write);
	}

	//	printf("BTW: %x\n", bits_to_write);


	//	pbb->buf = (pbb->buf << nbits) | (bits & ((1 << nbits) - 1));

//	bits_to_write = (bits_to_write << shift_amount_inversed) >> shift_amount_inversed;

	//	output->content[0] = (output->content[0] << number_of_bits_to_write) | (bits_to_write & ((1 << number_of_bits_to_write) - 1));
	output->content[0] = (output->content[0] << number_of_bits_to_write) | bits_to_write;

	output->bit_position += number_of_bits_to_write;

//	print_huffman_output(output);

	//	printf("BP: %u\n", output->bit_position);
	//	for (i = 0; i < 8; i++)
	//	{
	//		printf("WHO: %x\n", output->content[i]);
	//	}
	//	printf("\n");
}

void init_huffman_output(huffman_encoding_output* output)
{
	output->bit_position = 0;
	memset(output->content, 0, 8 * sizeof (int));
}

void huffman_encode(huffman_t * const ctx, const short data[], huffman_encoding_output* current_output)
{

	// initialize the current output block
	init_huffman_output(current_output);


	unsigned magn, bits;
	unsigned zerorun, i;
	short diff;

	unsigned int sum_of_bits = 0;

	short dc = quantize(data[0], ctx->qtable[0]);
//	printf("dc: %d\n", dc);
	// difference between old and new DC
	diff = dc - ctx->dc;
	ctx->dc = dc;
	//	printf("diff: %i\n", diff);

	bits = huffman_bits(diff);
	magn = huffman_magnitude(diff);


	//	writebits(&bitbuf, ctx->hdcbit[magn], ctx->hdclen[magn]);

	write_huffman_output(current_output, ctx->hdcbit[magn], ctx->hdclen[magn]);
	//	printf("%x %x\n", ctx->hdcbit[magn], ctx->hdclen[magn]);
	//	printf("%u\n", ctx->hdclen[magn]);

	//	printf("writing %u bits\n", ctx->hdclen[magn]);

	sum_of_bits += ctx->hdclen[magn];
	//	writebits(&bitbuf, bits, magn);
	write_huffman_output(current_output, bits, magn);
	//	printf("%u\n", magn);

	sum_of_bits += magn;

	for (zerorun = 0, i = 1; i < 64; i++)
	{
		const short ac = quantize(data[zig[i]], ctx->qtable[zig[i]]);
		//		printf("data zig: %i\n", data[zig[i]]);
		//		printf("ctx table zig: %i\n", ctx->qtable[zig[i]]);		
		//		printf("ac: %i\n", ac);

		if (ac)
		{
			while (zerorun >= 16)
			{
				zerorun -= 16;
				// ZRL
				//				writebits(&bitbuf, ctx->hacbit[15][0], ctx->haclen[15][0]);
				write_huffman_output(current_output, ctx->hacbit[15][0], ctx->haclen[15][0]);
				//				printf("%u\n", ctx->haclen[15][0]);

				sum_of_bits += ctx->haclen[15][0];
			}

			bits = huffman_bits(ac);
			magn = huffman_magnitude(ac);

			//			writebits(&bitbuf, ctx->hacbit[zerorun][magn], ctx->haclen[zerorun][magn]);
			write_huffman_output(current_output, ctx->hacbit[zerorun][magn], ctx->haclen[zerorun][magn]);
			//			printf("%u\n", ctx->haclen[zerorun][magn]);

			sum_of_bits += ctx->haclen[zerorun][magn];
			//			writebits(&bitbuf, bits, magn);
			write_huffman_output(current_output, bits, magn);
			//			printf("%u\n", magn);
			sum_of_bits += magn;

			zerorun = 0;
		}
		else zerorun++;
	}

	if (zerorun)
	{ // EOB - End Of Block
		//		writebits(&bitbuf, ctx->hacbit[0][0], ctx->haclen[0][0]);
		write_huffman_output(current_output, ctx->hacbit[0][0], ctx->haclen[0][0]);
		//		printf("%u\n", ctx->haclen[0][0]);

		sum_of_bits += ctx->haclen[0][0];
	}
	
//	printf("\n");
	//	overall_sum_of_bits += sum_of_bits;
	//	printf("Bits written in this block: %u, overall: %u\n", current_output.bit_position, overall_sum_of_bits);

	//	return current_output;
}


#endif	/* HUFFMAN_ENCODE_HPP */

