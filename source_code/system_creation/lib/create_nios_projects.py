# !/usr/bin/python
# -*- coding: utf-8 -*-

##
# @file
# This script creates Nios II BSPs and code projects for each CPU in a given basic system description using the <TT>name</TT> tag of the CPU.
# 
# @copyright BSD New
# @author Marcus Mikulcak
# 
# @param sopc_file The SOPC Builder file.
# @param input_xml The basic model description to find the list of CPUs.
# @param base_folder The base path for the Nios2 projects and BSPs to be created, represented by the Quartus project name

# propagate the functions offered by this module
__all__ = ["create"]


import sys, os, subprocess

try:
	from blessings import Terminal
except:
	print "Module blessings not found..."
	sys.exit()

t = Terminal()

try:
	from lxml import etree as ET
except:
	print "Module lxml not found..."
	sys.exit()


## This function performs the main body of work of this script.
#
# By iterating over the provided basic system description, it recognizes instantiates CPUs and uses the 
# provided locations of C source as well as project base folder and name to create Board Support Packages
# and code projects for each CPU.
# 
# Additionally, messages are printed to guide the programmer to open terminals to CPU instances on the FPGA 
# as well as the set environment variables in order to perform a ModelSim simulation of the created system.
# 
# @param base_folder The location of the current Quartus project.
# @param project_name The name of the current Quartus project.
# @param input_xml The basic system description XML file.
# @param sopc_file The .sopc file generated by SOPC Builder, using by the BSP creation tool.
# @param source_location The folder containing the individual CPU C source, i.e., containing a set of folders such as cpu_0, cpu_1, ...
# 

def create(base_folder, project_name, input_xml, sopc_file, source_location):
	print t.bold("Reading xml input file " + input_xml + "...\n")
	try:
		tree = ET.parse(os.path.abspath(input_xml))
	except:
		print "The input model could not be read"
		sys.exit()
	input_system_root = tree.getroot()

	# iterate over the initial system description and find all CPU modules
	for module in input_system_root.iter('module'):
		if module.get('kind') == "altera_nios2":
			# for every CPU in the system, try to create a BSP and catch possible errors
			print t.bold("Creating BSP for " + module.get('name'))
			try:
				# the length of this line is due to the building of different paths:
				# to the C sources (the path is created using the path of the input xml and the CPU names from the system description)
				# to the projects to be created (using the project name and the subfolder 'software')
				p2 = subprocess.check_call(["nios2-bsp", "HAL", str(os.path.join(base_folder, project_name, "software", str(os.path.basename(sopc_file)).rstrip(".sopc") + "_" + module.get('name') + "_bsp")), os.path.abspath(sopc_file) + "info", "--cpu-name=" + module.get('name'), "--set", "hal.enable_reduced_device_drivers" , "true", "--set", "hal.enable_small_c_library", "true", "--default_sections_mapping", "memory_" + module.get('name')])
				print t.bold("If this CPU is connected to an SRAM, add: --cmd add_section_mapping sram sram to the BSP creation command.")
			except subprocess.CalledProcessError:
				print t.color(1)("BSP could not be created...")
				# sys.exit()
	# iterate over the initial system description and find all CPU modules
	for module in input_system_root.iter('module'):
		if module.get('kind') == "altera_nios2":
			# for every CPU found, create a Nios2 project and catch possible errors
			print t.bold("Creating project for " + module.get('name'))
			try:
				# the length of this line is due to the building of different paths:
				# to the C sources (the path is created using the path of the input xml and the CPU names from the system description)
				# to the projects to be created (using the project name and the subfolder 'software')
				print str(os.path.abspath(os.path.join(source_location, module.get('name'))))
				print str(os.path.abspath(os.path.join(os.getcwd(), "lib", "common_files", "buffer_code")))
				p2 = subprocess.check_call(["nios2-app-generate-makefile", "--src-dir", str(os.path.join(source_location, module.get('name'))), "--inc-dir", str(os.path.join(os.getcwd(), "lib", "common_files", "buffer_code")), "--bsp-dir", str(os.path.join(base_folder, project_name, "software", str(os.path.basename(sopc_file)).rstrip(".sopc") + "_" + module.get('name') + "_bsp")), "--app-dir", str(os.path.join(base_folder, project_name, "software", str(os.path.basename(sopc_file)).rstrip(".sopc") + "_" + module.get('name')))])
			except subprocess.CalledProcessError:
				print t.color(1)("Project could not be created...")
				# sys.exit()

	print t.bold("To enable a ModelSim simulation of the system, set the following environment variables:")
	print t.bold("\texport QUARTUS_PROJECT_DIR=" + str(os.path.abspath(project_name)))
	print t.bold("\texport SOPC_NAME=" + str(os.path.abspath(sopc_file)))

	print t.bold("And start a terminal to the simulation with:")

	for module in input_system_root.iter('module'):
		if module.get('kind') == "altera_avalon_jtag_uart":
			uart_name = module.get('name')
			print t.bold("\tnios2-terminal -M -n " + uart_name)

	print t.bold("To start a terminal to the development board:")

	instance = 0

	for module in input_system_root.iter('module'):
		if module.get('kind') == "altera_avalon_jtag_uart":
			print t.bold("\tnios2-terminal --instance " + str(instance))
			instance = instance + 1



## If this script is called by itself (and not from within another script where it is imported), this function calls create() with a number of test parameters.
#
def main():
	create("/home/marcus/Desktop/example_applications/susan/susan_two_processos", "susan_two_processos", "/home/marcus/Desktop/thesis/branch/susan_two_processors/create_quartus_projects/input_files/model_input.xml", "/home/marcus/Desktop/example_applications/susan/susan_two_processos/susan_two_processos_system.sopc", "/home/marcus/Desktop/example/sources/")

if __name__ == "__main__":
	main()