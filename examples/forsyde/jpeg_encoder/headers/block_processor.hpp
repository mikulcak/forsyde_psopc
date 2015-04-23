/*
 * block_processor.hpp

 *
 *  Created on: May 11, 2013
 *      Author: marcus
 */

// turn off the reorder warning when using zipN and unzipN
// TODO: fix the warning instead of ignoring it
#pragma GCC diagnostic ignored "-Wreorder"

#ifndef BLOCK_PROCESSOR_HPP_
#define BLOCK_PROCESSOR_HPP_

#include "types.h"
#include "tables.h"
#include "process_block.hpp"

#include "forsyde.hpp"

using namespace ForSyDe;
using namespace ForSyDe::SDF;

typedef std::tuple<std::vector<smaller_block>, std::vector<smaller_block>, std::vector<smaller_block>, std::vector<smaller_block>,
		std::vector<smaller_block>, std::vector<smaller_block> > color_conversion_output_type;

typedef unzipN<smaller_block, smaller_block, smaller_block, smaller_block, smaller_block, smaller_block> block_unzipper;

typedef std::tuple<std::vector<huffman_encoding_output>, std::vector<huffman_encoding_output>, std::vector<huffman_encoding_output>,
		std::vector<huffman_encoding_output>, std::vector<huffman_encoding_output>, std::vector<huffman_encoding_output>> concatenate_smaller_blocks_input_type;

typedef zipN<huffman_encoding_output, huffman_encoding_output, huffman_encoding_output, huffman_encoding_output, huffman_encoding_output,
		huffman_encoding_output> encoded_blocks_zipper;

std::ostream &operator <<(std::ostream &os, const bitmap_reader_output &obj)
{
	return os;
}

std::ostream &operator <<(std::ostream &os, const encoded_block &obj)
{
	return os;
}

std::ostream &operator <<(std::ostream &os, const smaller_block &obj)
{
	return os;
}

std::ostream &operator <<(std::ostream &os, const huffman_encoding_output &obj)
{
	return os;
}

void color_conversion_func(std::vector<color_conversion_output_type> &out, const std::vector<bitmap_reader_output> &inp1)
{
	auto current_color_block = inp1[0].color_block;
	auto current_width = inp1[0].width;
	auto current_height = inp1[0].height;
	auto current_block_id = inp1[0].block_id;

	if (current_block_id != -1)
	{
		std::cout << "Processing block " << current_block_id << std::endl;
	}

	// four blocks of 8x8 Y
	smaller_block y_blocks[2][2];

	// Cb8x8[8][8] block
	smaller_block cb_block;
	cb_block.is_cb = 1;
	cb_block.is_cr = 0;
	cb_block.width = current_width;
	cb_block.height = current_height;
	cb_block.block_id = current_block_id;

	// Cr8x8[8][8] block
	smaller_block cr_block;
	cr_block.is_cb = 0;
	cr_block.is_cr = 1;
	cr_block.width = current_width;
	cr_block.height = current_height;
	cr_block.block_id = current_block_id;

#pragma ForSyDe begin color_conversion_func

	unsigned int i, j, r, c;

	// getting four 8x8 Y-blocks
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
			for (r = 0; r < 8; r++)
			{
				for (c = 0; c < 8; c++)
				{
					const unsigned rr = (i << 3) + r;
					const unsigned cc = (j << 3) + c;

					const color R = current_color_block[rr][cc].Red;
					const color G = current_color_block[rr][cc].Green;
					const color B = current_color_block[rr][cc].Blue;

					// converting RGB into Y (luminance)
					y_blocks[i][j].content[r][c] = RGB2Y(R, G, B) - 128;

					y_blocks[i][j].is_cb = 0;
					y_blocks[i][j].is_cr = 0;
					y_blocks[i][j].width = current_width;
					y_blocks[i][j].height = current_height;
					y_blocks[i][j].block_id = current_block_id;
				}
			}
		}
	}

	// one block of 8x8 Cr
	short Cb8x8[8][8];
	// one block of 8x8 Cb
	short Cr8x8[8][8];

	// create subsampled Cb and Cr blocks
	subsample(current_color_block, Cb8x8, Cr8x8);

	for (r = 0; r < 8; r++)
	{
		for (c = 0; c < 8; c++)
		{
			cb_block.content[r][c] = Cb8x8[r][c];
		}
	}
	for (r = 0; r < 8; r++)
	{
		for (c = 0; c < 8; c++)
		{
			cr_block.content[r][c] = Cr8x8[r][c];
		}
	}
#pragma ForSyDe end

	// I had trouble thinking of an uglier solution, but this will do!
	std::vector<smaller_block> y00_vector;
	y00_vector.push_back(y_blocks[0][0]);

	std::vector<smaller_block> y01_vector;
	y01_vector.push_back(y_blocks[0][1]);

	std::vector<smaller_block> y10_vector;
	y10_vector.push_back(y_blocks[1][0]);

	std::vector<smaller_block> y11_vector;
	y11_vector.push_back(y_blocks[1][1]);

	std::vector<smaller_block> cb_vector;
	cb_vector.push_back(cb_block);

	std::vector<smaller_block> cr_vector;
	cr_vector.push_back(cr_block);

	// pack the six arrays into the output tuple of vectors
	auto output_tuple = std::make_tuple(y00_vector, y01_vector, y10_vector, y11_vector, cb_vector, cr_vector);
	out[0] = output_tuple;
}

void dct_func(std::vector<smaller_block> &out, const std::vector<smaller_block> &inp1)
{
	smaller_block output_block;
	// pass through all picture information to avoid having to create an extra channel for it
	output_block.is_cb = inp1[0].is_cb;
	output_block.is_cr = inp1[0].is_cr;
	output_block.block_id = inp1[0].block_id;
	output_block.width = inp1[0].width;
	output_block.height = inp1[0].height;

#pragma ForSyDe begin dct_func
	dct3(inp1[0].content, output_block.content);
#pragma ForSyDe end

	out[0] = output_block;
}

void huffman_encode_func(std::vector<huffman_encoding_output> &out, const std::vector<smaller_block> &inp1)
{
	huffman_encoding_output huffman_step_output;

	huffman_step_output.width = inp1[0].width;
	huffman_step_output.height = inp1[0].height;
	huffman_step_output.block_id = inp1[0].block_id;

#pragma ForSyDe begin huffman_encode_func

	if (inp1[0].is_cb == 1)
	{
		huffman_encode(HUFFMAN_CTX_Cb, (short*) inp1[0].content, &huffman_step_output);
		printf("Cb: Got %u bits form Huffman step\n", huffman_step_output.bit_position);
	}
	else if (inp1[0].is_cr == 1)
	{
		huffman_encode(HUFFMAN_CTX_Cr, (short*) inp1[0].content, &huffman_step_output);
		printf("Cr: Got %u bits form Huffman step\n", huffman_step_output.bit_position);
	}
	else if ((inp1[0].is_cr == 0) && (inp1[0].is_cb == 0))
	{
		huffman_encode(HUFFMAN_CTX_Y, (short*) inp1[0].content, &huffman_step_output);
		printf("Y: Got %u bits form Huffman step\n", huffman_step_output.bit_position);
	}
#pragma ForSyDe end

	out[0] = huffman_step_output;
}

void concatenate_huffman_steps_func(std::vector<encoded_block> &out, const std::vector<concatenate_smaller_blocks_input_type> &inp1)
{
	encoded_block output_block;

	block_output* current_block_output = &(output_block.current_block_data);
	init_block_output(current_block_output);

	concatenate_smaller_blocks_input_type incoming_tuple = inp1[0];
	huffman_encoding_output huffman_out;

#pragma ForSyDe begin concatenate_huffman_steps_func
	// collect all six huffman steps and concatenate them to a complete block output
	// Y[0][0]
	huffman_out = (std::get < 0 > (incoming_tuple))[0];
	write_block_output(current_block_output, &huffman_out);
	// Y[0][1]
	huffman_out = (std::get < 1 > (incoming_tuple))[0];
	write_block_output(current_block_output, &huffman_out);
	// Y[1][0]
	huffman_out = (std::get < 2 > (incoming_tuple))[0];
	write_block_output(current_block_output, &huffman_out);
	// Y[1][1]
	huffman_out = (std::get < 3 > (incoming_tuple))[0];
	write_block_output(current_block_output, &huffman_out);
	// Cb
	huffman_out = (std::get < 4 > (incoming_tuple))[0];
	write_block_output(current_block_output, &huffman_out);
	// Cr
	huffman_out = (std::get < 5 > (incoming_tuple))[0];
	write_block_output(current_block_output, &huffman_out);
#pragma ForSyDe end

	// just get the information through to the JPEG writer
	output_block.block_id = huffman_out.block_id;
	output_block.height = huffman_out.height;
	output_block.width = huffman_out.width;

	out[0] = output_block;
}

SC_MODULE(block_processor)
{
public:

	SDF_in<bitmap_reader_output> incoming_bitmap_block;
	SDF_out<encoded_block> outgoing_encoded_block;

	SDF2SDF<color_conversion_output_type> color_conversion_output;

	SDF2SDF<concatenate_smaller_blocks_input_type> concatenated_block;

	SDF2SDF<smaller_block> cc_to_dct_1;
	SDF2SDF<smaller_block> cc_to_dct_2;
	SDF2SDF<smaller_block> cc_to_dct_3;
	SDF2SDF<smaller_block> cc_to_dct_4;
	SDF2SDF<smaller_block> cc_to_dct_5;
	SDF2SDF<smaller_block> cc_to_dct_6;

	SDF2SDF<smaller_block> dct_to_huffman_1;
	SDF2SDF<smaller_block> dct_to_huffman_2;
	SDF2SDF<smaller_block> dct_to_huffman_3;
	SDF2SDF<smaller_block> dct_to_huffman_4;
	SDF2SDF<smaller_block> dct_to_huffman_5;
	SDF2SDF<smaller_block> dct_to_huffman_6;

	SDF2SDF<huffman_encoding_output> huffman_to_conc_1;
	SDF2SDF<huffman_encoding_output> huffman_to_conc_2;
	SDF2SDF<huffman_encoding_output> huffman_to_conc_3;
	SDF2SDF<huffman_encoding_output> huffman_to_conc_4;
	SDF2SDF<huffman_encoding_output> huffman_to_conc_5;
	SDF2SDF<huffman_encoding_output> huffman_to_conc_6;

	SC_CTOR(block_processor)
	{
		make_comb("color_conversion_comb", color_conversion_func, 1, 1, color_conversion_output, incoming_bitmap_block);

		std::vector<unsigned int> unzip_blocks_rates =
		{ 1, 1, 1, 1, 1, 1 };

		auto unzip_blocks = new block_unzipper("block_unzipper", unzip_blocks_rates);
		unzip_blocks->iport1(color_conversion_output);
		std::get < 0 > (unzip_blocks->oport)(cc_to_dct_1);
		std::get < 1 > (unzip_blocks->oport)(cc_to_dct_2);
		std::get < 2 > (unzip_blocks->oport)(cc_to_dct_3);
		std::get < 3 > (unzip_blocks->oport)(cc_to_dct_4);
		std::get < 4 > (unzip_blocks->oport)(cc_to_dct_5);
		std::get < 5 > (unzip_blocks->oport)(cc_to_dct_6);

		make_comb("dct_cr", dct_func, 1, 1, dct_to_huffman_1, cc_to_dct_1);
		make_comb("dct_cb", dct_func, 1, 1, dct_to_huffman_2, cc_to_dct_2);
		make_comb("dct_y00", dct_func, 1, 1, dct_to_huffman_3, cc_to_dct_3);
		make_comb("dct_y01", dct_func, 1, 1, dct_to_huffman_4, cc_to_dct_4);
		make_comb("dct_y10", dct_func, 1, 1, dct_to_huffman_5, cc_to_dct_5);
		make_comb("dct_y11", dct_func, 1, 1, dct_to_huffman_6, cc_to_dct_6);

		make_comb("huffman_cr", huffman_encode_func, 1, 1, huffman_to_conc_1, dct_to_huffman_1);
		make_comb("huffman_cb", huffman_encode_func, 1, 1, huffman_to_conc_2, dct_to_huffman_2);
		make_comb("huffman_y00", huffman_encode_func, 1, 1, huffman_to_conc_3, dct_to_huffman_3);
		make_comb("huffman_y01", huffman_encode_func, 1, 1, huffman_to_conc_4, dct_to_huffman_4);
		make_comb("huffman_y10", huffman_encode_func, 1, 1, huffman_to_conc_5, dct_to_huffman_5);
		make_comb("huffman_y11", huffman_encode_func, 1, 1, huffman_to_conc_6, dct_to_huffman_6);

		std::vector<unsigned> zip_blocks_rates =
		{ 1, 1, 1, 1, 1, 1 };

		auto zip_blocks = new encoded_blocks_zipper("block_zipper", zip_blocks_rates);
		std::get < 0 > (zip_blocks->iport)(huffman_to_conc_1);
		std::get < 1 > (zip_blocks->iport)(huffman_to_conc_2);
		std::get < 2 > (zip_blocks->iport)(huffman_to_conc_3);
		std::get < 3 > (zip_blocks->iport)(huffman_to_conc_4);
		std::get < 4 > (zip_blocks->iport)(huffman_to_conc_5);
		std::get < 5 > (zip_blocks->iport)(huffman_to_conc_6);
		zip_blocks->oport1(concatenated_block);

		make_comb("concatenate_steps", concatenate_huffman_steps_func, 1, 1, outgoing_encoded_block, concatenated_block);
	}

};
#endif /* BLOCK_PROCESSOR_HPP_ */
