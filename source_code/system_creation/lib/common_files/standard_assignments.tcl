## \file standard_assignments.tcl
# \brief This script sets a number of assignments in the created Quartus project.
# \details These assignments include the setting of the output folder to <TT>output</TT> to make it easier to find created files and
# and the location assignments for the FPGA device.


load_package flow
# Create the project and overwrite any settings
# files that exist
# project_new fir_filter -revision filtref -overwrite
# Set the device, the name of the top-level BDF,
# and the name of the top level entity
# set_global_assignment -name FAMILY Cyclone
# set_global_assignment -name DEVICE EP1C6F256C6
# set_global_assignment -name BDF_FILE filtref.bdf
# set_global_assignment -name TOP_LEVEL_ENTITY filtref
# Add other pin assignments here

project_open [lindex $quartus(args) 0]

set_global_assignment -name PROJECT_OUTPUT_DIRECTORY output_files


#============================================================
# CLOCK
#============================================================
set_location_assignment PIN_Y2 -to CLOCK_50
# set_location_assignment PIN_AG14 -to CLOCK2_50
# set_location_assignment PIN_AG15 -to CLOCK3_50
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to CLOCK_50
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to CLOCK2_50
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to CLOCK3_50

#============================================================
# LED
#============================================================
set_location_assignment PIN_E21 -to LEDG[0]
set_location_assignment PIN_E22 -to LEDG[1]
set_location_assignment PIN_E25 -to LEDG[2]
set_location_assignment PIN_E24 -to LEDG[3]
set_location_assignment PIN_H21 -to LEDG[4]
set_location_assignment PIN_G20 -to LEDG[5]
set_location_assignment PIN_G22 -to LEDG[6]
set_location_assignment PIN_G21 -to LEDG[7]
set_location_assignment PIN_F17 -to LEDG[8]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDG[0]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDG[1]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDG[2]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDG[3]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDG[4]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDG[5]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDG[6]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDG[7]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDG[8]

set_location_assignment PIN_G19 -to LEDR[0]
set_location_assignment PIN_E19 -to LEDR[2]
set_location_assignment PIN_F19 -to LEDR[1]
set_location_assignment PIN_F21 -to LEDR[3]
set_location_assignment PIN_F18 -to LEDR[4]
set_location_assignment PIN_E18 -to LEDR[5]
set_location_assignment PIN_J19 -to LEDR[6]
set_location_assignment PIN_H19 -to LEDR[7]
set_location_assignment PIN_J17 -to LEDR[8]
set_location_assignment PIN_G17 -to LEDR[9]
set_location_assignment PIN_J15 -to LEDR[10]
set_location_assignment PIN_H16 -to LEDR[11]
set_location_assignment PIN_J16 -to LEDR[12]
set_location_assignment PIN_H17 -to LEDR[13]
set_location_assignment PIN_F15 -to LEDR[14]
set_location_assignment PIN_G15 -to LEDR[15]
set_location_assignment PIN_G16 -to LEDR[16]
set_location_assignment PIN_H15 -to LEDR[17]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[0]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[1]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[3]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[4]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[5]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[6]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[7]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[8]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[9]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[10]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[11]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[12]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[13]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[14]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[15]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[16]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to LEDR[17]

#============================================================
# KEY
#============================================================
set_location_assignment PIN_M23 -to KEY[0]
set_location_assignment PIN_M21 -to KEY[1]
set_location_assignment PIN_N21 -to KEY[2]
set_location_assignment PIN_R24 -to KEY[3]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to KEY[0]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to KEY[1]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to KEY[2]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to KEY[3]

#============================================================
# SW
#============================================================
# set_location_assignment PIN_AB28 -to SW[0]
# set_location_assignment PIN_AC28 -to SW[1]
# set_location_assignment PIN_AC27 -to SW[2]
# set_location_assignment PIN_AD27 -to SW[3]
# set_location_assignment PIN_AB27 -to SW[4]
# set_location_assignment PIN_AC26 -to SW[5]
# set_location_assignment PIN_AD26 -to SW[6]
# set_location_assignment PIN_AB26 -to SW[7]
# set_location_assignment PIN_AC25 -to SW[8]
# set_location_assignment PIN_AB25 -to SW[9]
# set_location_assignment PIN_AC24 -to SW[10]
# set_location_assignment PIN_AB24 -to SW[11]
# set_location_assignment PIN_AB23 -to SW[12]
# set_location_assignment PIN_AA24 -to SW[13]
# set_location_assignment PIN_AA23 -to SW[14]
# set_location_assignment PIN_AA22 -to SW[15]
# set_location_assignment PIN_Y24 -to SW[16]
# set_location_assignment PIN_Y23 -to SW[17]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[0]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[1]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[2]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[3]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[4]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[5]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[6]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[7]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[8]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[9]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[10]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[11]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[12]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[13]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[14]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[15]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[16]
# set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[17]

#============================================================
# SEG7
#============================================================
set_location_assignment PIN_G18 -to HEX0[0]
set_location_assignment PIN_F22 -to HEX0[1]
set_location_assignment PIN_E17 -to HEX0[2]
set_location_assignment PIN_L26 -to HEX0[3]
set_location_assignment PIN_L25 -to HEX0[4]
set_location_assignment PIN_J22 -to HEX0[5]
set_location_assignment PIN_H22 -to HEX0[6]
set_location_assignment PIN_M24 -to HEX1[0]
set_location_assignment PIN_Y22 -to HEX1[1]
set_location_assignment PIN_W21 -to HEX1[2]
set_location_assignment PIN_W22 -to HEX1[3]
set_location_assignment PIN_W25 -to HEX1[4]
set_location_assignment PIN_U23 -to HEX1[5]
set_location_assignment PIN_U24 -to HEX1[6]
set_location_assignment PIN_AA25 -to HEX2[0]
set_location_assignment PIN_AA26 -to HEX2[1]
set_location_assignment PIN_Y25 -to HEX2[2]
set_location_assignment PIN_W26 -to HEX2[3]
set_location_assignment PIN_Y26 -to HEX2[4]
set_location_assignment PIN_W27 -to HEX2[5]
set_location_assignment PIN_W28 -to HEX2[6]
set_location_assignment PIN_V21 -to HEX3[0]
set_location_assignment PIN_U21 -to HEX3[1]
set_location_assignment PIN_AB20 -to HEX3[2]
set_location_assignment PIN_AA21 -to HEX3[3]
set_location_assignment PIN_AD24 -to HEX3[4]
set_location_assignment PIN_AF23 -to HEX3[5]
set_location_assignment PIN_Y19 -to HEX3[6]
set_location_assignment PIN_AB19 -to HEX4[0]
set_location_assignment PIN_AA19 -to HEX4[1]
set_location_assignment PIN_AG21 -to HEX4[2]
set_location_assignment PIN_AH21 -to HEX4[3]
set_location_assignment PIN_AE19 -to HEX4[4]
set_location_assignment PIN_AF19 -to HEX4[5]
set_location_assignment PIN_AE18 -to HEX4[6]
set_location_assignment PIN_AD18 -to HEX5[0]
set_location_assignment PIN_AC18 -to HEX5[1]
set_location_assignment PIN_AB18 -to HEX5[2]
set_location_assignment PIN_AH19 -to HEX5[3]
set_location_assignment PIN_AG19 -to HEX5[4]
set_location_assignment PIN_AF18 -to HEX5[5]
set_location_assignment PIN_AH18 -to HEX5[6]
set_location_assignment PIN_AA17 -to HEX6[0]
set_location_assignment PIN_AB16 -to HEX6[1]
set_location_assignment PIN_AA16 -to HEX6[2]
set_location_assignment PIN_AB17 -to HEX6[3]
set_location_assignment PIN_AB15 -to HEX6[4]
set_location_assignment PIN_AA15 -to HEX6[5]
set_location_assignment PIN_AC17 -to HEX6[6]
set_location_assignment PIN_AD17 -to HEX7[0]
set_location_assignment PIN_AE17 -to HEX7[1]
set_location_assignment PIN_AG17 -to HEX7[2]
set_location_assignment PIN_AH17 -to HEX7[3]
set_location_assignment PIN_AF17 -to HEX7[4]
set_location_assignment PIN_AG18 -to HEX7[5]
set_location_assignment PIN_AA14 -to HEX7[6]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX0[0]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX0[1]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX0[2]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX0[3]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX0[4]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX0[5]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX0[6]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX1[0]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX1[1]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX1[2]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX1[3]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX1[4]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX1[5]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX1[6]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX2[0]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX2[1]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX2[2]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX2[3]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX2[4]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX2[5]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX2[6]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX3[0]
set_instance_assignment -name IO_STANDARD "2.5 V" -to HEX3[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX3[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX3[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX3[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX3[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX3[6]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX4[6]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX5[6]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX6[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX6[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX6[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX6[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX6[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX6[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX6[6]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX7[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX7[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX7[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX7[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX7[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX7[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to HEX7[6]

#============================================================
# LCD
#============================================================
# set_location_assignment PIN_L6 -to LCD_BLON
# set_location_assignment PIN_M5 -to LCD_DATA[7]
# set_location_assignment PIN_M3 -to LCD_DATA[6]
# set_location_assignment PIN_K2 -to LCD_DATA[5]
# set_location_assignment PIN_K1 -to LCD_DATA[4]
# set_location_assignment PIN_K7 -to LCD_DATA[3]
# set_location_assignment PIN_L2 -to LCD_DATA[2]
# set_location_assignment PIN_L1 -to LCD_DATA[1]
# set_location_assignment PIN_L3 -to LCD_DATA[0]
# set_location_assignment PIN_L4 -to LCD_EN
# set_location_assignment PIN_M1 -to LCD_RW
# set_location_assignment PIN_M2 -to LCD_RS
# set_location_assignment PIN_L5 -to LCD_ON
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LCD_DATA[0]
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LCD_DATA[1]
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LCD_DATA[2]
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LCD_DATA[3]
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LCD_DATA[4]
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LCD_DATA[5]
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LCD_DATA[6]
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LCD_DATA[7]
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LCD_BLON
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LCD_RW
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LCD_EN
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LCD_RS
# set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LCD_ON

#============================================================
# SRAM
#============================================================
set_location_assignment PIN_AG3 -to SRAM_DQ[15]
set_location_assignment PIN_AF3 -to SRAM_DQ[14]
set_location_assignment PIN_AE4 -to SRAM_DQ[13]
set_location_assignment PIN_AE3 -to SRAM_DQ[12]
set_location_assignment PIN_AE1 -to SRAM_DQ[11]
set_location_assignment PIN_AE2 -to SRAM_DQ[10]
set_location_assignment PIN_AD2 -to SRAM_DQ[9]
set_location_assignment PIN_AD1 -to SRAM_DQ[8]
set_location_assignment PIN_AF7 -to SRAM_DQ[7]
set_location_assignment PIN_AH6 -to SRAM_DQ[6]
set_location_assignment PIN_AG6 -to SRAM_DQ[5]
set_location_assignment PIN_AF6 -to SRAM_DQ[4]
set_location_assignment PIN_AH4 -to SRAM_DQ[3]
set_location_assignment PIN_AG4 -to SRAM_DQ[2]
set_location_assignment PIN_AF4 -to SRAM_DQ[1]
set_location_assignment PIN_AH3 -to SRAM_DQ[0]
set_location_assignment PIN_AC4 -to SRAM_UB_N
set_location_assignment PIN_AD4 -to SRAM_LB_N
set_location_assignment PIN_AF8 -to SRAM_CE_N
set_location_assignment PIN_AD5 -to SRAM_OE_N
set_location_assignment PIN_AE8 -to SRAM_WE_N
set_location_assignment PIN_AE6 -to SRAM_ADDR[5]
set_location_assignment PIN_AB5 -to SRAM_ADDR[6]
set_location_assignment PIN_AC5 -to SRAM_ADDR[7]
set_location_assignment PIN_AF5 -to SRAM_ADDR[8]
set_location_assignment PIN_T7 -to SRAM_ADDR[9]
set_location_assignment PIN_AF2 -to SRAM_ADDR[10]
set_location_assignment PIN_AD3 -to SRAM_ADDR[11]
set_location_assignment PIN_AB4 -to SRAM_ADDR[12]
set_location_assignment PIN_AC3 -to SRAM_ADDR[13]
set_location_assignment PIN_AA4 -to SRAM_ADDR[14]
set_location_assignment PIN_AB7 -to SRAM_ADDR[0]
set_location_assignment PIN_AD7 -to SRAM_ADDR[1]
set_location_assignment PIN_AE7 -to SRAM_ADDR[2]
set_location_assignment PIN_AC7 -to SRAM_ADDR[3]
set_location_assignment PIN_AB6 -to SRAM_ADDR[4]
set_location_assignment PIN_T8 -to SRAM_ADDR[19]
set_location_assignment PIN_AB8 -to SRAM_ADDR[18]
set_location_assignment PIN_AB9 -to SRAM_ADDR[17]
set_location_assignment PIN_AC11 -to SRAM_ADDR[16]
set_location_assignment PIN_AB11 -to SRAM_ADDR[15]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[6]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[7]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[8]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[9]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[10]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[11]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[12]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[13]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[14]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[15]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[16]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[17]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[18]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_ADDR[19]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[6]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[7]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[8]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[9]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[10]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[11]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[12]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[13]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[14]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_DQ[15]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_UB_N
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_LB_N
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_CE_N
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_OE_N
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SRAM_WE_N

# compile the project
# execute_flow -compile

project_close
