/*  =============================================================

	SYSTEM BUS
	---------------------------------
	|  CPU  <-->  Bus  <-->  Memory |
	---------------------------------

	Class for CPU - Memory interfacing.
	CPU, Memory and Bus are attached to the system at startup.

	- - - - - - - - - - - - - - - - - - - - -
	Bus :  16 bit : [0000 0000  0000 0000]
					     |          |
				    Address[8-F]   Multiplexed Address/Data [0-7]
	- - - - - - - - - - - - - - - - - - - - -

	Function for attaching modules is available  [attach()].
	Functions for putting content[load()] in the bus and retrieving data[getData()] from the bus.
	Auxiliary functions for printing memory and bus content.

	=============================================================*/

#pragma once

#include <string>
#include <cstdint>
#include "namespaceDeclaration.h"
#include "Memory.h"
#include "CPU.h"

class Bus
{
private:
	/* AD0 - AD7 */
	uint8_t multiplexedWord;
	/* A8  - A15 */
	uint8_t addressBusHigh;  

	CPU* cpu;
	Memory* ram;

public:
	/* Constructor */
	Bus();
	/* Attach Modules to bus */
	void attach(CPU* cpu, Memory* ram);
	/* Put address/data in the bus */
	void load(uint16_t ADDR);
	/* Retrieve data from Data Bus (AD0-AD7) */
	uint8_t getData();

	/* Auxiliary Functions */
	void printMemory();
	void print();
};

