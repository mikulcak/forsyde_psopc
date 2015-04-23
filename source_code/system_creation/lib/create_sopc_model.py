# !/usr/bin/python
# -*- coding: utf-8 -*-


## 
#
# @file
# This script creates a SOPC Builder input file from a basic system description XML.
#
# This script and its create() function perform the most extensive work in this suite of scripts
# starting from a basic XML description of a system, it generates a complete SOPC Builder input file
# from which SOPC Builder can generate a complete Nios2 system.
# In addition to that, it generates a graphical representation of the system using GraphvViz
# it works as follows:
# - it parses the XML input file and finds modules, connections and interrupts of the system
# - the 'kind' attribute of each module and connection relates to the name of an XML file in the library
# - these represent the XML sources of SOPC moduldes and are integrated into the system when they are found in the input XML description
# - all XML files in lib/xml_element_sources form the XML library.
# 
# @param project_name The project name to generate the system name
# @param input_xml The basic XML system description
# @param fpga The FPGA device to be set in the SOPC model
# @param fpga_family The FPGA device family to be set in the SOPC model
# @return The SOPC Builder input file

# propagate the functions offered by this module
__all__ = ["create"]

import os, sys, time, fileinput

sys.tracebacklimit = 3

# module lxml to parse through the input XML file as well as to generate the output SOPC file
try:
	from lxml import etree as ET
except:
	print "Module lxml not found..."
	sys.exit()

# module pygraphviz to generate the graphical representation of the system using graphviz
try:
	import pygraphviz as pgv
except:
	print "Module pygraphviz not found..."
	sys.exit()

# module blessings to enable stled terminal output
try:
	from blessings import Terminal
except:
	print "Module blessings not found..."
	sys.exit()

t = Terminal()

## This function adds an element from the library of possible elements and sets its name 
# as well as a list of parameters provided as a dictionary and returns an instance of the 
# lxml Element class.
# 
# @param element_name The name of the element to be added.
# @param module_name The kind of element to be added.
# @param value_changes_dict The dictionary of values to be set as parameters into the module, 
# this corresponds to the parameters of the module in the XML library
# @return An instance of the lxml Element class containing the XML represetation of the module.
def get_element_and_change_values(element_name, module_name, value_changes_dict={}):
	try:
		tree = ET.parse(os.path.join(os.getcwd(), 'lib', 'xml_element_sources', element_name + '.xml'))
	except:
		print t.color(1)("An error occured opening the xml source " + element_name + "...")
		sys.exit()
	root = tree.getroot()

	# set the module name
	root.set('name', module_name)

	# set all the parameters provided
	for parameter in root.iter('parameter'):
		if parameter.get('name') in value_changes_dict.keys():
			parameter.set('value', value_changes_dict[parameter.get('name')])

	# return an instance of ElementTree so it can be written directly to a file
	return root


## This function adds an connection element, either a clock or a module connection
# and sets the base address (in case it is a module connection).
# The XML source for the connection is taken from the XML library
# and returns an instance of the lxml Element class.
# @todo: detect if base address has been given as hexadecimal or decimal value and set it accordingly
# @param kind The kind of connection to be added (module, clock or interrupt)
# @param start The starting module of the connection
# @param end The ending module of the connection
# @param base_address The base address for the module connection (needed for memory-mapped Avalon slave modules)
# @param irq_number The number of the interrupt to be added
# @return An instance of the lxml Element class containing the connection with all given parameters set

def add_connection(kind, start, end, base_address=9999, irq_number=0):
	try:
		tree = ET.parse(os.path.join(os.getcwd(), 'lib', 'xml_element_sources', kind + '_connection.xml'))
	except:
		print t.color(1)("No valid connection could be found. The valid choices are \"clock\" and \"module\"...")
		sys.exit()
	root = tree.getroot()

	if kind == "module":
		if base_address == 9999:
			print t.color(1)("A module connection must include a base address, exiting...")
			sys.exit()
		else:
			for parameter in root.iter('parameter'):
				if parameter.get('name') == 'baseAddress':
					parameter.set('value', str(hex(base_address)))
	elif kind == "interrupt":
		for parameter in root.iter('parameter'):
			if parameter.get('name') == 'irqNumber':
				parameter.set('value', str(irq_number))


	root.set('start', start)
	root.set('end', end)
	return root

## This function creates the addreass maps that are part of every processor.
# There is one address map for the data master and one for every instruction master
# 
# Given the list of slaves to each master and the name of the master for which the list
# shall be created, this functions return the corresponding address map as a CDATA string
# 
# @param master_name The name of the master for which the list is to be created, 
# 	 especially containing the address spans for each slave.
# @param input_list The list of masters and slaves created in the main function.
# @return A CDATA string containing the address map.

def create_slave_map(master_name, input_list, dma=False):
	slave_list = input_list[master_name]

	output_string = "<![CDATA[<address-map>"

	# small hack to have the memories start at an even address, which is required by SOPC Builder
	# the reason is the following:
	# the first slave is always the JTAG Debug Module, which has a memory span of 2048
	# if its base address is uneven, SOPC Builder does not complain, however
	# if the instruction memory base address is uneven, the system cannot be created
	# therefore the starting address (and therefore the JTAG Debug Module base address)
	# is decremented by 2048 so the first memory will have an even base address

	# current_address = 262144 - 2048
	current_address = 32768 - 2048

	if dma == True:
		current_address = 32768

	for slave in slave_list:
		for slave_key in slave.keys():
			# base addresses have to be aligned to the size of the slave module, an Avalon bus requirement
			if (int(current_address) % int(slave[slave_key])) != 0:
				current_address = int(current_address) + (int(slave[slave_key]) - (int(current_address) % int(slave[slave_key])))
			output_string = output_string + "<slave name=\'" + slave_key + "\'" + " start=\'" + str(hex(current_address)) + "\'"
			slave_base_address = current_address
			current_address = int(slave[slave_key]) + int(current_address)
			output_string = output_string + " end=\'" + str(hex(current_address)) + "\'/>"
			slave[slave_key] = str(slave_base_address)
	output_string = output_string + "</address-map>]]>"

	return output_string

## This function returns the name of the instruction memory to a given master whereas this master is generally a processor.
# 
# @param master_name The name of the master.
# @param input_list The list of slaves and masters generated in the main function.
# @return The name of the instruction memory to a given master
def get_instruction_memory_for_master(master_name, input_list):
	slave_list = input_list[master_name + ".instruction_master"]
	for connected_slave in slave_list:
		if not (master_name + ".jtag_debug_module" in connected_slave.keys()):
			return connected_slave.keys()[0]

## This functions return the unix time stamp of the current time, used for completeness' sake in the SOPC Builder input file
#
def time_stamp_now():
	return int(time.time())

## This functions returns the kind of module for a given module name
# e.g. module_name = "cpu_0", this function returns "altera_nios2".
# @param module_name The name of the module.
# @param input_file_root The pointer to the XML parser root of the basic system description
# @return The kind of module for which the name was given.
def get_kind_for_name(module_name, input_file_root):
	for module in input_file_root.iter('module'):
		if module.get('name') == module_name:
			return module.get('kind')
	return ""

## The main body of work is performed by this function.
# @param base_folder The location for the SOPC Buidler file to be written to.
# @param project_name The project name to generate the system name
# @param input_file The basic XML system description
# @param device The FPGA device to be set in the SOPC model
# @param device_family The FPGA device family to be set in the SOPC model
# @return The SOPC Builder input file
def create(base_folder, project_name, input_file, device, device_family):

	G = pgv.AGraph()
	G.graph_attr.update()

	print t.bold("Reading xml input file " + input_file + "...\n")
	try:
		tree = ET.parse(os.path.abspath(input_file))
	except Exception as e: 
		print e
		print "The input model could not be read"
		sys.exit()
	input_system_root = tree.getroot()

	system_name = project_name + "_system"
	print t.bold("Creating system called " + system_name + "...")

	quartus_project_name = project_name

	slave_list = {}
	dma_slave_list = {}

	# TODO: system hash could be set somehow, although it does not seem to be necessary
	print "\tSetting basic system settings...\n"
	output_root = get_element_and_change_values('bare_system', system_name, {'device' : device, 'deviceFamily' : device_family, 'projectName' : quartus_project_name, "timeStamp": str(time_stamp_now())})
	
	# this adds the Avalon clock source module for the system
	# @todo: get frequency from input XML
	print "\tAdding clock source...\n"
	output_root.append(get_element_and_change_values('clock_source', "clock_0", {'clockFrequency' : '50000000'}))

	cpu_counter = 0
	jtag_counter = 0

	# this iteration adds all processors and memory modules to the output xml
	# and sets the CPU id for every processor
	for module in input_system_root.iter('module'):
		if module.get('kind') == "altera_nios2":
			print "\tAdding processor named " + module.get('name') + "..."
			slave_list[module.get('name') + ".data_master"] = [{module.get('name') + ".jtag_debug_module" : "2048"}]
			slave_list[module.get('name') + ".instruction_master"] = [{module.get('name') + ".jtag_debug_module" : "2048"}]
			output_root.append(get_element_and_change_values(module.get('kind'), module.get('name'), {"cpuID" : str(cpu_counter), "debug_jtagInstanceID" : str(cpu_counter)}))
			cpu_counter = cpu_counter + 1;
		elif module.get('kind') == "altera_avalon_onchip_memory2":
			print "\tAdding memory named " + module.get('name') + "..."
			output_root.append(get_element_and_change_values(module.get('kind'), module.get('name'), {'memorySize' : module.get('size')}))
		elif module.get('kind') == "altera_avalon_jtag_uart":
			print "\tAdding JTAG UART named " + module.get('name') + "..."
			output_root.append(get_element_and_change_values(module.get('kind'), module.get('name'), {"hubInstanceID" : str(jtag_counter)}))			
			jtag_counter = jtag_counter + 1
		elif module.get('kind') == "altera_avalon_performance_counter":
			print "\tAdding performance counter named " + module.get('name') + "..."
			output_root.append(get_element_and_change_values(module.get('kind'), module.get('name')))			
		elif module.get('kind') == "altera_avalon_pio":
			print "\tAdding PIO named " + module.get('name') + "..."
			output_root.append(get_element_and_change_values(module.get('kind'), module.get('name'), {"width" : module.get('width'), "direction" : module.get('direction')}))				
		elif module.get('kind') == "altera_avalon_fifo":
			print "\tAdding FIFO named " + module.get('name') + "..."			
			output_root.append(get_element_and_change_values(module.get('kind'), module.get('name'), {"fifoDepth" : module.get('depth')}))				
		elif module.get('kind') == "altera_up_avalon_sram":
			print "\tAdding SRAM named " + module.get('name') + "..."			
			output_root.append(get_element_and_change_values(module.get('kind'), module.get('name')))				
		elif module.get('kind') == "altera_avalon_dma":
			print "\tAdding DMA controller named " + module.get('name') + "..."			
			dma_slave_list[module.get('name') + ".read_master"] = []
			dma_slave_list[module.get('name') + ".write_master"] = []
			output_root.append(get_element_and_change_values(module.get('kind'), module.get('name')))				
		else:
			print t.color(1)("\tUnknown module named  " + module.get('name') + " found...")
			continue
		G.add_node(module.get('name'), color='black', label=module.get('name'), shape="rectangle", fontsize='8', fontname='Helvetica')	


	# this loop sets all clock connections
	# they could be set in the loop above but SOPC Builder 
	# requires that all modules are listed first, then all connections
	# additionally, the SOPC modules have different names for their clock ports
	for module in input_system_root.iter('module'):
		if module.get('kind') == "altera_nios2":
			output_root.append(add_connection('clock', "clock_0.clk", module.get('name') + ".clk"))
		elif module.get('kind') == "altera_avalon_onchip_memory2":
			output_root.append(add_connection('clock', "clock_0.clk", module.get('name') + ".clk1"))
		elif module.get('kind') == "altera_avalon_performance_counter":
			output_root.append(add_connection('clock', "clock_0.clk", module.get('name') + ".clk"))
		elif module.get('kind') == "altera_avalon_jtag_uart":
			output_root.append(add_connection('clock', "clock_0.clk", module.get('name') + ".clk"))
		elif module.get('kind') == "altera_avalon_pio":
			output_root.append(add_connection('clock', "clock_0.clk", module.get('name') + ".clk"))			
		elif module.get('kind') == "altera_avalon_fifo":
			output_root.append(add_connection('clock', "clock_0.clk", module.get('name') + ".clk_in"))						
			# output_root.append(add_connection('clock', "clock_0.clk", module.get('name') + ".clk_out"))						
		elif module.get('kind') == "altera_avalon_dma":
			output_root.append(add_connection('clock', "clock_0.clk", module.get('name') + ".clk"))						
		elif module.get('kind') == "altera_up_avalon_sram":
			output_root.append(add_connection('clock', "clock_0.clk", module.get('name') + ".clock_reset"))						
		else:
			print t.color(1)("\tUnknown module named  " + module.get('name') + " found...")
			continue

	print ""

	# this iteration adds all the connections to later create the address maps
	# but it does not actually add any connections
	for connection in input_system_root.iter('connection'):
		print "\tAdding connection between " + connection.get('start') + " and " + connection.get('end') + "..."
		edge_label = ""
		# if the connection points to a JTAG UART, add it like this
		# if connection.get('end') == "jtag_uart_cpu_0":
			# slave_list[connection.get('start') + ".data_master"] = [{connection.get('end') + ".avalon_jtag_slave" : "8"}]
			# pass
		# if it is any other kind of connection, watch for data and instruction flags
		# else:
		if connection.get('data') == "yes":
			# if connection.get('end') == "jtag_uart_cpu_0":
			# if get_kind_for_name(connection.get('end'), input_system_root) == "altera_avalon_onchip_memory2":
				# pass
				# slave_list[connection.get('start') + ".data_master"].append({connection.get('end') + ".s1" : ""})
			if get_kind_for_name(connection.get('end'), input_system_root) == "altera_avalon_jtag_uart":
				slave_list[connection.get('start') + ".data_master"].append({connection.get('end') + ".avalon_jtag_slave" : "8"})
			elif get_kind_for_name(connection.get('end'), input_system_root) == "altera_avalon_pio":
				slave_list[connection.get('start') + ".data_master"].append({connection.get('end') + ".s1" : "16"})
			elif get_kind_for_name(connection.get('end'), input_system_root) == "altera_avalon_performance_counter":
				slave_list[connection.get('start') + ".data_master"].append({connection.get('end') + ".control_slave" : "64"})
			elif get_kind_for_name(connection.get('end'), input_system_root) == "altera_avalon_dma":
				slave_list[connection.get('start') + ".data_master"].append({connection.get('end') + ".control_port_slave" : "32"})
			elif get_kind_for_name(connection.get('end'), input_system_root) == "altera_up_avalon_sram":
				slave_list[connection.get('start') + ".data_master"].append({connection.get('end') + ".avalon_sram_slave" : ""})
			elif get_kind_for_name(connection.get('end'), input_system_root) == "altera_avalon_fifo":
				if connection.get('direction') == "in":
					slave_list[connection.get('start') + ".data_master"].append({connection.get('end') + ".in" : "4"})
					# slave_list[connection.get('start') + ".data_master"].append({connection.get('end') + ".in_csr" : "32"})
				else:
					slave_list[connection.get('start') + ".data_master"].append({connection.get('end') + ".out" : "4"})
					# slave_list[connection.get('start') + ".data_master"].append({connection.get('end') + ".out_csr" : "32"})					
			# elif get_kind_for_name(connection.get('start'), input_system_root) == "altera_avalon_onchip_memory2":
				# continue
			elif (get_kind_for_name(connection.get('end'), input_system_root) == "altera_avalon_onchip_memory2") and (get_kind_for_name(connection.get('start'), input_system_root) == "altera_avalon_dma"):
				pass
				# continue
			else:
				print "else case"
				slave_list[connection.get('start') + ".data_master"].append({connection.get('end') + ".s1" : ""})
			edge_label = edge_label + "Data"
		if connection.get('instructions') == "yes":
			slave_list[connection.get('start') + ".instruction_master"].append({connection.get('end') + ".s1" : ""})
			edge_label = edge_label + "/Instructions"
		# add connection to the graph
		G.add_edge(connection.get('start'), connection.get('end'), label=edge_label, fontsize='6')
	print ""

	print "\tAdding DMA master connections"
	for connection in input_system_root.iter('connection'):
		print "\tAdding connection between " + connection.get('start') + " and " + connection.get('end') + "..."
		if get_kind_for_name(connection.get('start'), input_system_root) == "altera_avalon_dma":
			if connection.get('read') == "yes":
				dma_slave_list[connection.get('start') + ".read_master"].append({connection.get('end') + ".s1" : ""})
			elif connection.get('write') == "yes":
				dma_slave_list[connection.get('start') + ".write_master"].append({connection.get('end') + ".s1" : ""})

	# print dma_slave_list

	# this iteration gets the memory sizes of all memory modules
	for module in input_system_root.iter('module'):
		# add sizes of memory slaves
		if module.get('kind') == "altera_avalon_onchip_memory2":
			print "\tGetting size of memory named " + module.get('name') + "..."
			for master in slave_list.keys():
				for connected_slave in slave_list[master]:
					if (module.get('name') + ".s1") in connected_slave:
						connected_slave[(module.get('name') + ".s1")] = module.get('size')
			for master in dma_slave_list.keys():
				for connected_slave in dma_slave_list[master]:
					if (module.get('name') + ".s1") in connected_slave:
						connected_slave[(module.get('name') + ".s1")] = module.get('size')
		if module.get('kind') == "altera_up_avalon_sram":
			print "\tGetting size of SRAM named " + module.get('name') + "..."
			for master in slave_list.keys():
				for connected_slave in slave_list[master]:
					if (module.get('name') + ".avalon_sram_slave") in connected_slave:
						connected_slave[(module.get('name') + ".avalon_sram_slave")] = module.get('size')
			for master in dma_slave_list.keys():
				for connected_slave in dma_slave_list[master]:
					if (module.get('name') + ".avalon_sram_slave") in connected_slave:
						connected_slave[(module.get('name') + ".avalon_sram_slave")] = module.get('size')

	print ""

	# print dma_slave_list

	# print slave_list


	# print "\tAdding interrupts..."
	for interrupt in input_system_root.iter('interrupt'):
		print "\tAdding interrupt " + interrupt.get('number') + " from " + interrupt.get('end') + " to " + interrupt.get('start')
		output_root.append(add_connection('interrupt', interrupt.get('start') + ".d_irq", interrupt.get('end') + ".irq", irq_number = interrupt.get('number')))

	# this loop creates the strings containing the address maps for every master
	# and after it has finished, the slave list is updated to contain
	# the base address and not the sizes anymore
	# it also sets the slave map in the output tree
	# and a number of important parameters to the processors

	print "\tWriting slave maps and settings into processor modules..."
	
	for module in output_root.iter('module'):
		if module.get('kind') == "altera_nios2":
			for parameter in module.iter('parameter'):
				if parameter.get('name') == "resetSlave":
					parameter.set('value', get_instruction_memory_for_master(module.get('name'), slave_list))
				if parameter.get('name') == "resetOffset":
					parameter.set('value', "0")
				if parameter.get('name') == "exceptionSlave":
					parameter.set('value', get_instruction_memory_for_master(module.get('name'), slave_list))
				if parameter.get('name') == "exceptionOffset":
					parameter.set('value', "32")
				if parameter.get('name') == "breakSlave":
					parameter.text = module.get('name') + ".jtag_debug_module"
				if parameter.get('name') == "breakOffset":
					parameter.set('value', "32")					
				if parameter.get('name') == "dataSlaveMapParam":
					# print module.get('name')
					parameter.text = ET.CDATA(create_slave_map(module.get('name') + ".data_master", slave_list))
				elif parameter.get('name') == "instSlaveMapParam":
					# print module.get('name')
					parameter.text = ET.CDATA(create_slave_map(module.get('name') + ".instruction_master", slave_list))

	print ""

	print "\tWriting slave maps and settings into DMA controller modules..."
	
	for module in output_root.iter('module'):
		if module.get('kind') == "altera_avalon_dma":
			for parameter in module.iter('parameter'):
				if parameter.get('name') == "readAddressMap":
					# print module.get('name')
					parameter.text = ET.CDATA(create_slave_map(module.get('name') + ".read_master", dma_slave_list, True))
				elif parameter.get('name') == "writeAddressMap":
					# print module.get('name')
					parameter.text = ET.CDATA(create_slave_map(module.get('name') + ".write_master", dma_slave_list, True))

	# the actual connections have to be added using the base address information has just been created

	for master_key in slave_list.keys():
		# print master_key
		# print slave_list[master_key]
		for slave_module in slave_list[master_key]:
			for slave_module_name in slave_module.keys():
				# print slave_module_name
				output_root.append(add_connection('module', master_key, slave_module_name, int(slave_module[slave_module_name])))

	for master_key in dma_slave_list.keys():
		# print master_key
		# print slave_list[master_key]
		for slave_module in dma_slave_list[master_key]:
			for slave_module_name in slave_module.keys():
				# print slave_module_name
				output_root.append(add_connection('module', master_key, slave_module_name, int(slave_module[slave_module_name])))


	print t.bold("Writing SOPC Builder file " + system_name + ".sopc...")

	# turn the created Element class instance into an ElementTree class instance for writing
	output_tree = ET.ElementTree(output_root)

	# write the created xml structure into a .sopc file
	output_tree.write(os.path.join(base_folder, project_name, system_name + '.sopc'), pretty_print=True)

	# this loop adds line breaks into the xml file that are unfortunately
	# not generated by the lxml write() function
	# TODO: solve this with a regular expression
	sopc_file = fileinput.input(os.path.join(base_folder, project_name, system_name + ".sopc"), inplace=1)
	for line in sopc_file:
		if "<module" in line:
				line = line.replace("<module", "\n<module")
		if "<connection" in line:
				line = line.replace("<connection", "\n<connection")
		print line,
	sopc_file.close()

	print t.bold("Writing graphical representation to " + system_name + ".dot...")

	# write the created graph to a .dot file
	G.write(os.path.join(base_folder, project_name, system_name + '.dot'))

	print t.bold("Done")

## If this script is called by itself (and not from within another script where it is imported), this function calls create() with a number of test parameters.
#
def main():
	create("/Users/marcus/Desktop", "test_project", "/Users/marcus/Desktop/thesis/code/python/create_quartus_projects/input_files/model_input.xml", "test_device", "test_family")

if __name__ == "__main__":
	main()
