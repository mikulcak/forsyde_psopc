/** @file
 * @brief The header file for CPU 0, containing all CPU-specific information necessary to use the communication functions.
 */

#include <stdio.h>
#include "system.h"
#include "altera_avalon_performance_counter.h"
/** include the CircularBuffer structure definition in buffer_types.h*/
#include "buffer_types.h"

/** The number of processes (SDF actors) in the system.*/
#define NR_PROCESSES 2

// The current CPU is connected to a DMA controller.
/** Turn on the usage of DMA controllers in the communication functions.*/
#define DMA 1
/** The name of the DMA controller base address macro.*/
#define THIS_DMA_BASE DMA_CPU_0_BASE
/** The name of the DMA controller connected to the current CPU.*/
#define THIS_DMA_NAME "/dev/dma_cpu_0"

/** Turn on the usage of FIFO and their drivers in the communication functions.*/
#define FIFO 0

/** The base addresses of all shared memories connected to the current CPU.*/
volatile CircularBuffer* address_map[NR_PROCESSES][NR_PROCESSES] = 
{
	{0, (volatile CircularBuffer *) SHARED_MEMORY_PROCESS_0_PROCESS_1_BASE},
	{0, 0}
};

/** The hardware FIFO module base addresses connected to the current CPU.*/
volatile int* fifo_list[NR_PROCESSES][NR_PROCESSES] = 
{
	{0, 0},
	{0, 0}
};

/** The token sizes of all SDF channels in the system.*/
unsigned short token_sizes[NR_PROCESSES][NR_PROCESSES] = 
{
	{0, 32},
	{0, 0}
};

/** The buffer lenghts of all SDF channels in the system.*/
unsigned char buffer_length[NR_PROCESSES][NR_PROCESSES] = 
{
	{0, 10},
	{0, 0}
};

/** include the communication functions in buffer_functions.h*/
#include "buffer_functions.h"