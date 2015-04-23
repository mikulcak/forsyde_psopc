#include "jpeg_writer.h"
#include "types.h"

encoded_block input;

void write_jpeg_func(encoded_block inp)
{
	block_output current_block = inp.current_block_data;
	int current_block_id = inp.block_id;

	int width = inp.width;
	int height = inp.height;

	if (current_block_id == -1)
	{
		// initialize jpeg output file
		initialize_jpeg_file(width, height);
	}
	else if (current_block_id == ((width * height) >> 8))
	{
		// if the image is completely processed, write the EOI marker and stop the simulation
		finalize_jpeg_file();
	}
	else
	{
		write_jpeg_file(&current_block);
	}
}


int main()
{
	int i;
	for (i = 0; i < 2; ++i)
	{
		write_jpeg_func(input);
	}

	return 1;
}

