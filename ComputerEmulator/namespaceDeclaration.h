#pragma once

class CPU;
class Bus;
class Memory;
class Assembler;

struct ControlSignals {
	// IO/Mbar, S0, S1, ALE, RDbar, WRbar
	unsigned char iom, S0, S1, ALE, RDbar, WRbar;
};

struct INSTRUCTION;
