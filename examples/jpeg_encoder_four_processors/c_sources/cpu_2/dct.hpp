/* 
 * File:   dct.hpp
 * Author: marcus
 *
 * Created on March 28, 2013, 5:50 PM
 */

#ifndef DCT_HPP
#define	DCT_HPP

/******************************************************************************
 **  dct
 **  --------------------------------------------------------------------------
 **  Fast DCT - Discrete Cosine Transform.
 **  This function converts 8x8 pixel block into frequencies.
 **  Lowest frequencies are at the upper-left corner.
 **  The input and output could point at the same array, in this case the data
 **  will be overwritten.
 **  
 **  ARGUMENTS:
 **      pixels  - 8x8 pixel array;
 **      data    - 8x8 freq block;
 **
 **  RETURN: -
 ******************************************************************************/
// void dct(short pixels[8][8], short data[8][8])
// {
//     short rows[8][8];
//     unsigned i;

//     static const int
//     c1 = 1004, /* cos(pi/16) << 10 */
//             s1 = 200, /* sin(pi/16) */
//             c3 = 851, /* cos(3pi/16) << 10 */
//             s3 = 569, /* sin(3pi/16) << 10 */
//             r2c6 = 554, /* sqrt(2)*cos(6pi/16) << 10 */
//             r2s6 = 1337, /* sqrt(2)*sin(6pi/16) << 10 */
//             r2 = 181; /* sqrt(2) << 7*/

//     /* transform rows */
//     for (i = 0; i < 8; i++)
//     {
//         int x0, x1, x2, x3, x4, x5, x6, x7, x8;

//         x0 = pixels[i][0];
//         x1 = pixels[i][1];
//         x2 = pixels[i][2];
//         x3 = pixels[i][3];
//         x4 = pixels[i][4];
//         x5 = pixels[i][5];
//         x6 = pixels[i][6];
//         x7 = pixels[i][7];

//         /* Stage 1 */
//         x8 = x7 + x0;
//         x0 -= x7;
//         x7 = x1 + x6;
//         x1 -= x6;
//         x6 = x2 + x5;
//         x2 -= x5;
//         x5 = x3 + x4;
//         x3 -= x4;

//         /* Stage 2 */
//         x4 = x8 + x5;
//         x8 -= x5;
//         x5 = x7 + x6;
//         x7 -= x6;
//         x6 = c1 * (x1 + x2);
//         x2 = (-s1 - c1) * x2 + x6;
//         x1 = (s1 - c1) * x1 + x6;
//         x6 = c3 * (x0 + x3);
//         x3 = (-s3 - c3) * x3 + x6;
//         x0 = (s3 - c3) * x0 + x6;

//         /* Stage 3 */
//         x6 = x4 + x5;
//         x4 -= x5;
//         x5 = r2c6 * (x7 + x8);
//         x7 = (-r2s6 - r2c6) * x7 + x5;
//         x8 = (r2s6 - r2c6) * x8 + x5;
//         x5 = x0 + x2;
//         x0 -= x2;
//         x2 = x3 + x1;
//         x3 -= x1;

//         /* Stage 4 and output */
//         rows[i][0] = x6;
//         rows[i][4] = x4;
//         rows[i][2] = x8 >> 10;
//         rows[i][6] = x7 >> 10;
//         rows[i][7] = (x2 - x5) >> 10;
//         rows[i][1] = (x2 + x5) >> 10;
//         rows[i][3] = (x3 * r2) >> 17;
//         rows[i][5] = (x0 * r2) >> 17;
//     }

//     /* transform columns */
//     for (i = 0; i < 8; i++)
//     {
//         int x0, x1, x2, x3, x4, x5, x6, x7, x8;

//         x0 = rows[0][i];
//         x1 = rows[1][i];
//         x2 = rows[2][i];
//         x3 = rows[3][i];
//         x4 = rows[4][i];
//         x5 = rows[5][i];
//         x6 = rows[6][i];
//         x7 = rows[7][i];

//         /* Stage 1 */
//         x8 = x7 + x0;
//         x0 -= x7;
//         x7 = x1 + x6;
//         x1 -= x6;
//         x6 = x2 + x5;
//         x2 -= x5;
//         x5 = x3 + x4;
//         x3 -= x4;

//         /* Stage 2 */
//         x4 = x8 + x5;
//         x8 -= x5;
//         x5 = x7 + x6;
//         x7 -= x6;
//         x6 = c1 * (x1 + x2);
//         x2 = (-s1 - c1) * x2 + x6;
//         x1 = (s1 - c1) * x1 + x6;
//         x6 = c3 * (x0 + x3);
//         x3 = (-s3 - c3) * x3 + x6;
//         x0 = (s3 - c3) * x0 + x6;

//         /* Stage 3 */
//         x6 = x4 + x5;
//         x4 -= x5;
//         x5 = r2c6 * (x7 + x8);
//         x7 = (-r2s6 - r2c6) * x7 + x5;
//         x8 = (r2s6 - r2c6) * x8 + x5;
//         x5 = x0 + x2;
//         x0 -= x2;
//         x2 = x3 + x1;
//         x3 -= x1;

//         /* Stage 4 and output */
//         data[0][i] = ((x6 + 16) >> 3);
//         data[4][i] = ((x4 + 16) >> 3);
//         data[2][i] = ((x8 + 16384) >> 13);
//         data[6][i] = ((x7 + 16384) >> 13);
//         data[7][i] = ((x2 - x5 + 16384) >> 13);
//         data[1][i] = ((x2 + x5 + 16384) >> 13);
//         data[3][i] = (((x3 >> 8) * r2 + 8192) >> 12);
//         data[5][i] = (((x0 >> 8) * r2 + 8192) >> 12);
//     }
// }

// simple but fast DCT

void dct3(const short pixels[8][8], short data[8][8])
{
    short rows[8][8];
    unsigned i;

    static const short // Ci = cos(i*PI/16)*(1 << 14);
    C1 = 16070,
            C2 = 15137,
            C3 = 13623,
            C4 = 11586,
            C5 = 9103,
            C6 = 6270,
            C7 = 3197;

    /* transform rows */
    for (i = 0; i < 8; i++)
    {
        short s07, s16, s25, s34, s0734, s1625;
        short d07, d16, d25, d34, d0734, d1625;

        s07 = pixels[i][0] + pixels[i][7];
        d07 = pixels[i][0] - pixels[i][7];
        s16 = pixels[i][1] + pixels[i][6];
        d16 = pixels[i][1] - pixels[i][6];
        s25 = pixels[i][2] + pixels[i][5];
        d25 = pixels[i][2] - pixels[i][5];
        s34 = pixels[i][3] + pixels[i][4];
        d34 = pixels[i][3] - pixels[i][4];

        rows[i][1] = (C1 * d07 + C3 * d16 + C5 * d25 + C7 * d34) >> 14;
        rows[i][3] = (C3 * d07 - C7 * d16 - C1 * d25 - C5 * d34) >> 14;
        rows[i][5] = (C5 * d07 - C1 * d16 + C7 * d25 + C3 * d34) >> 14;
        rows[i][7] = (C7 * d07 - C5 * d16 + C3 * d25 - C1 * d34) >> 14;

        s0734 = s07 + s34;
        d0734 = s07 - s34;
        s1625 = s16 + s25;
        d1625 = s16 - s25;

        rows[i][0] = (C4 * (s0734 + s1625)) >> 14;
        rows[i][4] = (C4 * (s0734 - s1625)) >> 14;

        rows[i][2] = (C2 * d0734 + C6 * d1625) >> 14;
        rows[i][6] = (C6 * d0734 - C2 * d1625) >> 14;
    }

    /* transform columns */
    for (i = 0; i < 8; i++)
    {
        short s07, s16, s25, s34, s0734, s1625;
        short d07, d16, d25, d34, d0734, d1625;

        s07 = rows[0][i] + rows[7][i];
        d07 = rows[0][i] - rows[7][i];
        s16 = rows[1][i] + rows[6][i];
        d16 = rows[1][i] - rows[6][i];
        s25 = rows[2][i] + rows[5][i];
        d25 = rows[2][i] - rows[5][i];
        s34 = rows[3][i] + rows[4][i];
        d34 = rows[3][i] - rows[4][i];

        data[1][i] = (C1 * d07 + C3 * d16 + C5 * d25 + C7 * d34) >> 16;
        data[3][i] = (C3 * d07 - C7 * d16 - C1 * d25 - C5 * d34) >> 16;
        data[5][i] = (C5 * d07 - C1 * d16 + C7 * d25 + C3 * d34) >> 16;
        data[7][i] = (C7 * d07 - C5 * d16 + C3 * d25 - C1 * d34) >> 16;

        s0734 = s07 + s34;
        d0734 = s07 - s34;
        s1625 = s16 + s25;
        d1625 = s16 - s25;

        data[0][i] = (C4 * (s0734 + s1625)) >> 16;
        data[4][i] = (C4 * (s0734 - s1625)) >> 16;

        data[2][i] = (C2 * d0734 + C6 * d1625) >> 16;
        data[6][i] = (C6 * d0734 - C2 * d1625) >> 16;
    }
}


#endif	/* DCT_HPP */

