/** @file 
 * @brief The header file containing the C data structure of the circular buffer object
 * @author Marcus Mikulcak
 * @copyright BSD New
 */
 
/** The circular buffer data structure.
 @details This structure is used to store the FIFO management information in each instantiated shared memory acting as a SDF data channel
 */
typedef struct {
    int         size;                   /*!< maximum number of elements */
    int         start;                  /*!< index of oldest element              */
    int         count;                  /*!< index at which to write new element  */
	// from here on, the memory will be filled with the actual elements
} CircularBuffer;

/**
 * The message counter, currently not used.
 */
unsigned int global_message_counter = 0;
