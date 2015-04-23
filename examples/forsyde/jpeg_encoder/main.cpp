/* 
 * File:   main.cpp
 * Author: marcus
 *
 * Created on March 29, 2013, 10:55 PM
 */

#include <cstdlib>
#include <stdio.h>
#include <cstring>
#define FORSYDE_INTROSPECTION

#include "headers/top.hpp"

int main(int argc, char** argv)
{
	std::cout << "Starting conversion..." << std::endl;

	Top top("top");
	sc_start();

	return 0;
}
