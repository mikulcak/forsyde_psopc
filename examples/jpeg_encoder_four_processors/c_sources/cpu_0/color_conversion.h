#include "subsample.hpp"

void color_conversion_func(bitmap_reader_output *inp1)
{
	BGR **current_color_block = &(inp1->color_block);
	int current_width = inp1->width;
	int current_height = inp1->height;
	int current_block_id = inp1->block_id;

	if (current_block_id != -1)
	{
#ifdef VERBOSE
		printf("Processing block %d\n", current_block_id);
#endif
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

#ifdef SEND_WCCT
	PERF_RESET(PERFORMANCE_COUNTER_CPU_0_BASE);
	PERF_START_MEASURING(PERFORMANCE_COUNTER_CPU_0_BASE);
	PERF_BEGIN (PERFORMANCE_COUNTER_CPU_0_BASE, SEND_SECTION);
#endif
	send_token(2, 3, (unsigned int *) & (y_blocks[0][0]));
#ifdef SEND_WCCT
	PERF_END (PERFORMANCE_COUNTER_CPU_0_BASE, SEND_SECTION);
	printf("send_token WCCT: %u\n", (unsigned int) perf_get_section_time((void *) PERFORMANCE_COUNTER_CPU_0_BASE, SEND_SECTION));
#endif
	

	send_token(2, 5, (unsigned int *) & (y_blocks[0][1]));
	send_token(2, 7, (unsigned int *) & (y_blocks[1][0]));
	send_token(2, 9, (unsigned int *) & (y_blocks[1][1]));
	send_token(2, 11, (unsigned int *) &cb_block);
	send_token(2, 13, (unsigned int *) &cr_block);
}
