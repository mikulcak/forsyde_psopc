#!/usr/bin/python
# -*- coding: utf-8 -*-

##
# @file
# This script performs the main work of the system creation flow. Starting from a basic system description it creates, 
# compiles and synthesizes a Quartus project, programs a connected FPGA and creates software projects for each instantiated CPU.
# 
# It uses a number of python modules, from which blessings is the only non-standard one. It can, however, easily be installed using <TT>pip</TT> or <TT>easy_install</TT>.
# Import errors are caught, as well as all errors in called scripts and Quartus executables and hints are given in case errors occur.
# 
# @copyright BSD New
# @author Marcus Mikulcak
# 
# @param project_name The name of the project to be created. (Required)
# @param system_description The location and file name of the basic system description XML file. (Required)
# @param sopc If this option is set, SOPC Builder opens in order to give the programmer the possibility to perform changes on the created system. (Optional)
# @param no_tdma If this option is set, no modification of the slave arbitrators will be performed. (Optional)
# @param source_code The location of the processor source code, which will be included as the -I option in the Nios II project Makefiles. (Required)


# import the required python modules
# os, shutil and sys for file and path accesses
# subprocess to run external commands and check their return values
# argparse to enable the usage of command line arguments
import os, subprocess, sys, shutil, argparse

sys.tracebacklimit = 3

## Reimplementation of the standard which tool in unix systems
#
#  @param program The program whose existence in the PATH is to be checked.
#  @return None, if the executable is available, the path and location of the executable if it is.
def which(program):
	import os
	def is_exe(fpath):
		return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

	fpath, fname = os.path.split(program)
	if fpath:
		if is_exe(program):
			return program
	else:
		for path in os.environ["PATH"].split(os.pathsep):
			path = path.strip('"')
			exe_file = os.path.join(path, program)
			if is_exe(exe_file):
				return exe_file
	return None


# add the lib folder containing the scripts imported below to the python search path
script_subfolder = os.path.join(os.getcwd(), "lib")
if script_subfolder not in sys.path:
	sys.path.insert(0, script_subfolder)

# import the required functions from the developed scripts
import prepare_vhdl
import create_sopc_model
import modify_arbiters
import create_nios_projects

# blessings module enables styles in the terminal output (bold print and colored output)
# if it is not found, exit the script
try:
	from blessings import Terminal
except:
	print "Module blessings not found..."
	sys.exit()

# create a terminal object from the blessings module to print styled text
t = Terminal()

# test if quartus_sh and sopc_builder are in the current path, otherwise complain and exit script
if which("quartus_sh") == None:
	print t.color(1)("Quartus executable not found...")
	sys.exit()
if which("sopc_builder") == None:
	print t.color(1)("SOPC Builder executable not found, maybe you need to start a Nios2 shell with nios2_command_shell.sh...")
	sys.exit()

# add the command line arguments
# project_name: the name of the project to be created
parser = argparse.ArgumentParser(description='Automatically create Quartus projects with given name.')
parser.add_argument('--project_name', 
							action='store', 
							default="", 
							help='The Quartus project name',
							required=True)

parser.add_argument('--system_description', 
							action='store', 
							default="", 
							help='Basic system description XML file',
							required=True)

parser.add_argument('--sopc', 
							action='store_true', 
							default=False, 
							help='Start SOPC Builder to modify system',
							required=False)

parser.add_argument('--no_tdma', 
							action='store_true', 
							default=False, 
							help='Do not modify arbiters',
							required=False)

parser.add_argument('--source_code', 
							action='store', 
							default="", 
							help='Location of C sources for each CPU',
							required=True)


args = parser.parse_args()

base_folder = "/home/psopc/Desktop/"

print "Creating project called: " + args.project_name

# set basic project properties
# get the project name from the command line arguments
# set the FPGA family and the device
project_name = args.project_name
fpga_family = "Cyclone IV E"
fpga_part = "EP4CE115F29C7"

# try to create the project folder
# if it already exists, delete it
try:
	os.makedirs(os.path.join(base_folder, project_name))
except os.error:
	# print t.color(1)("The project directory already exists...")
	print t.color(1)("The directory already exists, so it will be deleted.")
	shutil.rmtree(os.path.join(base_folder, project_name))
	os.makedirs(os.path.join(base_folder, project_name))

# copy the standard VHDL top level file into the newly created project folder
shutil.copyfile(os.path.join("lib", "common_files", "standard_top_level.vhd"), os.path.join(base_folder, project_name, project_name + ".vhd"))

# TODO: find out how to set the timing constraints, the line below does not have any effect
shutil.copyfile(os.path.join("lib", "common_files", "standard_constraints.tcl"), os.path.join(base_folder, project_name, project_name + ".sdc"))

print t.bold("Modifying top level VHDL file...\n")

# call the first script function to modify the top level VHDL file
# set the correct entity and architecture name as well as the name of the system component
prepare_vhdl.prepare_vhdl_file(os.path.join(base_folder, project_name, project_name + ".vhd"), project_name)

print t.bold("Creating Quartus project...\n")

# try to run quartus_sh --prepare, which creates an empty project with the properties set above
# if it returns a non-zero value, exit the script and print an error message
# in any case, the Quartus output is shown
try:
	p2 = subprocess.check_call(["quartus_sh", "--prepare","-f", fpga_family, "-d", fpga_part, os.path.join(base_folder, project_name, project_name)])
except subprocess.CalledProcessError:
	print t.color(1)("The Quartus project could not be created...")
	sys.exit()

print t.bold("Set assignments from file...")

# try to call the Quartus and run a TCL script which sets further project properties
# the output folder is set to output_files
# the pin location assignments are set

working_directory = os.getcwd()

os.chdir(os.path.join(base_folder, project_name))

try:
	p2 = subprocess.check_call(["quartus_sh", "-t", os.path.join(working_directory, "lib", "common_files", "standard_assignments.tcl"), project_name])
except subprocess.CalledProcessError:
	print t.color(1)("Assignments could not be set...")
	sys.exit()

os.chdir(working_directory)

print t.bold("Creating a SOPC Builder system and generating it...")

# call the second script function, which creates a SOPC Builder input file from a given basic architecture description
create_sopc_model.create(base_folder, project_name, args.system_description, fpga_part, fpga_family)

# call SOPC Builder to generate the newly created model
# if there is an error during generation, open SOPC Builder to fix it
print t.bold("SOPC Builder running in background...")
p = subprocess.Popen(["sopc_builder", os.path.join(base_folder, project_name, project_name + "_system.sopc"), "--no-splash", "--generate"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

output, error_text = p.communicate()
print "Output"
print output

# if the SOPC generation encountered an error, open the GUI to be able to fix it by hand
if "Error" in error_text:
	print error_text
	subprocess.call(["sopc_builder", os.path.join(base_folder, project_name, project_name + "_system.sopc")])

# if requested, open the SOPC Builder GUI
# if an error occurred, SOPC Builder will have been opened already
if not "error" in error_text:
	if args.sopc == True:
		print t.bold("Starting SOPC Builder as requested...")
		subprocess.call(["sopc_builder", os.path.join(base_folder, project_name, project_name + "_system.sopc")])

# if requested, do not modify the arbiters in the system
if args.no_tdma == False:
	print t.bold("Modifying the system file generated by SOPC Builder...")

	# call the third script, which modifies the system VHDL file generated by SOPC Builder
	# it takes the basic architecture description and the system VHDL file as input values
	# using the architecture description, it finds all slaves which are connected to more than one master
	# and modifies their arbiters
	modify_arbiters.modify(args.system_description, os.path.join(base_folder, project_name, project_name + "_system.vhd"))

print t.bold("Compiling and synthesizing the system...")

# try to run the Quartus compilation flow to fully compile the project and create the FPGA programming image in the output_files folder
try:
	p2 = subprocess.check_call(["quartus_sh", "--flow","compile", os.path.join(base_folder, project_name, project_name)])
except subprocess.CalledProcessError:
	print t.color(1)("The Quartus project could not be compiled...")
	sys.exit()

# print t.bold("Setting clock constraints...")
# os.chdir(project_name)
# try:
# 	p2 = subprocess.check_call(["quartus_sta", "-t", os.path.join("..", "input_files", "standard_constraints.tcl"), project_name])
# except subprocess.CalledProcessError:
# 	print t.color(1)("Constraints could not be set...")
# 	sys.exit()

# print t.bold("Running timing analyzer again...")

# try:
# 	p2 = subprocess.check_call(["quartus_sta", project_name, '-c', project_name])
# except subprocess.CalledProcessError:
# 	print t.color(1)("Timing analyzer failed to run...")
# 	sys.exit()
# os.chdir("..")

print t.bold("Programming the FPGA...")

try:
	p2 = subprocess.check_call(["nios2-configure-sof", os.path.join(base_folder, project_name, "output_files", project_name + ".sof")])
except subprocess.CalledProcessError:
	print t.color(1)("The programming did not succeed, you will have to run the following command manually:")
	print t.color(1)("\tnios2-configure-sof " + str(os.path.join(base_folder, project_name, "output_files", project_name + ".sof")))

print t.bold("Creating board support packages and projects for each CPU...")

# the fourth script creates BSP support packages and Nios2 projects from given C or C++ source files
create_nios_projects.create(base_folder, project_name, args.system_description, os.path.join(base_folder, project_name, project_name + "_system.sopc"), args.source_code)
