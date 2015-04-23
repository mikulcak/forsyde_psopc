/** @file 
 * @brief The header file containing the functions facilitating communication over shared modules
 * @details Using the send_token() and receive_token() functions, it is abstract to the programmer what communication medium is used, i.e.
 * if the respective SDF channel uses a hardware FIFO, the function identifies this and issues the appropriate commands.
 * If not, it uses a DMA controller to send and receive data from the shared memory.
 * @author Marcus Mikulcak
 * @copyright BSD New
 */
 

// include string.h for memcpy()
#include <string.h>
// include stdlib.h for malloc()
#include <stdlib.h>

// include DMA HAL
#if DMA
#include "sys/alt_dma.h"
#include "altera_avalon_dma_regs.h"


/** 
 * DMA channels for transmitting and receiving when SENDING tokens
*/
alt_dma_txchan transmit_channel;

/** 
 * Multiple receive channels, one for each token channel
*/
alt_dma_rxchan receive_channel[NR_PROCESSES][NR_PROCESSES];

int dma_result;

// static volatile int rx_done = 0;

// Callback function that obtains notification that the data is received.
// static void done (void *handle, void *data)
// {
//    rx_done++;
// }
#endif

// include FIFO HAL if a FIFO is connected to the CPU
#if FIFO
#include "altera_avalon_fifo_regs.h"
#include "altera_avalon_fifo_util.h"
#endif


/**
 * This function is used to initialize the shared memory circular buffer structures with the appropriate values
 * @param cb A pointer to the CircularBuffer structure at the lowest address in the shared memory.
 * @param size The maximum number of tokens to be set for the current shared memory.
 */
void cbInit(volatile CircularBuffer *cb, int size)
{
	cb->size  = size;
	cb->start = 0;
	cb->count = 0;
}

/**
 * This function checks if a circular buffer is currently full
 * @param cb A pointer to the CircularBuffer structure.
 * @return 1, if the buffer is full; 0, if it is not.
 */
int cbIsFull(volatile CircularBuffer *cb)
{
	return cb->count == cb->size;
}

/**
 * This function checks if a circular buffer is currently empty
 * @param cb A pointer to the CircularBuffer structure.
 * @return 1, if the buffer is empty; 0, if it is not.
 */
int cbIsEmpty(volatile CircularBuffer *cb)
{
	return cb->count == 0;
}

/**
 * This function implements a busy wait.
 * @param counter A counter used as the length of the wait.
 */
void sleep(int counter)
{
	int i;
	for (i = 0; i < counter; i++);
}

/**
 * This function is used by the programmer to send a token from one SDF actor to another, across CPU borders.
 * @details By using the channel-specific information found in the buffers.h header, this function identifies the size of a token 
 * to be sent via a shared module. The type of this shared module, either a hardware FIFO or shared memory managed using a software FIFO
 * (the circular buffer structure), is also determined by the information in this header file.
 * @param sender The number of the process aiming to send data.
 * @param receiver The number of the process to receive the data.
 * @param content_to_send A pointer to the local CPU memory where the data to be sent is located.
 * @todo Replace modulo operations as they take ages on the Nios II/e.
 */
void send_token(unsigned int sender, unsigned int receiver, void *content_to_send)
{

	if (fifo_list[sender][receiver] != 0)
	{
#if FIFO
		IOWR_ALTERA_AVALON_FIFO_DATA(fifo_list[sender][receiver], content_to_send);
#endif
	}
	else
	{
		// get the base address of the circular buffer
		volatile CircularBuffer *current_buffer = address_map[sender][receiver];
		// get the size of one data element in this buffer
		size_t data_length = token_sizes[sender][receiver];

		// printf("send address: %x\n", current_buffer);


		while (current_buffer->count == current_buffer->size)
		{
			sleep(1000);
			// printf("full\n");
		}

		// calculate the next fill status of the buffer
		size_t end = (current_buffer->start + current_buffer->count) % current_buffer->size;

		// calculate the target address for the next message in the shared memory, considering the fill status of the buffer
		void *content_target_base = (void *) (((void *) (((void *) current_buffer) + 3 * sizeof(int))) + ((data_length + sizeof(int)) * end));

		// write the message counter for the next message to be sent in the shared memory
		// *((unsigned int *) content_target_base) = global_message_counter++;

		// if DMA is set to true in the processor's buffers.h, use the DMA controller to send data
#if DMA
		// do not attempt to post a new write request if the DMA is not yet done
		while (!(IORD_ALTERA_AVALON_DMA_STATUS(THIS_DMA_BASE) == 0));
		// printf("Using DMA\n");
		// set DMA transmit request into the DMA controller queue
		if ((dma_result = alt_dma_txchan_send (transmit_channel,
		                                       content_to_send,
		                                       data_length,
		                                       NULL,
		                                       NULL)) < 0)
		{
			printf("Failed to post transmit request, reason = %i\n", dma_result);
			exit (1);
		}
		// set DMA receive request into the controller queue
		if ((dma_result = alt_dma_rxchan_prepare (receive_channel[sender][receiver],
		                  content_target_base,
		                  data_length,
		                  NULL, // function pointed to here will be called via an interrupt when the DMA finishes its operation
		                  NULL)) < 0)
		{
			printf("Failed to post read request, reason = %i\n", dma_result);
			exit (1);
		}

		//
		// while (!rx_done);


#else
		// printf("Using memcpy\n");
		memcpy(content_target_base + sizeof(int), content_to_send, data_length);
#endif

		if (current_buffer->count == current_buffer->size)
			// the buffer is now full
		{
			current_buffer->start = (current_buffer->start + 1) % current_buffer->size;
		}
		else
			// there is room for at least one more element
		{
			current_buffer->count = current_buffer->count + 1;
		}

		// printf("write: start: %d\n", current_buffer->start);
		// printf("write: size: %d\n", current_buffer->size);
		// printf("write: count: %d\n", current_buffer->count);
		// printf("write: end: %d\n", end);

	}
}

/**
 * A helper function for receive_token() reading data from the circular buffer structure.
 * @param cb A pointer to the CircularBuffer structure.
 * @param data_length The size of the token to be read.
 * @param target_address A pointer to the local CPU memory where the received data is to be stored.
 * @todo Replace modulo operations as they take ages on the Nios II/e.
 */
void cbRead(volatile CircularBuffer *cb, void *target_address, size_t data_length)
{
	int start = cb->start;

	// printf("Reading...\n");
	// set the correct source of the content
	// use byte-wise increment for pointers and skip the id that gets written
	void *content_source = (void *) (((void *) cb) + 3 * sizeof(int)) + (start * (data_length + sizeof(int)));// + sizeof(int);

	// printf("target_address before: %x\n", target_address);

	// copy the data from the shared memory to the provided address
	memcpy(target_address, content_source, data_length);
	// printf("target_address after: %x\n", target_address);
	// printf("Copied...\n");

	// set the next start
	cb->start = (cb->start + 1) % cb->size;
	cb->count = cb->count - 1;

	// printf("read: start: %d\n", cb->start);
	// printf("read: size: %d\n", cb->size);
	// printf("read: count: %d\n", cb->count);
	// printf("read: end: %d\n", end);


	// printf("Ready, current count: %d, size: %d, start: %d\n", cb->count, cb->size, cb->start);
}

/**
 * This function is used by the programmer to receive a token from one SDF actor to another, across CPU borders.
 * @details By using the channel-specific information found in the buffers.h header, this function identifies the size of a token 
 * to be received via a shared module. The type of this shared module, either a hardware FIFO or shared memory managed using a software FIFO
 * (the circular buffer structure), is also determined by the information in this header file.
 * @param sender The number of the process aiming to send data.
 * @param receiver The number of the process to receive the data.
 * @param target_address A pointer to the local CPU memory where the received data is to be stored.
 */
void receive_token(unsigned int sender, unsigned int receiver, void *target_address)
{

	if (fifo_list[sender][receiver] != 0)
	{
#if FIFO
		*((unsigned int *) target_address) = IORD_ALTERA_AVALON_FIFO_DATA(fifo_list[sender][receiver]);
#endif
	}
	else
	{

		// get the address of the correct buffer according to sending and receiving process
		volatile CircularBuffer *current_buffer = address_map[sender][receiver];

		// printf("receive address: %x\n", current_buffer);

		// check if this buffer is currently empty
		// if it is, wait a little while, then try again
		while (cbIsEmpty(current_buffer))
		{
			sleep(1000);
			// printf("empty\n");
		}

		// then read the value from it and write it to the provided address
		cbRead(current_buffer, target_address, token_sizes[sender][receiver]);
		// printf("Data read...\n");
	}
}

/**
 * This function initializes CircularBuffer data structures
 * @details This function needs to be called by each CPU before performing any send_token() or receive_token() operations as it 
 * sets the FIFO management information in the structures and opens the DMA controller send and receive channels, provided 
 * DMA controllers are connected to the CPU.
 */
void initialize_buffers()
{
	int i, j;
	for (i = 0; i < NR_PROCESSES; i++)
	{
		for (j = 0; j < NR_PROCESSES; j++)
		{
			if (!(address_map[i][j] == 0))
			{
				// clean all the space required for the current buffer
				// correct token size + one integer to store message if
				// everything plus three integers for buffer header information
				memset((void *)address_map[i][j], 0, ((token_sizes[i][j] + sizeof(int)) * buffer_length[i][j]) + 3 * sizeof(int));
				// set buffer size information
				cbInit(address_map[i][j], buffer_length[i][j]);
			}
		}
	}

#if DMA
	// create the DMA transmitter channel to be used by this processor
	if ((transmit_channel = alt_dma_txchan_open(THIS_DMA_NAME)) == NULL)
	{
		printf ("Failed to open transmit channel\n");
		exit (1);
	}

	for (i = 0; i < NR_PROCESSES; i++)
	{
		for (j = 0; j < NR_PROCESSES; j++)
		{
			if (!(address_map[i][j] == 0))
			{
				// create the DMA receiver channels
				if ((receive_channel[i][j] = alt_dma_rxchan_open(THIS_DMA_NAME)) == NULL)
				{
					printf ("Failed to open receive channel\n");
					exit (1);
				}
			}
		}
	}
#endif
}
