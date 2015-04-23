# !/usr/bin/python
# -*- coding: utf-8 -*-

## 
#
# @file
# This script modifies the standard VHDL top level file in the input_files folder.
# 
# It replaces all occurrences of the entity name with the project name
# and the name of the SOPC system component with <TT>project_name + "_system"</TT>. It uses the <TT>fileinput</TT> python module for 
# direct in-file replacing of text
# @copyright BSD New
# @author Marcus Mikulcak
# 
# @param vhdl_input The VHDL top level file.
# @param project_name The project name.

# propagate the functions offered by this module
__all__ = ["prepare_vhdl_file"]

# the fileinput module enables direct replacing in a file
import fileinput

## This function performs the work in this script, preparing the standard VHDL top level file to be used in the newly created project.
# 
# @param vhdl_input The standard VHDL top level file.
# @param project_name The name of the project.
def prepare_vhdl_file(vhdl_input, project_name):

	# use the fileinput to open the input VHDL file and set the in-place replacement option
	vhdl_input_file = fileinput.input(vhdl_input, inplace=1)
	# iterate over every line of the input file
	for line in vhdl_input_file:
		# replace the entity name with the project name
		if "top_level_entity" in line:
			line = line.replace("top_level_entity", project_name)
		# replace the SOPC system component name with project_name + "_system"
		if "sopc_system" in line:
			line = line.replace("sopc_system", project_name + "_system")
		print line,
	# finish operation and close the input file
	vhdl_input_file.close()