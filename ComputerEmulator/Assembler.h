/*  =============================================================

	ASSEMBLER
	---------------------------
	|  CPU  <-->  ASSEMBLER   |
	---------------------------

	Class for managing program execution inside CPU.
	CPU, Memory and Bus are attached to the system at startup.

	Entry Point: formatProgram()
		- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		> source.asm ----- formatProgram()
								   o
								   |
								   >  temp.txt  ( Formatting Operation I) : labels and routines  ( "_skip:" -> "label 0x0070:" )
										   o
										   |
										   >  tmp.txt ( Formatting Operation II) : replacing routine names with addresses ( "jmp _skip" -> "jmp 0x0070" )
												o
												|
												>  out.txt ( Formatting Operation III) : opcodes, addresses and immediates formatted ( "add A, 0x10" -> "74 10" )
		- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	   > out.txt ----getProgram()-----> program[]

	   > Auxiliary functions to format string and more

	   ==========================================================================
	   NOTES:
			> each program begins with "label 0x0000"
			> routines must begin with '_'  (ex. _func, _skip, _loop)
			> data section base address must be hardcoded
			> label, data & routine lines must not contain spaces or tabs at the beginning
			> each simple line must be indented with a single tab
	   ==========================================================================

	   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	   _________________________________________

	   PROGRAM EXAMBLE:  add.asm

		label 0x0000:
			mvi C, 0x00
			lxi H, 0x0070
			mov A, m
			inx H
			add m
			jnc _skip
			inr C

		_skip:
			inx H
			mov m, A
			inx H
			mov m, C
			hlt

		data 0x0070:
			FE FF
	    _______________________________________

	============================================================= */

#pragma once

#include "namespaceDeclaration.h"
#include "CPU.h"
#include "Memory.h"
#include <string>

#include <fstream>
#include <vector>
#include <algorithm>

class Assembler
{
private:
	/* CPU module */
	CPU* uProcessor;

	/* Sub_routine */
	struct routine{
		std::string name; // label
		uint16_t baseAddr; // address chosen at assembly time for each routine
	};

public:
	/* Constructor */
	Assembler(CPU* cpu);
	/* from a source code to a formatted binary */
	void formatProgram(std::string filePath, std::string outputPath);
	/* from formatted binary to a program layout ready to load in a memory sector */
	uint8_t* getProgram();

private:
	/* Auxiliary Functions */
	std::vector<std::string> splitString(std::string str, std::string str_delimiter);
	uint8_t fromStringToHex8(std::string str);
	uint16_t fromStringToHex16(std::string str);
	void printRegister16(uint16_t reg);
	std::string register16(uint16_t reg);
	bool checkExists(std::vector<routine> routines, routine x);
	int setRoutineParams(std::vector<routine>& routines, std::string name, uint16_t baseAddr);
	uint16_t getRoutineBaseAddress(std::vector<routine> routines, std::string name);
	std::string capitalizeString(std::string s);
};

