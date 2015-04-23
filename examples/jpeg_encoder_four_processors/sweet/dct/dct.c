/* MDH WCET BENCHMARK SUITE. File version $Id: insertsort.c,v 1.3 2005/11/11 10:30:41 ael01 Exp $ */

/*************************************************************************/
/*                                                                       */
/*   SNU-RT Benchmark Suite for Worst Case Timing Analysis               */
/*   =====================================================               */
/*                              Collected and Modified by S.-S. Lim      */
/*                                           sslim@archi.snu.ac.kr       */
/*                                         Real-Time Research Group      */
/*                                        Seoul National University      */
/*                                                                       */
/*                                                                       */
/*        < Features > - restrictions for our experimental environment   */
/*                                                                       */
/*          1. Completely structured.                                    */
/*               - There are no unconditional jumps.                     */
/*               - There are no exit from loop bodies.                   */
/*                 (There are no 'break' or 'return' in loop bodies)     */
/*          2. No 'switch' statements.                                   */
/*          3. No 'do..while' statements.                                */
/*          4. Expressions are restricted.                               */
/*               - There are no multiple expressions joined by 'or',     */
/*                'and' operations.                                      */
/*          5. No library calls.                                         */
/*               - All the functions needed are implemented in the       */
/*                 source file.                                          */
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*  FILE: insertsort.c                                                   */
/*  SOURCE : Public Domain Code                                          */
/*                                                                       */
/*  DESCRIPTION :                                                        */
/*                                                                       */
/*     Insertion sort for 10 integer numbers.                            */
/*     The integer array a[] is initialized in main function.            */
/*                                                                       */
/*  REMARK :                                                             */
/*                                                                       */
/*  EXECUTION TIME :                                                     */
/*                                                                       */
/*                                                                       */
/*************************************************************************/



#ifdef DEBUG
int cnt1, cnt2;
#endif

unsigned int a[11];

short incoming_pixels[8][8], outgoing_pixel[8][8];

      void sort(unsigned int *a)
{
	int i, j, temp;
	i = 2;
	while (i <= 10)
	{
#ifdef DEBUG
		cnt1++;
#endif
		j = i;
#ifdef DEBUG
		cnt2 = 0;
#endif
		while (a[j] < a[j - 1])
		{
#ifdef DEBUG
			cnt2++;
#endif
			temp = a[j];
			a[j] = a[j - 1];
			a[j - 1] = temp;
			j--;
		}
#ifdef DEBUG
		printf("Inner Loop Counts: %d\n", cnt2);
#endif
		i++;
	}
#ifdef DEBUG
	printf("Outer Loop : %d ,  Inner Loop : %d\n", cnt1, cnt2);
#endif

}

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


int main()
{
	// int  i, j, temp;

	// a[0] = 0;   /* assume all data is positive */
	// a[1] = 11; a[2] = 10; a[3] = 9; a[4] = 8; a[5] = 7; a[6] = 6; a[7] = 5;
	// a[8] = 4; a[9] = 3; a[10] = 2;
	// i = 2;

	// sort(a);
	dct3(incoming_pixels, outgoing_pixel);

	return 1;
}

