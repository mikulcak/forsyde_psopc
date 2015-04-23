/* 
 * File:   subsample.hpp
 * Author: marcus
 *
 * Created on March 28, 2013, 5:46 PM
 */

#ifndef SUBSAMPLE_HPP
#define	SUBSAMPLE_HPP

#include "types.h"

/*
 RGB to YCbCr Conversion:
 */
// Y = 0.299*R + 0.587*G + 0.114*B
__inline color RGB2Y(const color r, const color g, const color b)
{
	return (153 * r + 301 * g + 58 * b) >> 9;
}
// Cb = -0.1687*R - 0.3313*G + 0.5*B + 128
__inline color RGB2Cb(const color r, const color g, const color b)
{
	return (65536 - 86 * r - 170 * g + 256 * b) >> 9;
}
// Cr = 0.5*R - 0.4187*G - 0.0813*B + 128
__inline color RGB2Cr(const color r, const color g, const color b)
{
	return (65536 + 256 * r - 214 * g - 42 * b) >> 9;
}

// chroma subsampling, i.e. converting a 16x16 RGB block into 8x8 Cb and Cr

void subsample(const BGR rgb[16][16], short cb[8][8], short cr[8][8])
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

#endif	/* SUBSAMPLE_HPP */

