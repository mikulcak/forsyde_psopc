#include "buffers.h"


#define READ_BITMAP_SECTION 0
#define COLOR_CONVERSION_SECTION 1
#define SEND_SECTION 2

// #define READ_BITMAP_WCEC
// #define COLOR_CONVERSION_WCEC
#define SEND_WCCT

// #define VERBOSE


#include "types.h"
#include "read_bitmap.hpp"
#include "color_conversion.h"


// process 0, read_bitmap
// process 1, color_conversion

unsigned int finished = 0;

void read_bitmap_func(bitmap_reader_output *output, bitmap_reader_output *input)
{
	int current_block_id = input->block_id;

	if (current_block_id == -2)
	{
#ifdef VERBOSE
		printf("Initialization bitmap file reading...\n");
#endif
		init_bitmap(output);
	}
	else if (current_block_id == 156)
	{
		finished = 1;
	}
	else
	{
		read_bitmap(input, output);
#ifdef VERBOSE
		printf("Reading block %d\n", output->block_id);
#endif
	}
}

int main()
{
	initialize_buffers();
	printf("\n\nHej from " ALT_CPU_NAME "!\n");

	//declare communication variables here

	bitmap_reader_output read_bitmap_func_input;

	// set initial value, in case of a delay process
	read_bitmap_func_input.block_id = -2;


	while (1)
	{
		if (finished != 1)
		{
#ifdef READ_BITMAP_WCEC
			PERF_RESET(PERFORMANCE_COUNTER_CPU_0_BASE);
			PERF_START_MEASURING(PERFORMANCE_COUNTER_CPU_0_BASE);
			PERF_BEGIN (PERFORMANCE_COUNTER_CPU_0_BASE, READ_BITMAP_SECTION);
#endif
			read_bitmap_func(&read_bitmap_func_input, &read_bitmap_func_input);
#ifdef READ_BITMAP_WCEC
			PERF_END (PERFORMANCE_COUNTER_CPU_0_BASE, READ_BITMAP_SECTION);
			printf("read_bitmap WCEC: %u\n", (unsigned int) perf_get_section_time((void *) PERFORMANCE_COUNTER_CPU_0_BASE, READ_BITMAP_SECTION));
#endif
#ifdef COLOR_CONVERSION_WCEC
			PERF_RESET(PERFORMANCE_COUNTER_CPU_0_BASE);
			PERF_START_MEASURING(PERFORMANCE_COUNTER_CPU_0_BASE);
			PERF_BEGIN (PERFORMANCE_COUNTER_CPU_0_BASE, COLOR_CONVERSION_SECTION);
#endif
			color_conversion_func(&read_bitmap_func_input);
#ifdef COLOR_CONVERSION_WCEC
			PERF_END (PERFORMANCE_COUNTER_CPU_0_BASE, COLOR_CONVERSION_SECTION);
			printf("color_conversion WCEC: %u\n", (unsigned int) perf_get_section_time((void *) PERFORMANCE_COUNTER_CPU_0_BASE, COLOR_CONVERSION_SECTION));
#endif
		}

		//receive_token(sender, receiver, pointer_to_data);
		//call process functions here
		//send_token(sender, receiver, pointer_to_data);
	}
}
