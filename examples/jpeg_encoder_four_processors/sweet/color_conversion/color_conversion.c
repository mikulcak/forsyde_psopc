typedef unsigned char color;

typedef struct
{
	color Blue;
	color Green;
	color Red;
} BGR;

typedef struct
{
	BGR content[16][16];
} color_block;

typedef struct
{
	short content[8][8];
} color_converted_block;


color_block color_block_to_be_converted;

/*
 RGB to YCbCr Conversion:
 */
// Y = 0.299*R + 0.587*G + 0.114*B
color RGB2Y(const color r, const color g, const color b)
{
	return (153 * r + 301 * g + 58 * b) >> 9;
}
// Cb = -0.1687*R - 0.3313*G + 0.5*B + 128
color RGB2Cb(const color r, const color g, const color b)
{
	return (65536 - 86 * r - 170 * g + 256 * b) >> 9;
}
// Cr = 0.5*R - 0.4187*G - 0.0813*B + 128
color RGB2Cr(const color r, const color g, const color b)
{
	return (65536 + 256 * r - 214 * g - 42 * b) >> 9;
}

// chroma subsampling, i.e. converting a 16x16 RGB block into 8x8 Cb and Cr

void subsample(BGR rgb[16][16], short cb[8][8], short cr[8][8])
{
	unsigned r, c;
	for (r = 0; r < 8; r++)
		for (c = 0; c < 8; c++)
		{
			unsigned rr = (r << 1);
			unsigned cc = (c << 1);

			// calculating average values
			color R = (rgb[rr][cc].Red + rgb[rr][cc + 1].Red + rgb[rr + 1][cc].Red + rgb[rr + 1][cc + 1].Red) >> 2;
			color G = (rgb[rr][cc].Green + rgb[rr][cc + 1].Green + rgb[rr + 1][cc].Green + rgb[rr + 1][cc + 1].Green) >> 2;
			color B = (rgb[rr][cc].Blue + rgb[rr][cc + 1].Blue + rgb[rr + 1][cc].Blue + rgb[rr + 1][cc + 1].Blue) >> 2;

			cb[r][c] = (short) RGB2Cb(R, G, B) - 128;
			cr[r][c] = (short) RGB2Cr(R, G, B) - 128;
		}
}


void color_conversion(color_block current_color_block)
{

	color_converted_block outgoing_blocks[2][2];
	color_converted_block cb_block;
	color_converted_block cr_block;

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

					const color R = current_color_block.content[rr][cc].Red;
					const color G = current_color_block.content[rr][cc].Green;
					const color B = current_color_block.content[rr][cc].Blue;

					// converting RGB into Y (luminance)
					outgoing_blocks[i][j].content[r][c] = RGB2Y(R, G, B) - 128;
				}
			}
		}
	}

	// one block of 8x8 Cr
	short Cb8x8[8][8];
	// one block of 8x8 Cb
	short Cr8x8[8][8];

	// create subsampled Cb and Cr blocks
	subsample(current_color_block.content, Cb8x8, Cr8x8);

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

}

int main()
{
	color_conversion(color_block_to_be_converted);
	return 1;
}

