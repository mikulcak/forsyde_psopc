/* 
 * File:   top.hpp
 * Author: marcus
 *
 * Created on March 29, 2013, 5:44 PM
 */

#ifndef TOP_HPP
#define	TOP_HPP

#include "types.h"

#include "read_bitmap.hpp"
#include "jpeg_writer.hpp"
#include "block_processor.hpp"

#include "forsyde.hpp"
#include <tuple>

using namespace ForSyDe;
using namespace ForSyDe::SDF;

typedef std::tuple<std::vector<encoded_block>, std::vector<encoded_block>, std::vector<encoded_block>, std::vector<encoded_block>> merge_blocks_input_type;

typedef zipN<encoded_block, encoded_block, encoded_block, encoded_block> merge_blocks_zipper;

typedef std::tuple<std::vector<bitmap_reader_output>, std::vector<bitmap_reader_output>, std::vector<bitmap_reader_output>,
		std::vector<bitmap_reader_output>> rgb_collector_output_type;

typedef unzipN<bitmap_reader_output, bitmap_reader_output, bitmap_reader_output, bitmap_reader_output> rgb_block_unzipper;

void read_bitmap_func(std::vector<bitmap_reader_output> &out, const std::vector<bitmap_reader_output> &inp1)
{
//	std::cout << "read_bitmap_func" << std::endl;
	int current_block_id = inp1[0].block_id;

#pragma ForSyDe begin read_bitmap_func
	if (current_block_id == -2)
	{
		std::cout << "Initialization bitmap file reading, properties: ";
		init_bitmap(&out[0]);
	}
	else
	{
		read_bitmap(&inp1[0], &out[0]);
		std::cout << "Reading block " << out[0].block_id << std::endl;
	}
#pragma ForSyDe end
}

void rgb_block_collector_func(std::vector<rgb_collector_output_type> &out, const std::vector<bitmap_reader_output> &inp1)
{
	std::cout << "Block collector function" << std::endl;
	std::vector<bitmap_reader_output> first_block;
	std::vector<bitmap_reader_output> second_block;
	std::vector<bitmap_reader_output> third_block;
	std::vector<bitmap_reader_output> fourth_block;

	first_block.push_back(inp1[0]);
	second_block.push_back(inp1[1]);
	third_block.push_back(inp1[2]);
	fourth_block.push_back(inp1[3]);

	out[0] = std::make_tuple(first_block, second_block, third_block, fourth_block);
}

void write_jpeg_func(const encoded_block inp)
{
//	std::cout << "write_jpeg_func" << std::endl;

	block_output current_block = inp.current_block_data;
	int current_block_id = inp.block_id;

	int width = inp.width;
	int height = inp.height;

#pragma ForSyDe begin write_jpeg_func
	if (current_block_id == -1)
	{
		// initialize jpeg output file
		std::cout << "Initializating JPEG file" << std::endl;
		initialize_jpeg_file(width, height);
	}
	else if (current_block_id == ((width * height) >> 8))
	{
		// if the image is completely processed, write the EOI marker and stop the simulation
		std::cout << "Writing EOI" << std::endl;
		finalize_jpeg_file();
		sc_stop();
	}
	else
	{
		std::cout << "Writing block " << current_block_id << std::endl;
		write_jpeg_file(&current_block);
	}
#pragma ForSyDe end

}

void merge_and_write_blocks_func(const merge_blocks_input_type inp)
{
	encoded_block incoming_blocks[4];

	incoming_blocks[0] = std::get < 0 > (inp)[0];
	incoming_blocks[1] = std::get < 1 > (inp)[0];
	incoming_blocks[2] = std::get < 2 > (inp)[0];
	incoming_blocks[3] = std::get < 3 > (inp)[0];

	int i;
	for (i = 0; i < 4; ++i)
	{
		write_jpeg_func(incoming_blocks[i]);
	}
}

SC_MODULE(Top)
{
public:
	// Signals
	SDF2SDF<bitmap_reader_output> bitmap_reader_output_to_fanout;
	SDF2SDF<bitmap_reader_output> bitmap_reader_to_delay;
	SDF2SDF<bitmap_reader_output> bitmap_reader_to_processing;
	SDF2SDF<bitmap_reader_output> bitmap_reader_next_state;
	bitmap_reader_output bitmap_reader_initial_state;

	SDF2SDF<rgb_collector_output_type> rgb_collector_output;

	rgb_block_unzipper *unzip_blocks;
	std::vector<unsigned int> rgb_block_unzipper_rates;

	SDF2SDF<bitmap_reader_output> processing_input_1;
	SDF2SDF<bitmap_reader_output> processing_input_2;
	SDF2SDF<bitmap_reader_output> processing_input_3;
	SDF2SDF<bitmap_reader_output> processing_input_4;

	SDF2SDF<encoded_block> processing_to_writer_1;
	SDF2SDF<encoded_block> processing_to_writer_2;
	SDF2SDF<encoded_block> processing_to_writer_3;
	SDF2SDF<encoded_block> processing_to_writer_4;

	SDF2SDF<merge_blocks_input_type> block_merger_input;

	merge_blocks_zipper *zip_blocks;
	std::vector<unsigned> zip_blocks_rates;

	SC_CTOR(Top)
	{
		bitmap_reader_initial_state.block_id = -2;
		bitmap_reader_initial_state.height = 0;
		bitmap_reader_initial_state.width = 0;

		make_delay("bitmap_reader_delay", // Name
				bitmap_reader_initial_state, // Initial state
				bitmap_reader_next_state, bitmap_reader_to_delay);

		make_comb("read_bitmap_comb", // Name
				read_bitmap_func, // Function to invoke
				1, // Production rate
				1, // Consumption rate
				bitmap_reader_output_to_fanout, // Output signal to drive
				bitmap_reader_next_state // Input signal
				);

		auto bitmap_reader_fanout = make_fanout("bitmap_reader_fanout", bitmap_reader_to_delay, bitmap_reader_output_to_fanout);
		bitmap_reader_fanout->oport1(bitmap_reader_to_processing);

		make_comb("rgb_block_collector_comb", rgb_block_collector_func, 1, 4, rgb_collector_output, bitmap_reader_to_processing);

		rgb_block_unzipper_rates =
		{	1, 1, 1, 1};
		unzip_blocks = new rgb_block_unzipper("rgb_blocks_unzipper", rgb_block_unzipper_rates);
		unzip_blocks->iport1(rgb_collector_output);
		std::get < 0 > (unzip_blocks->oport)(processing_input_1);
		std::get < 1 > (unzip_blocks->oport)(processing_input_2);
		std::get < 2 > (unzip_blocks->oport)(processing_input_3);
		std::get < 3 > (unzip_blocks->oport)(processing_input_4);

		auto block_processor_1 = new block_processor("block_processor_1");
		block_processor_1->incoming_bitmap_block(processing_input_1);
		block_processor_1->outgoing_encoded_block(processing_to_writer_1);

		auto block_processor_2 = new block_processor("block_processor_2");
		block_processor_2->incoming_bitmap_block(processing_input_2);
		block_processor_2->outgoing_encoded_block(processing_to_writer_2);

		auto block_processor_3 = new block_processor("block_processor_3");
		block_processor_3->incoming_bitmap_block(processing_input_3);
		block_processor_3->outgoing_encoded_block(processing_to_writer_3);

		auto block_processor_4 = new block_processor("block_processor_4");
		block_processor_4->incoming_bitmap_block(processing_input_4);
		block_processor_4->outgoing_encoded_block(processing_to_writer_4);

		zip_blocks_rates =
		{	1, 1, 1, 1};

		zip_blocks = new merge_blocks_zipper("block_zipper", zip_blocks_rates);
		std::get < 0 > (zip_blocks->iport)(processing_to_writer_1);
		std::get < 1 > (zip_blocks->iport)(processing_to_writer_2);
		std::get < 2 > (zip_blocks->iport)(processing_to_writer_3);
		std::get < 3 > (zip_blocks->iport)(processing_to_writer_4);
		zip_blocks->oport1(block_merger_input);

		make_sink("merge_and_write_sink", // Name
				merge_and_write_blocks_func, // Function to invoke
				block_merger_input // Signal to report
				);

	}

#ifdef FORSYDE_INTROSPECTION

	void start_of_simulation()
	{
		ForSyDe::XMLExport dumper("");
		dumper.traverse(this);
	}

	void end_of_simulation()
	{
		std::cout << "Conversion finished..." << std::endl;
	}
#endif
};

#endif	/* TOP_HPP */

