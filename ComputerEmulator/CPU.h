/*  =============================================================

	CPU
	---------------------------
	|  CPU  <-->  Bus  |
	---------------------------

	Class for managing program execution inside CPU.
	CPU, Memory and Bus are attached to the system at startup.

	Entry Point: Function for running the CPU [run()] is available.
		> Internal Architecture: 
	    _________________________________________________________________________________________
	   |   ______________________________                _                 ___________________   |
	   |  | Reg A : 0x00  |  temp : 0x00 |<------------>| |<------------->| Interrupt Control |--|--->
	   |  |_______________|______________|              | |               |___________________|  |
	   |          |               |                     | |                ___________________   |
	   |         _|_______________|__                   | |<------------->| Serial IO Control |--|--->
	   |        |        ALU         |----------------->| |               |___________________|  |
	   |        |____________________|    ___________   | |        __________________________    |
	   |                          L------| FR : 0x00 |--| |       |                          |   |
	   |                                 |___________|  | |       |   Timing & Control       |   |
	   |   ______________________________               | |       |                          |   |
	   |  | Reg B : 0x00  |  RegC : 0x00 |              | |       |                          |   |
	   |  | Reg D : 0x00  |  RegE : 0x00 |<------------>| |       |__________________________|   |
	   |  | Reg H : 0x00  |  RegL : 0x00 |              | |                         ____|____    |
	   |  | SP :  0x0000  |              |          INTERNAL BUS   ___________     | Decode  |   |
	   |  | PC :  0x0000  |              |	            | |------>| IR : 0x00 |----| Logic   |   |
	   |  |_______________|______________|              | |       |___________|    |_________|   |
	   |                        |     |_________________| |                                      |
	   |                        |                |      |_|                                      |
	   |                   _____|________________|_________                                      |
	   |                  | AD0-AD7 Buffer | A8-AF Buffer  |                                     |
	   |__________________|________________|_______________|_____________________________________|
	                               |               |
		       	               v               v


	   > Machine Cycles: Fetch(OFMC), Execution(Execute), MemoryRead(MRMC), MemoryWrite(MWMC)
	   > Auxiliary functions to link to system bus and to print
	   > Control Signals : S0, S1, RDbar, WRbar, iom, ALE
	   > Addressing Modes (NOT USED)
	   > Instructions Implementation
				lookup table : vector<INSTRUCTION>
					INSTRUCTION : { name, *func, *addressingMode(NOT USED), #bytesInMemory }
	   > Utility & Helper functions to keep the instruction implementation clean

	- - - - - - - - - - - - - - - - - -
	ex.  Computer pc = Computer();  ->  pc.run()
	- - - - - - - - - - - - - - - -        o
					       |
					       >  cpu->run()  [loop]
					      - - - - - - - - - - - - -
	=============================================================*/

#pragma once

#include "namespaceDeclaration.h"
#include "Bus.h"

#include <vector>

struct INSTRUCTION {
	std::string name;
	uint8_t(CPU::* operate)(void) = nullptr;
	uint8_t(CPU::* addrmode)(void) = nullptr;
	uint8_t bytes = 0;
};

class CPU
{
private:
	bool endProgram = false; // HLT state

	uint16_t PC;   // Program Counter PC
	uint16_t SP;   // Stack Pointer SP
	uint8_t  IR;   // Instruction Register IR
	uint8_t  FR;   // Flag Register FR  (7: S, 6: Z, , 4: AC, , 2: P, , 0: CY)

	uint8_t regA;  // Register A
	uint8_t regB;  // Register B
	uint8_t regC;  // Register C
	uint8_t regD;  // Register D
	uint8_t regE;  // Register D
	uint8_t regH;  // Register H
	uint8_t regL;  // Register L

	uint8_t temp;  // Register Temp

	uint8_t AddrDataBuffer;  // Addr/Data Buffer (tri-state interface with AD0-AD7)
	uint8_t HighAddrBuffer;  // Higher byte of address (tri-state interface with A8-A15)

	Bus* bus;  // system bus module

public:
	/* Constructor */
	CPU();
	/* Execution entry point */
	void run();
	
	/* Cycles and procedures */
	void OFMC();  // Opcode fetch Machine Cycle
	void execute();
	void MRMC(uint16_t ADDR, bool isFromPC);  // Memory Read Machine Cycle
	void MWMC(uint16_t ADDR, uint8_t DATA);   // Memory Write Machine Cycle

	/* Link to system bus */
	void attachBus(Bus* bus);

	/* Auxiliary Functions */
	void print();

	/* Control Signals */
	ControlSignals CTRL_SIG;

	/* Addressing modes (NOT USED!) */
	uint8_t IMM();    uint8_t REG();     // Immediate,  Register
	uint8_t DIR();    uint8_t IND();     // Direct,     Indirect
	uint8_t IMP();                       // Implied

	/* Instruction opcodes =============================================== */

	/* > Data Transfer Group */
	uint8_t MOV();    uint8_t MVI();    uint8_t LXI();
	uint8_t LDA();    uint8_t STA();
	uint8_t LHLD();   uint8_t SHLD();
	uint8_t LDAX();   uint8_t STAX();   uint8_t XCHG();
	uint8_t XXX();    

	/* > Data Manipulation Group - Arithmetic */
	uint8_t ADD();    uint8_t ADI();    uint8_t ADC();    uint8_t ACI();
	uint8_t SUB();    uint8_t SUI();    uint8_t SBB();    uint8_t SBI();
	uint8_t INR();    uint8_t DCR();    uint8_t DAD();
	uint8_t INX();    uint8_t DCX();
	uint8_t DAA();    uint8_t CMA();    uint8_t CMC();    uint8_t STC();

	/* > Data Manipulation Group - Logical */
	uint8_t ANA();    uint8_t ORA();   uint8_t XRA();    uint8_t CMP();
	uint8_t RLC();    uint8_t RRC();   uint8_t RAL();    uint8_t RAR();
	uint8_t ANI();    uint8_t XRI();   uint8_t ORI();    uint8_t CPI();

	/* > Transfer Control Group / Branch Group */
	uint8_t JMP();    uint8_t JNZ();   uint8_t JZ();     uint8_t JNC();    uint8_t JC();     uint8_t JPO();   uint8_t JPE();    uint8_t JP();     uint8_t JM();
	uint8_t CALL();   uint8_t CNZ();   uint8_t CZ();     uint8_t CNC();    uint8_t CC();     uint8_t CPO();   uint8_t CPE();    uint8_t CP();     uint8_t CM();
	uint8_t RET();    uint8_t RNZ();   uint8_t RZ();     uint8_t RNC();    uint8_t RC();     uint8_t RPO();   uint8_t RPE();    uint8_t RP();     uint8_t RM();
	uint8_t PCHL();      

	/* > Input / Output Group */

	/* > Stack And Machine Control Group */
	uint8_t NOP();    uint8_t HLT();
	uint8_t RST();

	/* ==================================================================== */

	/* Instruction lookup table */
	std::vector<INSTRUCTION> lookup;

private:
	/* Utility Functions */
	void MOV_utility(uint8_t& reg, uint8_t offset);
	void MOVm_utility(uint8_t offset);

	/* Helper Functions */
	uint16_t getMfromRegPair(uint8_t reg1, uint8_t reg2);
	void printRegister16(uint16_t reg);
	void printRegister8(uint8_t reg);
};

