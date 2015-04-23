## \file 
# \brief This script sets clocking information and constraints in the Quartus project.

load_package sta


project_open [lindex $quartus(args) 0]

# Clock constraints

create_timing_netlist

create_clock -name "CLOCK_50" -period 20.000ns [get_ports {CLOCK_50}]


# Automatically constrain PLL and other generated clocks
derive_pll_clocks -create_base_clocks

# Automatically calculate clock uncertainty to jitter and other effects.
derive_clock_uncertainty

# tsu/th constraints

# tco constraints

# tpd constraints
project_close