/* 
 * File:   jpeg_writer.hpp
 * Author: marcus
 *
 * Created on March 29, 2013, 10:39 PM
 */

#ifndef JPEG_WRITER_HPP
#define	JPEG_WRITER_HPP

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include "types.h"
#include "tables.h"

unsigned int jpeg_output_file;

unsigned int jpgn = 0;

static unsigned char jpgbuff[256];
unsigned int byte_counter = 0;

void write_jpeg(const unsigned char buff[], const unsigned size)
{

	write(jpeg_output_file, buff, size);
}

static void writebyte(const unsigned char b)
{

//	if (byte_counter > 627)
//	{
//		printf("%04u: Writing byte %x\n", byte_counter, b);
//	}
	byte_counter++;
	jpgbuff[jpgn++] = b;

	if (jpgn == sizeof(jpgbuff))
	{
		jpgn = 0;
		write_jpeg(jpgbuff, sizeof(jpgbuff));
	}
}

static void writeword(const unsigned short w)
{
	writebyte(w >> 8);
	writebyte(w);
}

static void write_APP0info(void)
{
	writeword(0xFFE0); //marker
	writeword(16); //length
	writebyte('J');
	writebyte('F');
	writebyte('I');
	writebyte('F');
	writebyte(0);
	writebyte(1); //versionhi
	writebyte(1); //versionlo
	writebyte(0); //xyunits
	writeword(1); //xdensity
	writeword(1); //ydensity
	writebyte(0); //thumbnwidth
	writebyte(0); //thumbnheight
}

// should set width and height before writing

static void write_SOF0info(const short height, const short width)
{
	writeword(0xFFC0); //marker
	writeword(17); //length
	writebyte(8); //precision
	writeword(height); //height
	writeword(width); //width
	writebyte(3); //nrofcomponents
	writebyte(1); //IdY
	writebyte(0x22); //HVY, 4:2:0 subsampling
	writebyte(0); //QTY
	writebyte(2); //IdCb
	writebyte(0x11); //HVCb
	writebyte(1); //QTCb
	writebyte(3); //IdCr
	writebyte(0x11); //HVCr
	writebyte(1); //QTCr
}

static void write_SOSinfo(void)
{
	writeword(0xFFDA); //marker
	writeword(12); //length
	writebyte(3); //nrofcomponents
	writebyte(1); //IdY
	writebyte(0); //HTY
	writebyte(2); //IdCb
	writebyte(0x11); //HTCb
	writebyte(3); //IdCr
	writebyte(0x11); //HTCr
	writebyte(0); //Ss
	writebyte(0x3F); //Se
	writebyte(0); //Bf
}

static void write_DQTinfo(void)
{
	unsigned i;

	writeword(0xFFDB);
	writeword(132);
	writebyte(0);

	for (i = 0; i < 64; i++)
		writebyte(((unsigned char*) qtable_0_lum)[zig[i]]); // zig-zag order

	writebyte(1);

	for (i = 0; i < 64; i++)
		writebyte(((unsigned char*) qtable_0_chrom)[zig[i]]); // zig-zag order
}

static void write_DHTinfo(void)
{
	unsigned i;

	writeword(0xFFC4); // marker
	writeword(0x01A2); // length

	writebyte(0); // HTYDCinfo
	for (i = 0; i < 16; i++)
		writebyte(std_dc_luminance_nrcodes[i + 1]);
	for (i = 0; i < 12; i++)
		writebyte(std_dc_luminance_values[i]);

	writebyte(0x10); // HTYACinfo
	for (i = 0; i < 16; i++)
		writebyte(std_ac_luminance_nrcodes[i + 1]);
	for (i = 0; i < 162; i++)
		writebyte(std_ac_luminance_values[i]);

	writebyte(1); // HTCbDCinfo
	for (i = 0; i < 16; i++)
		writebyte(std_dc_chrominance_nrcodes[i + 1]);
	for (i = 0; i < 12; i++)
		writebyte(std_dc_chrominance_values[i]);

	writebyte(0x11); // HTCbACinfo = 0x11;
	for (i = 0; i < 16; i++)
		writebyte(std_ac_chrominance_nrcodes[i + 1]);
	for (i = 0; i < 162; i++)
		writebyte(std_ac_chrominance_values[i]);
}

void write_jpeg_header(unsigned int file_pointer, unsigned int height, unsigned int width)
{
	writeword(0xFFD8); // SOI
	write_APP0info();
	write_DQTinfo();
	write_SOF0info(height, width);
	write_DHTinfo();
	write_SOSinfo();
}

unsigned int remaining_data = 0;
unsigned int remaining_data_count = 0;

void finalize_jpeg_file()
{
	if (remaining_data_count > 0)
	{
		unsigned char byte_to_write = remaining_data << remaining_data_count;
		writebyte(byte_to_write);
	}
	writeword(0xFFD9); // EOI - End of Image
	write_jpeg(jpgbuff, jpgn);

}

unsigned char extract_byte_from_word(unsigned int input_word, unsigned int left_border)
{
	//	printf("Extracting %x\n", (unsigned char) (input_word >> (left_border - 7)));
	return (unsigned char) (input_word >> (left_border - 8));
}

unsigned int delete_byte_from_word(unsigned int input_word, unsigned int left_border)
{
	return input_word & (0xFFFFFF >> (32 - left_border));
}

unsigned char get_oldest_byte(block_output* block)
{
	unsigned char output_byte;

	unsigned int target_word = block->bit_position >> 5;

//	printf("target word: %u\n", target_word);
	unsigned int position_in_target_word = block->bit_position - (target_word << 5);
//	printf("position in target word: %u\n", position_in_target_word);

	if (position_in_target_word > 7)
	{
		output_byte = extract_byte_from_word(block->content[target_word], position_in_target_word);
		//		printf("%x\n", output_byte);
		block->content[target_word] = delete_byte_from_word(block->content[target_word], position_in_target_word);
		block->bit_position -= 8;
	}
	else
	{
		//		printf("%x, %x\n", (unsigned char) (block->content[target_word] << (8 - position_in_target_word)), (unsigned char) (block->content[target_word - 1] >> (32 - position_in_target_word)));
		output_byte = (unsigned char) (block->content[target_word] << (8 - position_in_target_word))
				| (unsigned char) (block->content[target_word - 1] >> (32 - (8 - position_in_target_word)));
		block->content[target_word] = 0;
		block->content[target_word - 1] = (block->content[target_word - 1] << (8 - position_in_target_word)) >> (8 - position_in_target_word);
		block->bit_position -= 8;
	}
//	printf("output_byte: %x\n", output_byte);
//	print_block_output(block);
	return output_byte;
}

void add_remaining_bits(block_output* block, unsigned char remaining_data, unsigned int remaining_data_count)
{
	unsigned int target_word = block->bit_position >> 5;
	unsigned int position_in_target_word = block->bit_position - (target_word << 5);

	// TODO: check if this position check is right (23 or 24)
	if (position_in_target_word > 23)
	{
		block->content[target_word] = block->content[target_word] | (remaining_data << position_in_target_word);
		block->content[target_word + 1] = remaining_data >> (32 - position_in_target_word);
	}
	else
	{
		block->content[target_word] = block->content[target_word] | (remaining_data << position_in_target_word);

	}
	block->bit_position += remaining_data_count;
}

void write_jpeg_file(block_output* incoming_block)
{
	if (remaining_data_count > 0)
	{
//		printf("Adding %u remaining bits: %x...\n", remaining_data_count, remaining_data);
//		printf("Before:\n");
//		print_block_output(incoming_block);
		add_remaining_bits(incoming_block, remaining_data, remaining_data_count);
//		printf("After:\n");
//		print_block_output(incoming_block);
//		printf("\n\n\n");
	}

	while (incoming_block->bit_position > 7)
	{
		unsigned char byte_to_write = get_oldest_byte(incoming_block);
		writebyte(byte_to_write);

		if (byte_to_write == 0xFF)
		{
			writebyte(0);
		}
	}

	remaining_data = incoming_block->content[0];
	remaining_data_count = incoming_block->bit_position;

}

void initialize_jpeg_file(unsigned int width, unsigned int height)
{

	if ((jpeg_output_file = open("output_file.jpg", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR)) < 0)
	{
		printf("The JPEG output file could not be created...\n");
	}

	write_jpeg_header(jpeg_output_file, height, width);
}

#endif	/* JPEG_WRITER_HPP */

