/*  =============================================================
	
	COMPUTER 
	---------------------------
	|  CPU  |  Bus  |  Memory |
	---------------------------

	Class for managing program execution.
	CPU, Memory and Bus are attached to the system at startup.
	Function for running the system [run()] is available.

	- - - - - - - - - - - - - - - - - -
	ex.  Computer pc = Computer();  ->  pc.run()
	- - - - - - - - - - - - - - - -        o
										   |
										   >  cpu->run()  [loop]
	                             - - - - - - - - - - - - -
    =============================================================*/

#pragma once

#include <cstdint>
#include "namespaceDeclaration.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include "Assembler.h"

class Computer
{
private:
	/* CPU, Memory, Bus modules */
	CPU* cpu8085;
	Memory* ram;
	Bus* bus;

public:
	/* Constructor */
	Computer();
	/* Entry point for system execution */
	void run();
};

