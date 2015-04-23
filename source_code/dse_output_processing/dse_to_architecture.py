##
# @file
# This script converts a possible DSE phase output, described as an XML file, into a 
# system description and set of C header files containing information needed for the implemented
# communication functions.
# @copyright BSD New
# @author Marcus Mikulcak
# 
#

import os, sys, time, fileinput, argparse, shutil

try:
	from lxml import etree as ET
except:
	print "Module lxml not found..."
	sys.exit()

## The class describing the data channels carrying tokens between 
# SDF actors in the process network.
class data_channel:
   ## @var source
   #  The ID number of the source SDF actor
   ## @var sink
   #  The ID number of the target SDF actor
   ## @var source_cpu
   #  The number of the CPU the source actor is mapped to
   ## @var sink_cpu
   #  The number of the CPU the target actor is mapped to
   ## @var buffer_length
   #  The length of the SDF buffer (the number of tokens the instiatated FIFO can hold)
   ## @var token_size
   #  The size of a single token to be sent
   ## @var fifo
   #  If this data channel holds tokens of a size smaller or equal to four bytes, this field will be True


	## The constructor.
   #  @param self The object pointer.
   #  @param source The ID number of the source SDF actor.
   #  @param sink The ID number of the target SDF actor.
	def __init__(self, source = 0, sink = 0):
		self.source = source
		self.sink = sink
		self.buffer_length = 0
		self.token_size = 0
		self.sink_cpu = 0
		self.source_cpu = 0
		self.fifo = False

   ## The overridden print method to simply print the attributes of this data channel as a string.
   #  @param self The object pointer.
   #  @return The string containing all information encapsulated in the current instance.
	def __str__(self):
		return_value = ""
		if self.fifo == True:
			return_value = "FIFO channel "
		else:
			return_value = "Channel "
		return_value = return_value + "from process " + str(self.source) + " on CPU " + str(self.source_cpu) + " to process " + str(self.sink) + " on CPU " + str(self.sink_cpu) + ", buffer size: " + str(self.buffer_size)
		if self.fifo == False:
			return_value = return_value + ", " + str(self.token_size) + " bytes each"
		return return_value


## This function returns the number of the CPU a given SDF actor is mapped to.
#
#  @param dse_tree_root The pointer to the root of the parsed XML input file.
#  @param process_id The SDF actor ID number.
#  @return The number of the CPU to which the SDF actor is mapped.
def get_cpu_for_process(dse_tree_root, process_id):
	return_value = 0
	for process in dse_tree_root.iter('process'):
		if (process.get("id") == process_id):
			return_value = process.get("cpu")
	return return_value

## This function creates the text representation of a C array from a given python array.
#
#  @param highest_cpu The pointer to the root of the parsed XML input file.
#  @param base_text The SDF actor ID number.
#  @param python_array The python arrary to be converted to a C array.
#  @return A string containing the C array.
def create_c_array(highest_cpu, base_text, python_array):
	return_text = base_text + " = \n{\n"
	# return_text = return_text + 

	for i in range(highest_cpu + 1):
		for j in range(highest_cpu + 1):
			if j == 0:
				return_text = return_text + "\t{" + str(python_array[i][j])
			elif j < highest_cpu:
				return_text = return_text + ", " + str(python_array[i][j])
			else:
				return_text = return_text + ", " + str(python_array[i][j]) + "}"
		if i < highest_cpu:
			return_text = return_text + ",\n"
		else:
			return_text = return_text + "\n};\n"
	return return_text

## This function creates and writes the C source file structure to be later filled by the programmer.
#
#  @param current_cpu_number The current CPU number to write the file into the correct folder.
def create_basic_cpu_code(current_cpu_number):
	return_text = """/** @file
 * @brief The main source file for CPU """ + str(current_cpu_number) + """.
 */\n"""
	return_text = return_text + "#include \"buffers.h\"\n\n"
	return_text = return_text + "int main()\n{\n\tinitialize_buffers();\n\n"
	return_text = return_text + "\t//declare communication variables here\n\n"
	return_text = return_text + "\twhile (1)\n\t{\n\t\t//receive_token(sender, receiver, pointer_to_data);\n\t\t//call process functions here\n\t\t//send_token(sender, receiver, pointer_to_data);\n\t}\n"
	return_text = return_text + "}\n"

	f = open('cpu_' + str(current_cpu_number) + ".c", 'w')
	f.write(return_text)
	f.close()

## This function creates and writes the C header files for a given CPU into a folder.
#
# The created folders have the structure cpu_0, cpu_1, ..., cpu_n and will each contain a 
# file called buffers.h, which in turn contains the base address information for each shared memory,
# hardware FIFO and DMA controller.
#  @param list_of_communication_channels The list of all communication channels to extract the names and therefore base addresses of all shared elements.
#  @param highest_cpu The number of CPUs in the system.
#  @param current_cpu_number The number of the CPU currently being processed.
#  @param cpu_dma_list A list of Boolean values showing which CPU is equipped with a DMA controller. 
#  @param cpu_fifo_list A list of Boolean values showing which CPU is connected to at least one hardware FIFO module. 
def create_base_address_matrix(list_of_communication_channels, highest_cpu, current_cpu_number = 0, cpu_dma_list = [], cpu_fifo_list = []):
	buffer_address_list = [[0]*(highest_cpu + 1) for i in range(highest_cpu + 1)]
	token_size_list = [[0]*(highest_cpu + 1) for i in range(highest_cpu + 1)]
	buffer_size_list = [[0]*(highest_cpu + 1) for i in range(highest_cpu + 1)]

	fifo_list = [[0]*(highest_cpu + 1) for i in range(highest_cpu + 1)]


	for communication_channel in list_of_communication_channels:
		if getattr(communication_channel, "fifo") == False:
			buffer_address_list[int(getattr(communication_channel, "source"))][int(getattr(communication_channel, "sink"))] = "(volatile CircularBuffer *) " + ("shared_memory_process_" + getattr(communication_channel, "source") + "_process_" + getattr(communication_channel, "sink") + "_BASE").upper()
		else:
			fifo_list[int(getattr(communication_channel, "source"))][int(getattr(communication_channel, "sink"))] = "(volatile int *) " + ("FIFO_cpu_" + getattr(communication_channel, "source_cpu") + "_cpu_" + getattr(communication_channel, "sink_cpu") + "_BASE").upper()			

	for communication_channel in list_of_communication_channels:
		if getattr(communication_channel, "fifo") == False:
			token_size_list[int(getattr(communication_channel, "source"))][int(getattr(communication_channel, "sink"))] = getattr(communication_channel, "token_size")
		else:
			token_size_list[int(getattr(communication_channel, "source"))][int(getattr(communication_channel, "sink"))] = "4"


	for communication_channel in list_of_communication_channels:
		buffer_size_list[int(getattr(communication_channel, "source"))][int(getattr(communication_channel, "sink"))] = getattr(communication_channel, "buffer_size")

	return_text = """/** @file
 * @brief The header file for CPU """ + str(current_cpu_number) + """, containing all CPU-specific information necessary to use the communication functions.
 */\n
"""
	return_text = return_text + "#include <stdio.h>\n"
	return_text = return_text + "#include \"system.h\"\n"
	return_text = return_text + "#include \"altera_avalon_performance_counter.h\"\n"
	
	return_text = return_text + "/** include the CircularBuffer structure definition in buffer_types.h*/\n"
	return_text = return_text + "#include \"buffer_types.h\"\n\n"

	return_text = return_text + "/** The number of processes (SDF actors) in the system.*/\n"
	return_text = return_text + "#define NR_PROCESSES " + str(highest_cpu + 1) + "\n\n"

	# DMA-specific information in the header files
	if cpu_dma_list[int(current_cpu_number)] == 1:
		return_text = return_text + "// The current CPU is connected to a DMA controller.\n"
		return_text = return_text + "/** Turn on the usage of DMA controllers in the communication functions.*/\n"
		return_text = return_text + "#define DMA 1\n"
		return_text = return_text + "/** The name of the DMA controller base address macro.*/\n"
		return_text = return_text + "#define THIS_DMA_BASE DMA_CPU_" + str(current_cpu_number) + "_BASE\n"
		return_text = return_text + "/** The name of the DMA controller connected to the current CPU.*/\n"
		return_text = return_text + "#define THIS_DMA_NAME \"/dev/dma_cpu_" + str(current_cpu_number) + "\"\n\n"
	else:
		return_text = return_text + "// The current CPU is not connected to a DMA controller.\n"
		return_text = return_text + "/** Turn off the usage of DMA controllers in the communication functions.*/\n"
		return_text = return_text + "#define DMA 0\n\n"

	# if one or multiple FIFOs are connected to the current CPU, add the FIFO flag to add the correct headers in buffer_functions.h
	if cpu_fifo_list[int(current_cpu_number)] == 1:
		return_text = return_text + "/** Turn on the usage of FIFO and their drivers in the communication functions.*/\n"
		return_text = return_text + "#define FIFO 1\n\n"
	else:
		return_text = return_text + "/** Turn on the usage of FIFO and their drivers in the communication functions.*/\n"
		return_text = return_text + "#define FIFO 0\n\n"

	return_text = return_text + "/** The base addresses of all shared memories connected to the current CPU.*/\n"
	return_text = return_text + create_c_array(highest_cpu, "volatile CircularBuffer* address_map[NR_PROCESSES][NR_PROCESSES]", buffer_address_list) + "\n"
	return_text = return_text + "/** The hardware FIFO module base addresses connected to the current CPU.*/\n"
	return_text = return_text + create_c_array(highest_cpu, "volatile int* fifo_list[NR_PROCESSES][NR_PROCESSES]", fifo_list) + "\n"
	return_text = return_text + "/** The token sizes of all SDF channels in the system.*/\n"
	return_text = return_text + create_c_array(highest_cpu, "unsigned short token_sizes[NR_PROCESSES][NR_PROCESSES]", token_size_list) + "\n"
	return_text = return_text + "/** The buffer lenghts of all SDF channels in the system.*/\n"
	return_text = return_text + create_c_array(highest_cpu, "unsigned char buffer_length[NR_PROCESSES][NR_PROCESSES]", buffer_size_list) + "\n"

	return_text = return_text + "/** include the communication functions in buffer_functions.h*/\n"
	return_text = return_text + "#include \"buffer_functions.h\"\n"

	f = open('buffers.h', 'w')
	f.write(return_text)
	f.close()


## This function determines the next larger power of two for a given value, used by the instantiation of memory sizes.
#
#  @param input_value An integer for which the next larger power of two is to be determined.
#  @return The next power of two, an integer. 
def get_next_power_of_two(input_value):
	output_value = 0
	i = 0
	while (output_value < input_value):
		# print i
		output_value = 2**i
		i = i + 1
	return output_value

## The main function of the module.
#
#	This function calls all other functions and performs the main work in this script. 
#	By parsing the input file, it creates a list of communication channels using the data_channel class and sets all information accordingly, 
#	such as sink, source, token_size, etc. Using this information, it creates a basic system description with the required number of CPUs, DMA controllers and hardware FIFOs.
#
#  @param input_file The DSE output XML file.
def create(input_file):

	try:
		tree = ET.parse(os.path.abspath(input_file))
	except Exception as e: 
		print e
		print "The DSE output could not be read"
		sys.exit()
	dse_output_root = tree.getroot()

	list_of_processes = []
	list_of_cpus = []

	for process in dse_output_root.iter('process'):
		list_of_processes.append(process.get("id"))
		if not (process.get("cpu") in list_of_cpus):
			list_of_cpus.append(process.get("cpu"))

	list_of_channels = []

	for channel in dse_output_root.iter("channel"):
		new_channel = data_channel()

		for parent_process in channel.findall(".."):
			setattr(new_channel, "source", parent_process.get("id"))
			setattr(new_channel, "source_cpu", get_cpu_for_process(dse_output_root, parent_process.get("id")))

		for to_process in channel.iter("to_process"):
			setattr(new_channel, "sink", to_process.text)
			setattr(new_channel, "sink_cpu", get_cpu_for_process(dse_output_root, to_process.text))

		for token_size in channel.iter("token_size"):
			setattr(new_channel, "token_size", token_size.text)
			if int(token_size.text) <= 4:
				setattr(new_channel, "fifo", True)

		for buffer_size in channel.iter("buffer_size"):
			setattr(new_channel, "buffer_size", buffer_size.text)

		if not get_cpu_for_process(dse_output_root, getattr(new_channel, "source")) == get_cpu_for_process(dse_output_root, getattr(new_channel, "sink")):
			list_of_channels.append(new_channel)

	for element in list_of_channels:
		print element

	# now create the architecture specification for each CPU (list_of_cpus)

	# Create the root element
	# system_root = ET.Element('system', tdma_round_length="2", tdma_slot_length="5000")
	system_root = ET.Element('system')

	# Make a new document tree
	doc = ET.ElementTree(system_root)

	system_root.append(ET.Comment("CPUs"))

	cpu_has_dma = [0] * len(list_of_cpus)
	cpu_has_fifo = [0] * len(list_of_cpus)

	# Add the subelements
	for cpu in list_of_cpus:
		new_cpu_element = ET.SubElement(system_root, 'module', 
														kind="altera_nios2",
														name="cpu_" + cpu,
														tdma_slots = "")
	
	system_root.append(ET.Comment("CPU-specific memories"))
	for cpu in list_of_cpus:
		new_memory_element = ET.SubElement(system_root, 'module',
														kind="altera_avalon_onchip_memory2",
														name="memory_cpu_" + cpu,
														size="32768")

	system_root.append(ET.Comment("CPU-specific UARTs"))
	for cpu in list_of_cpus:
		new_memory_element = ET.SubElement(system_root, 'module',
														kind="altera_avalon_jtag_uart",
														name="uart_cpu_" + cpu)

	system_root.append(ET.Comment("CPU-specific performance counters"))
	for cpu in list_of_cpus:
		new_memory_element = ET.SubElement(system_root, 'module',
														kind="altera_avalon_performance_counter",
														name="performance_counter_cpu_" + cpu)

	system_root.append(ET.Comment("CPU-specific DMA controllers"))

	# add DMA controllers to CPUs that need it, which works in the following way:
	# go through all communication channels and check if they are FIFO-based or not
	# if not, then add DMA for the source CPU of the channel and set the flag in the cpu_has_dma list to avoid adding it twice
	for communication_channel in list_of_channels:
		# we need more than a FIFO here, so add DMA for the source CPU of the channel
		if int(getattr(communication_channel, "token_size")) > 4:
			if cpu_has_dma[int(getattr(communication_channel, "source_cpu"))] == 0:
				cpu_has_dma[int(getattr(communication_channel, "source_cpu"))] = 1
				new_dma_element = ET.SubElement(system_root, 'module',
														kind="altera_avalon_dma",
														name="dma_cpu_" + getattr(communication_channel, "source_cpu"))


	# and then the communication code for every channel between CPUs (list_of_channels)
	system_root.append(ET.Comment("Communication memories"))
	for communication_channel in list_of_channels:
		fifo = False
		if int(getattr(communication_channel, "token_size")) <= 4:
			# print "FIFO is enough"
			fifo = True
		required_size = get_next_power_of_two(12 + int(getattr(communication_channel, "buffer_size")) * (int(getattr(communication_channel, "token_size")) + 4))
		# print "Required memory size for communication: " + str(required_size)
		if not fifo:
			new_communication_memory = ET.SubElement(system_root, "module",
															kind="altera_avalon_onchip_memory2",
															size=str(required_size),
															name="shared_memory_process_" + getattr(communication_channel, "source") + "_process_" + getattr(communication_channel, "sink"),
															tdma_slot_length="5000")
			# create 
			# for cpu in list_of_cpus:
				# print "new communication memory"
				# print communication_channel
			new_communication_memory.set("cpu_" + getattr(communication_channel, "source_cpu") + "_data_master_slots", "-" * 3)
			new_communication_memory.set("dma_cpu_" + getattr(communication_channel, "source_cpu") + "_write_master_slots", "-" * 3)
			new_communication_memory.set("cpu_" + getattr(communication_channel, "sink_cpu") + "_data_master_slots", "-" * 3)
		else:
			# print "Inserting FIFO hardware..."
			cpu_has_fifo[int(getattr(communication_channel, "source_cpu"))] = 1
			cpu_has_fifo[int(getattr(communication_channel, "sink_cpu"))] = 1
			system_root.append(ET.Comment("FIFO"))
			new_fifo = ET.SubElement(system_root, "module",
													kind="altera_avalon_fifo",
													depth=str(get_next_power_of_two(int(getattr(communication_channel, "buffer_size")))),
													name="fifo_cpu_" + getattr(communication_channel, "source_cpu") + "_cpu_" + getattr(communication_channel, "sink_cpu"))

	# add the SRAM
	system_root.append(ET.Comment("SRAM"))
	new_sram = ET.SubElement(system_root, "module",
											kind="altera_up_avalon_sram",
											size="2097152",
											name="sram")



	system_root.append(ET.Comment("CPU-specific connections"))
	# add all basic connections between the CPUs and their memory, JTAG UARTs and performance counters
	for cpu in list_of_cpus:
		new_cpu_connection = ET.SubElement(system_root, 'connection',
														start="cpu_" + cpu,
														end="memory_cpu_" + cpu,
														data="yes",
														instructions="yes")
	for cpu in list_of_cpus:
		new_cpu_connection = ET.SubElement(system_root, 'connection',
														start="cpu_" + cpu,
														end="uart_cpu_" + cpu,
														data="yes",
														instructions="no")
	for cpu in list_of_cpus:
		new_cpu_connection = ET.SubElement(system_root, 'connection',
														start="cpu_" + cpu,
														end="performance_counter_cpu_" + cpu,
														data="yes",
														instructions="no")

	system_root.append(ET.Comment("DMA control connections"))
	for cpu in list_of_cpus:
		if cpu_has_dma[int(cpu)]:
			new_cpu_connection = ET.SubElement(system_root, 'connection',
														start="cpu_" + cpu,
														end="dma_cpu_" + cpu,
														data="yes",
														control="yes")

	system_root.append(ET.Comment("DMA <-> local memory connections"))
	for cpu in list_of_cpus:
		if cpu_has_dma[int(cpu)]:
			new_cpu_connection = ET.SubElement(system_root, 'connection',
														start="dma_cpu_" + cpu,
														end="memory_cpu_" + cpu,
														data="yes",
														read="yes")


	system_root.append(ET.Comment("Interrupts"))
	for cpu in list_of_cpus:
		new_uart_interrupt = ET.SubElement(system_root, 'interrupt',
														start="cpu_" + cpu,
														end="uart_cpu_" + cpu,
														number="0")
	for cpu in list_of_cpus:
		if cpu_has_dma[int(cpu)]:
			new_dma_interrupt = ET.SubElement(system_root, 'interrupt',
														start="cpu_" + cpu,
														end="dma_cpu_" + cpu,
														number="1")


	system_root.append(ET.Comment("Connections to shared memories"))
	for communication_channel in list_of_channels:
		fifo = False
		if int(getattr(communication_channel, "token_size")) <= 4:
			# print "FIFO is enough"
			fifo = True
		required_size = 12 + int(getattr(communication_channel, "buffer_size")) * (int(getattr(communication_channel, "token_size")) + 4)
		# print "Required memory size for communication: " + str(required_size)
		if not fifo:
			new_connection = ET.SubElement(system_root, "connection",
															start="cpu_" + getattr(communication_channel, "source_cpu"),
															end="shared_memory_process_" + getattr(communication_channel, "source") + "_process_" + getattr(communication_channel, "sink"),
															data="yes",
															instructions="no")

			new_connection = ET.SubElement(system_root, "connection",
															start="cpu_" + getattr(communication_channel, "sink_cpu"),
															end="shared_memory_process_" + getattr(communication_channel, "source") + "_process_" + getattr(communication_channel, "sink"),
															data="yes",
															instructions="no")

			new_connection = ET.SubElement(system_root, "connection",
															start="dma_cpu_" + getattr(communication_channel, "source_cpu"),
															end="shared_memory_process_" + getattr(communication_channel, "source") + "_process_" + getattr(communication_channel, "sink"),
															data="yes",
															write="yes")
		else:
			# print "Inserting FIFO connections..."
			system_root.append(ET.Comment("FIFO connection"))
			new_connection = ET.SubElement(system_root, "connection",
															start="cpu_" + getattr(communication_channel, "source_cpu"),
															end="fifo_cpu_" + getattr(communication_channel, "source_cpu") + "_cpu_" + getattr(communication_channel, "sink_cpu"),
															data="yes",
															direction="in")

			new_connection = ET.SubElement(system_root, "connection",
															start="cpu_" + getattr(communication_channel, "sink_cpu"),
															end="fifo_cpu_" + getattr(communication_channel, "source_cpu") + "_cpu_" + getattr(communication_channel, "sink_cpu"),
															data="yes",
															direction="out")

	# connect the SRAM to CPU 0
	system_root.append(ET.Comment("SRAM connection"))
	new_connection = ET.SubElement(system_root, "connection",
													start="cpu_0",
													end="sram",
													data="yes",
													instructions="no")



	highest_process = 0
	for item in list_of_channels:
		if (int(getattr(item, "sink")) > int(highest_process)):
			highest_process = getattr(item, "sink")
		if (int(getattr(item, "source")) > int(highest_process)):
			highest_process = getattr(item, "source")

	input_file_path = os.path.abspath(os.path.dirname(input_file))

	for cpu in list_of_cpus:
		print "Creating customized buffers.h for CPU " + str(cpu)
		sub_channel_list = []
		for current_channel in list_of_channels:
			if (cpu == current_channel.sink_cpu) or (cpu == current_channel.source_cpu):
				sub_channel_list.append(current_channel)
		try:
			# print os.path.join(input_file_path, "cpu_" + str(cpu))

			os.mkdir(os.path.join(input_file_path, "cpu_" + str(cpu)))
		except:
			shutil.rmtree(os.path.join(input_file_path, "cpu_" + str(cpu)))
			os.mkdir(os.path.join(input_file_path, "cpu_" + str(cpu)))
		
		os.chdir(os.path.join(input_file_path, "cpu_" + str(cpu)))

		create_base_address_matrix(sub_channel_list, int(highest_process), cpu, cpu_has_dma, cpu_has_fifo)
		create_basic_cpu_code(cpu)
		# os.chdir("..")


	# Save to XML file
	outFile = open(os.path.join(input_file_path, 'system_description.xml'), 'w')
	doc.write(outFile, xml_declaration=False, encoding='utf-8', pretty_print=True) 


## If this script is called by itself (and not from within another script where it is imported), this function calls create() with a number of test parameters.
#
def main():
	parser = argparse.ArgumentParser(description='Automatically create architecture description from DSE tool output.')
	parser.add_argument('--input_file', 
							action='store', 
							default="", 
							help='The DSE output file',
							required=True)

	args = parser.parse_args()
	create(args.input_file)

if __name__ == "__main__":
	main()
