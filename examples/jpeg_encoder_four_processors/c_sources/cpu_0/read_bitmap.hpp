/*
 * File:   read_bitmap.hpp
 * Author: marcus
 *
 * Created on March 29, 2013, 10:48 PM
 */

#ifndef READ_BITMAP_HPP
#define  READ_BITMAP_HPP

#include "types.h"
//#include "lena.h"
#include "cat.h"
//#include "ray.h"

void load_MCU(unsigned int MCU_x, unsigned int MCU_y, unsigned int width, unsigned int height, unsigned int picture_width, colorRGB *RGB_buffer,
              BGR *target_block)
{
	short int x, y;
	int location;

	location = MCU_y * 16 * picture_width + MCU_x * 16;
	for (y = 0; y < 16; y++)
	{
		for (x = 0; x < 16; x++)
		{
			(target_block + (width * y) + x)->Red = RGB_buffer[location + (picture_width * y) + x].R;
			(target_block + (width * y) + x)->Green = RGB_buffer[location + (picture_width * y) + x].G;
			(target_block + (width * y) + x)->Blue = RGB_buffer[location + (picture_width * y) + x].B;
		}
	}
}

// FILE *BMP_fp;
unsigned int width;
unsigned int height;

colorRGB *RGB_buffer;

void init_bitmap(bitmap_reader_output *output_data)
{
	width = (unsigned int) (bmp_data[19] * 256 + bmp_data[18]);
	height = (unsigned int) (bmp_data[23] * 256 + bmp_data[22]);

	RGB_buffer = (colorRGB *) ((unsigned char *) bmp_data + 54);

#ifdef VERBOSE
	printf("height: %u, width: %u\n", height, width);
#endif
	output_data->block_id = -1;
	output_data->height = height;
	output_data->width = width;
	// output_data->color_block = NULL;
}

void read_bitmap(const bitmap_reader_output *input_data, bitmap_reader_output *output_data)
{
	unsigned int MCU_x, MCU_y;
	int i = 0;

	int last_counter = input_data->block_id;
	// printf("last counter: %d\n", last_counter);
	for (MCU_y = 0; MCU_y < height / 16; MCU_y++)
	{
		for (MCU_x = 0; MCU_x < width / 16; MCU_x++)
		{
			if (i > last_counter)
			{
				load_MCU(MCU_x, MCU_y, 16, 16, width, RGB_buffer, (BGR *) output_data->color_block);
				output_data->height = height;
				output_data->width = width;
				output_data->block_id = i;
				return;
			}
			else
			{
				i++;
			}
		}
	}
	// if the function reaches this point, all blocks have been processed
	// and the number of blocks in the input picture is written into the output structure
	// this signals the jpeg_writer to finalize the writing and write the EOI marker as
	// well as stray bits in the buffer
	output_data->block_id = ((width * height) >> 8);
}

#endif   /* READ_BITMAP_HPP */

