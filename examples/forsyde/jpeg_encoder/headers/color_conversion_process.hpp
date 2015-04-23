/*
 * color_conversion_process.hpp
 *
 *  Created on: May 9, 2013
 *      Author: marcus
 */

#ifndef COLOR_CONVERSION_PROCESS_HPP_
#define COLOR_CONVERSION_PROCESS_HPP_

#include <forsyde.hpp>
#include "types.h"
#include "subsample.hpp"


using namespace ForSyDe::SDF;

SC_MODULE(color_conversion)
{
	SDF_in<bitmap_reader_output> iport1;
	// Cb
	SDF_out<short[8][8]> oport1;
	// Cr
	SDF_out<short[8][8]> oport2;
	// Yb8x8[0][0]
	SDF_out<short[8][8]> oport3;
	// Yb8x8[0][1]
	SDF_out<short[8][8]> oport4;
	// Yb8x8[1][0]
	SDF_out<short[8][8]> oport5;
	// Yb8x8[1][1]
	SDF_out<short[8][8]> oport6;

//	SDF2SDF<double> din, dout;

	SC_CTOR(color_conversion)
	{

		// four blocks of 8x8 Y
		short Y8x8[2][2][8][8];
		// one block of 8x8 Cr
		short Cb8x8[8][8];
		// one block of 8x8 Cb
		short Cr8x8[8][8];

		unsigned int i, j, r, c;

		// getting four 8x8 Y-blocks
		for (i = 0; i < 2; i++)
			for (j = 0; j < 2; j++)
			{
				for (r = 0; r < 8; r++)
					for (c = 0; c < 8; c++)
					{
						const unsigned rr = (i << 3) + r;
						const unsigned cc = (j << 3) + c;

						const color R = iport1[rr][cc].Red;
						const color G = iport1[rr][cc].Green;
						const color B = iport1[rr][cc].Blue;
						//					printf("R: %u G: %u B: %u\n", R, G, B);
						// converting RGB into Y (luminance)
						Y8x8[i][j][r][c] = RGB2Y(R, G, B) - 128;
					}
			}

//		auto averager1 = make_comb2("averager1", averager_func, 2, 3, 2, oport1, iport1, dout);
//		averager1->oport1(din);

//		make_delayn("avginit1", 0.0, 2, dout, din);
	}
};

#endif /* COLOR_CONVERSION_PROCESS_HPP_ */
