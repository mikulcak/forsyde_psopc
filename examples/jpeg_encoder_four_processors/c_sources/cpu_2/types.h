/* 
 * File:   types.h
 * Author: marcus
 *
 * Created on March 28, 2013, 6:39 PM
 */

#ifndef TYPES_H
#define	TYPES_H

#ifdef	__cplusplus
extern "C"
{
#endif

typedef unsigned char color;

typedef struct huffman_s
{
	const unsigned char (*haclen)[12];
	const unsigned short (*hacbit)[12];
	const unsigned char *hdclen;
	const unsigned short *hdcbit;
	const unsigned char *qtable;
	short dc;
} huffman_t;

//extern huffman_t huffman_ctx[3];

// the output of the Huffman encoding step
// packed into a 256 bit array, of which not all bits will be used

typedef struct huffman_out_struct
{
	unsigned int content[8];
	unsigned int bit_position;
	unsigned int height;
	unsigned int width;
	int block_id;
} huffman_encoding_output;

typedef struct block_output_struct
{
	unsigned int content[48];
	unsigned int bit_position;
} block_output;

typedef struct smaller_block_struct
{
	unsigned int height;
	unsigned int width;
	int block_id;
	short content[8][8];
	short is_cr, is_cb;
} smaller_block;


// bitmap types

typedef struct
{
	unsigned int HeaderSize;
	int Width;
	int Height;
	unsigned short int Planes;
	unsigned short int BitCount;
	unsigned int Compression;
	unsigned int SizeImage;
	int PelsPerMeterX;
	int PelsPerMeterY;
	unsigned int ClrUsed;
	unsigned int ClrImportant;
	unsigned int RedMask;
	unsigned int GreenMask;
	unsigned int BlueMask;
	unsigned int AlphaMask;
	unsigned int CsType;
	unsigned int Endpoints[9]; // see http://msdn2.microsoft.com/en-us/library/ms536569.aspx
	unsigned int GammaRed;
	unsigned int GammaGreen;
	unsigned int GammaBlue;
} BITMAP_HEADER;

typedef struct
{
	color Red;
	color Green;
	color Blue;
	color Alpha;
} RGBA;

typedef struct
{
	color Blue;
	color Green;
	color Red;
	color Alpha;
} BGRA;

typedef struct
{
	color Blue;
	color Green;
	color Red;
} BGR;

typedef struct
{
	unsigned char B, G, R;
} colorRGB;

typedef struct
{
	unsigned short int Blue :5;
	unsigned short int Green :5;
	unsigned short int Red :5;
	unsigned short int Reserved :1;
} BGR16;

typedef struct
{
	unsigned int height;
	unsigned int width;
	int block_id;
	BGR color_block[16][16];
} bitmap_reader_output;

typedef struct
{
	unsigned int height;
	unsigned int width;
	int block_id;
	block_output current_block_data;
} encoded_block;

#ifdef	__cplusplus
}
#endif

#endif	/* TYPES_H */

