#include "Computer.h"

/* 
	Constructor 
*/
Computer::Computer() {
	this->cpu8085 = new CPU();
	this->ram = new Memory();
	this->bus = new Bus();

	/* Attach CPU and Memory to the System Bus */
	this->bus->attach(this->cpu8085, this->ram);
}

/* 
	Entry point for system execution 
*/
void Computer::run() {

	/* Clear Screen */
	system("cls");

	/* program source file name */
	std::string sourceFile;
	std::cout << "------ COMPUTER CLI ------------\n";
	std::cout << "> Enter Program Source File : ";
	std::cin >> sourceFile;

	/* Assembler Operation */
	std::cout << "> Assembler...\n";
	// new Assembler object to format source file.  [ source.txt -> out.txt ]
	Assembler* assembler = new Assembler(this->cpu8085);
	assembler->formatProgram(sourceFile, "out.txt");
	// Program code layout construction
	uint8_t* program;
	program = assembler->getProgram();
	std::cout << "> Assembly done.\n";

	/* MEMORY Program Load */
	this->ram->loadProgram(program, 0);  // load program in sector 0 : first PROGRAM_DIM bytes
	this->ram->print();

	/* 
		User input to proceed to execution 
		-> Only Single-step program execution
	*/
	char ans = 'N';
	std::cout << "> Do you want to proceed (Y/N)?\n";
	std::cout << "> You must type a 'Y' or an 'N' :";
	std::cin >> ans;

	if ((ans == 'Y') || (ans == 'y')) {
		/* CPU running... */
		this->cpu8085->run();
	}

	/* Execution halted. Computer Shutdown. */
	std::cout << "\n\n=============================\n";
	std::cout << "> Computer Shutdown.";
	std::cout << "\n=============================\n\n";
}
