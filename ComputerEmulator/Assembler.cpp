#include "Assembler.h"

/* 
	Constructor
*/
Assembler::Assembler(CPU* cpu) {
	// __init__
	this->uProcessor = cpu;
}

/* 
	from a source code to a formatted binary 
*/
void Assembler::formatProgram(std::string filePath, std::string outputPath) {
	// file Read
	std::ifstream fpR;
	// file Output/Write
	std::ofstream fpW;

	/* 
		Formatting Operation 1 : labels and routines  ================================================================================ 
	*/
	fpR.open(filePath);
	fpW.open("temp.txt");

	// Routine Vector
	std::vector<routine> routines{};

	if (fpR.is_open()) {
		bool firstline = true;
		bool dataSection = false;

		// bytes allocated
		int bytesAllocated = 0;

		std::string line;
		std::vector<std::string> params{};

		/*
			for each <line>:
				> label line  : write without modify
				> routine line: save routine in vector <routines> if new  (name, baseAddress)
				> data line   : write without modify
				> simple line :   - search for instruction length in lookup table of the CPU and add to the bytesAllocated variable
				                  -if there is a routine involved save it in vector <routines>
		*/
		while (getline(fpR, line)) {
			// --- label 0x0000 ---
			std::size_t labelLine = line.find("label");
			// --- data: ---
			std::size_t dataLine = line.find("data");
			// --- _func: ---
			bool routineLine = (line[0] == '_') ? true : false;

			if (labelLine != std::string::npos) {
				if (firstline == false) fpW << std::endl;
				fpW << line << std::endl;
				firstline = false;
				dataSection = false;
			}
			else if (routineLine == 1) {
				uint16_t offset = (uint16_t)(bytesAllocated);
				std::string str = splitString(line, " ")[0];
				std::string name = str.substr(0, str.length()-1);
				if (setRoutineParams(routines, name, offset) == -1) {
					routine x;
					x.name = name;
					routines.push_back(x);
					setRoutineParams(routines, name, offset);
				}
				fpW << "label 0x" << register16(offset) << std::endl;
				dataSection = false;
			}
			else if (dataLine != std::string::npos) {
				if (firstline == false) fpW << std::endl;
				fpW << line << std::endl;
				firstline = false;
				dataSection = true;
			}
			else if (line.length() == 0) {
				// Do Nothing
			}
			else {
				line.erase(0, 1);  // tab removed
				params = splitString(line, " ");

				/*
				*  optimization: optimal code layout in memory
				*/
				if (dataSection == false) {
					std::string instr = capitalizeString(params.front());
					struct match {
						match(std::string i) : _instr(i) {}

						bool operator()(INSTRUCTION instr) {
							return (instr.name == _instr);
						}

						std::string _instr;
					};
					std::vector<INSTRUCTION>::iterator it = std::find_if(uProcessor->lookup.begin(), uProcessor->lookup.end(), match(instr));
					bytesAllocated += it->bytes;
				}

				for (std::string str : params) {
					if (str[0] == '_') {
						routine x;
						x.name = str;
						if (!checkExists(routines, x)) {
							routines.push_back(x);
						}
					}
					fpW << str << " ";
				}
				fpW << std::endl;
			}
		}

		for (routine r : routines) {
			std::cout << r.name << std::endl;
			std::cout << register16(r.baseAddr) << std::endl;
		}
	}

	fpR.close();
	fpW.close();

	/* 
		Formatting Operation 2 : temp.txt -> tmp.txt ============================================================================ 
	*/
	fpR.open("temp.txt");
	fpW.open("tmp.txt");

	if (fpR.is_open()) {

		std::string line;
		std::vector<std::string> params{};

		/*
			for each line:
				> label, routine or data line : write without modify
				> simple line :  if a routine label is present -> substitute with routine base Address saved previously in vector <routines>
		*/
		while (getline(fpR, line)) {

			// --- label 0x0000 ---
			std::size_t labelLine = line.find("label");
			// --- data: ---
			std::size_t dataLine = line.find("data");
			// --- _func: ---
			bool routineLine = (line[0] == '_') ? true : false;

			if (labelLine != std::string::npos) {
				fpW << line << std::endl;
			}
			else if (routineLine == 1) {
				fpW << line << std::endl;
			}
			else if (dataLine != std::string::npos) {
				fpW << line << std::endl;
			}
			else if (line.length() == 0) {
				fpW << line << std::endl;
			}
			else {
				params = splitString(line, " ");
				for (std::string str : params) {
					if (str[0] == '_') {
						routine x;
						x.name = str;
						if (checkExists(routines, x)) {
							fpW << "0x" << register16(getRoutineBaseAddress(routines, x.name)) << " ";
						}
						else {
							std::cout << "\n\n--- Error --- ROUTINE_FORMATTING_ERROR \n\n";
						}
					}
					else {
						fpW << str << " ";
					}
				}
				fpW << std::endl;
			}
		}

		fpR.close();
		fpW.close();
	}

	/* remove temp.txt file. */
	remove("temp.txt");

	/* 
		Formatting Operations III ==========================================================================================
	*/
	fpR.open("tmp.txt");
	fpW.open("out.txt");

	// Bytes written
	int bytesWritten = 0;

	if (fpR.is_open()) {

		bool firstline = true;
		bool dataSection = false;

		std::string line;

		/*
			for each line:
				> label, routine or data line : write without modify
				> simple line :  - in data section : write without modify
				                 - in code section : replace instructions with opcodes and format addresses and immediate values
		*/
		while (getline(fpR, line)) {
			std::cout << line << std::endl;

			// --- label 0x0000 ---
			std::size_t labelLine = line.find("label");
			// --- data: ---
			std::size_t dataLine = line.find("data");
			// --- _func: ---
			bool routineLine = (line[0] == '_') ? true : false;

			if (labelLine != std::string::npos) {
				if (firstline == false) fpW << std::endl;
				fpW << line << std::endl;
				firstline = false;
				dataSection = false;
				bytesWritten = 0;
			}
			else if (routineLine == 1) {
				fpW << line << std::endl;
				dataSection = false;
				bytesWritten = 0;
			}
			else if (dataLine != std::string::npos) {
				if (firstline == false) fpW << std::endl;
				fpW << line << std::endl;
				firstline = false;
				dataSection = true;
				bytesWritten = 0;
			}
			else if (line.length() == 0) {
				// Do Nothing
			}
			else {  // Instruction Line
				std::vector<std::string> params{};
				// Line Splitted
				params = splitString(line, " ");

				// ====== INSTRUCTION HANDLING =====

				if (dataSection == true) {
					fpW << line << std::endl;
				}
				else {
					/* --- INSTRUCTION ---*/
					std::string instr = params.front();

					if (instr == "mov") {    // ================================ MOV ================================
						char operands[2];
						operands[0] = params.begin()[1][0];
						operands[1] = params.begin()[2][0];
						if (operands[0] == 'A' && operands[1] == 'A') fpW << "7F ";      // A, X
						else if (operands[0] == 'A' && operands[1] == 'B') fpW << "78 ";
						else if (operands[0] == 'A' && operands[1] == 'C') fpW << "79 ";
						else if (operands[0] == 'A' && operands[1] == 'D') fpW << "7A ";
						else if (operands[0] == 'A' && operands[1] == 'E') fpW << "7B ";
						else if (operands[0] == 'A' && operands[1] == 'H') fpW << "7C ";
						else if (operands[0] == 'A' && operands[1] == 'L') fpW << "7D ";
						else if (operands[0] == 'A' && operands[1] == 'm') fpW << "7E ";
						else if (operands[0] == 'B' && operands[1] == 'A') fpW << "47 "; // B, X
						else if (operands[0] == 'B' && operands[1] == 'B') fpW << "40 ";
						else if (operands[0] == 'B' && operands[1] == 'C') fpW << "41 ";
						else if (operands[0] == 'B' && operands[1] == 'D') fpW << "42 ";
						else if (operands[0] == 'B' && operands[1] == 'E') fpW << "43 ";
						else if (operands[0] == 'B' && operands[1] == 'H') fpW << "44 ";
						else if (operands[0] == 'B' && operands[1] == 'L') fpW << "45 ";
						else if (operands[0] == 'B' && operands[1] == 'm') fpW << "46 ";
						else if (operands[0] == 'C' && operands[1] == 'A') fpW << "4F "; // C, X
						else if (operands[0] == 'C' && operands[1] == 'B') fpW << "48 ";
						else if (operands[0] == 'C' && operands[1] == 'C') fpW << "49 ";
						else if (operands[0] == 'C' && operands[1] == 'D') fpW << "4A ";
						else if (operands[0] == 'C' && operands[1] == 'E') fpW << "4B ";
						else if (operands[0] == 'C' && operands[1] == 'H') fpW << "4C ";
						else if (operands[0] == 'C' && operands[1] == 'L') fpW << "4D ";
						else if (operands[0] == 'C' && operands[1] == 'm') fpW << "4E ";
						else if (operands[0] == 'D' && operands[1] == 'A') fpW << "57 "; // D, X
						else if (operands[0] == 'D' && operands[1] == 'B') fpW << "50 ";
						else if (operands[0] == 'D' && operands[1] == 'C') fpW << "51 ";
						else if (operands[0] == 'D' && operands[1] == 'D') fpW << "52 ";
						else if (operands[0] == 'D' && operands[1] == 'E') fpW << "53 ";
						else if (operands[0] == 'D' && operands[1] == 'H') fpW << "54 ";
						else if (operands[0] == 'D' && operands[1] == 'L') fpW << "55 ";
						else if (operands[0] == 'D' && operands[1] == 'm') fpW << "56 ";
						else if (operands[0] == 'E' && operands[1] == 'A') fpW << "5F "; // E, X
						else if (operands[0] == 'E' && operands[1] == 'B') fpW << "58 ";
						else if (operands[0] == 'E' && operands[1] == 'C') fpW << "59 ";
						else if (operands[0] == 'E' && operands[1] == 'D') fpW << "5A ";
						else if (operands[0] == 'E' && operands[1] == 'E') fpW << "5B ";
						else if (operands[0] == 'E' && operands[1] == 'H') fpW << "5C ";
						else if (operands[0] == 'E' && operands[1] == 'L') fpW << "5D ";
						else if (operands[0] == 'E' && operands[1] == 'm') fpW << "5E ";
						else if (operands[0] == 'H' && operands[1] == 'A') fpW << "67 "; // H, X
						else if (operands[0] == 'H' && operands[1] == 'B') fpW << "60 ";
						else if (operands[0] == 'H' && operands[1] == 'C') fpW << "61 ";
						else if (operands[0] == 'H' && operands[1] == 'D') fpW << "62 ";
						else if (operands[0] == 'H' && operands[1] == 'E') fpW << "63 ";
						else if (operands[0] == 'H' && operands[1] == 'H') fpW << "64 ";
						else if (operands[0] == 'H' && operands[1] == 'L') fpW << "65 ";
						else if (operands[0] == 'H' && operands[1] == 'm') fpW << "66 ";
						else if (operands[0] == 'L' && operands[1] == 'A') fpW << "6F "; // L, X
						else if (operands[0] == 'L' && operands[1] == 'B') fpW << "68 ";
						else if (operands[0] == 'L' && operands[1] == 'C') fpW << "69 ";
						else if (operands[0] == 'L' && operands[1] == 'D') fpW << "6A ";
						else if (operands[0] == 'L' && operands[1] == 'E') fpW << "6B ";
						else if (operands[0] == 'L' && operands[1] == 'H') fpW << "6C ";
						else if (operands[0] == 'L' && operands[1] == 'L') fpW << "6D ";
						else if (operands[0] == 'L' && operands[1] == 'm') fpW << "6E ";
						else if (operands[0] == 'm' && operands[1] == 'A') fpW << "77 "; // m, X
						else if (operands[0] == 'm' && operands[1] == 'B') fpW << "70 ";
						else if (operands[0] == 'm' && operands[1] == 'C') fpW << "71 ";
						else if (operands[0] == 'm' && operands[1] == 'D') fpW << "72 ";
						else if (operands[0] == 'm' && operands[1] == 'E') fpW << "73 ";
						else if (operands[0] == 'm' && operands[1] == 'H') fpW << "74 ";
						else if (operands[0] == 'm' && operands[1] == 'L') fpW << "75 ";
						else if (operands[0] == 'm' && operands[1] == 'm') fpW << "76 ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "mvi") {  // ================================ MVI ================================ 
						char operand = params.begin()[1][0];
						if (operand == 'A')      fpW << "3E ";      // X, imm
						else if (operand == 'B') fpW << "06 ";
						else if (operand == 'C') fpW << "0E ";
						else if (operand == 'D') fpW << "16 ";
						else if (operand == 'E') fpW << "1E ";
						else if (operand == 'H') fpW << "26 ";
						else if (operand == 'L') fpW << "2E ";
						else if (operand == 'm') fpW << "36 ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}

						fpW << params.begin()[2].substr(2, 2) << " ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "lxi") {
						char operand = params.begin()[1][0];
						if (operand == 'B')      fpW << "01 ";      // X, double
						else if (operand == 'D') fpW << "11 ";
						else if (operand == 'H') fpW << "21 ";
						else if (operand == 'S') fpW << "31 ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}

						fpW << params.begin()[2].substr(4, 2) << " ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}

						fpW << params.begin()[2].substr(2, 2) << " ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "lda") {
						fpW << "3A ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}

						fpW << params.begin()[1].substr(4, 2) << " ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}

						fpW << params.begin()[1].substr(2, 2) << " ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "sta") {
						fpW << "32 ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}

						fpW << params.begin()[1].substr(4, 2) << " ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}

						fpW << params.begin()[1].substr(2, 2) << " ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "lhld") {
						// Todo
					}
					else if (instr == "shld") {
						// Todo
					}
					else if (instr == "ldax") {
						if (params.begin()[1][0] == 'B') {
							fpW << "0A ";
						}
						else if (params.begin()[1][0] == 'D') {
							fpW << "1A ";
						}
						else {
							std::cout << "\n\n --- Error --- __INVALID_INSTR_PARAMS__ \n\n";
						}

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "stax") {
						if (params.begin()[1][0] == 'B') {
							fpW << "02 ";
						}
						else if (params.begin()[1][0] == 'D') {
							fpW << "12 ";
						}
						else {
							std::cout << "\n\n --- Error --- __INVALID_INSTR_PARAMS__ \n\n";
						}

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "xchg") {
						// Todo
					}
					else if (instr == "add") {
						char operand = params.begin()[1][0];
						if (operand == 'A')      fpW << "87 ";      // X
						else if (operand == 'B') fpW << "80 ";
						else if (operand == 'C') fpW << "81 ";
						else if (operand == 'D') fpW << "82 ";
						else if (operand == 'E') fpW << "83 ";
						else if (operand == 'H') fpW << "84 ";
						else if (operand == 'L') fpW << "85 ";
						else if (operand == 'm') fpW << "86 ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "adi") {
						// Todo
					}
					else if (instr == "adc") {
						// Todo
					}
					else if (instr == "aci") {
						// Todo
					}
					else if (instr == "sub") {
						// Todo
					}
					else if (instr == "sui") {
						// Todo
					}
					else if (instr == "sbb") {
						// Todo
					}
					else if (instr == "sbi") {
						// Todo
					}
					else if (instr == "inr") {
						char operand = params.begin()[1][0];
						if (operand == 'A')      fpW << "3C ";      // X
						else if (operand == 'B') fpW << "04 ";
						else if (operand == 'C') fpW << "0C ";
						else if (operand == 'D') fpW << "14 ";
						else if (operand == 'E') fpW << "1C ";
						else if (operand == 'H') fpW << "24 ";
						else if (operand == 'L') fpW << "2C ";
						else if (operand == 'm') fpW << "34 ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "dcr") {
						char operand = params.begin()[1][0];
						if (operand == 'A')      fpW << "3D ";      // X
						else if (operand == 'B') fpW << "05 ";
						else if (operand == 'C') fpW << "0D ";
						else if (operand == 'D') fpW << "15 ";
						else if (operand == 'E') fpW << "1D ";
						else if (operand == 'H') fpW << "25 ";
						else if (operand == 'L') fpW << "2D ";
						else if (operand == 'm') fpW << "35 ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "dad") {
						// Todo
					}
					else if (instr == "inx") {
						char operand = params.begin()[1][0];
						if (operand == 'B')      fpW << "03 ";      // X
						else if (operand == 'D') fpW << "13 ";
						else if (operand == 'H') fpW << "23 ";
						else if (operand == 'S') fpW << "33 ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "dcx") {
						// Todo
					}
					else if (instr == "rlc") {
						// Todo
					}
					else if (instr == "rrc") {
						// Todo
					}
					else if (instr == "ral") {
						// Todo
					}
					else if (instr == "rar") {
						// Todo
					}
					else if (instr == "ani") {
						// Todo
					}
					else if (instr == "xri") {
						// Todo
					}
					else if (instr == "ori") {
						// Todo
					}
					else if (instr == "cpi") {
						// Todo
					}
					else if (instr == "jmp") {
						// Todo
					}
					else if (instr == "jnz") {
						fpW << "C2 ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}

						fpW << params.begin()[1].substr(4, 2) << " ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}

						fpW << params.begin()[1].substr(2, 2) << " ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "jz") {
						// Todo
					}
					else if (instr == "jnc") {
						fpW << "D2 ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}

						fpW << params.begin()[1].substr(4, 2) << " ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
 
						fpW << params.begin()[1].substr(2, 2) << " ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "jc") {
						// Todo
					}
					else if (instr == "jpo") {
						// Todo
					}
					else if (instr == "jpe") {
						// Todo
					}
					else if (instr == "jp") {
						// Todo
					}
					else if (instr == "jm") {
						// Todo
					}
					else if (instr == "call") {
						// Todo
					}
					else if (instr == "cnz") {
						// Todo
					}
					else if (instr == "cz") {
						// Todo
					}
					else if (instr == "cnc") {
						// Todo
					}
					else if (instr == "cc") {
						// Todo
					}
					else if (instr == "cpo") {
						// Todo
					}
					else if (instr == "cpe") {
						// Todo
					}
					else if (instr == "cp") {
						// Todo
					}
					else if (instr == "cm") {
						// Todo
					}
					else if (instr == "ret") {
						// Todo
					}
					else if (instr == "rnz") {
						// Todo
					}
					else if (instr == "rz") {
						// Todo
					}
					else if (instr == "rnc") {
						// Todo
					}
					else if (instr == "rc") {
						// Todo
					}
					else if (instr == "rpo") {
						// Todo
					}
					else if (instr == "rpe") {
						// Todo
					}
					else if (instr == "rp") {
						// Todo
					}
					else if (instr == "rm") {
						// Todo
					}
					else if (instr == "pchl") {
						// Todo
					}
					else if (instr == "nop") {
						fpW << "00 ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "hlt") {
						fpW << "76 ";

						bytesWritten++;
						if (bytesWritten % 8 == 0) {
							fpW << std::endl;
							bytesWritten = 0;
						}
					}
					else if (instr == "rst") {
					// Todo
					}
					else {
						std::cout << "\n------------- ERROR : __INVALID__INSTRUCTION__ ------------\n";
					}

				}

				// ===========================================   END INSTRUCTIONS    =============================================================
			}

		}

		// File close
		fpR.close();
		fpW.close();
	}

	/* delete tmp.txt file*/
	remove("tmp.txt");

	return;
}

/* 
	from formatted binary to a program layout ready to load in a memory sector 
*/
uint8_t* Assembler::getProgram() {

	// Blank Program
	uint8_t* program = new uint8_t[PROGRAM_DIM];
	for (int i = 0; i < PROGRAM_DIM; i++) {
		program[i] = 0x00;
	}
	
	// --- Open file ---
	std::ifstream fp("out.txt");

	if (fp.is_open()) {
		// --- Format file ---
		uint16_t base = 0x0000;

		/*
			For each <line>:
			  > label line or data line : calculate base address to put code for the section
			  > simple line : split line -> put each string in the program -> increment the base_address 
		*/
		std::string line;
		while (getline(fp, line)) {
			// --- label 0x0000 line ---
			std::size_t labelLine = line.find("label");
			std::size_t dataLine = line.find("data");
			if (labelLine != std::string::npos) {
				std::string base_str = splitString(line, " ").begin()[1].substr(2, 4);
				base = fromStringToHex16(base_str);
			} 
			else if (dataLine != std::string::npos) {
				std::string base_str = splitString(line, " ").begin()[1].substr(2, 4);
				base = fromStringToHex16(base_str);
			}
			else {  
				std::vector<std::string> bytes = splitString(line, " ");

				// Bytes Line / Data Line
				for (std::string str : bytes) {
					program[base] = fromStringToHex8(str);
					base++;
				}
			}
		}

		fp.close();
	}

	/* delete out.txt file and return */
	remove("out.txt");

	return program;
}

/*
	Auxiliary Functions 
*/

/* from a line to a vector of strings (slit based on a string delimiter) */
std::vector<std::string> Assembler::splitString(std::string str, std::string str_delimiter) {
	std::vector<std::string> params{};

	size_t pos = 0;
	while ((pos = str.find(str_delimiter)) != std::string::npos) {
		if (str.substr(0, pos).length() > 0) {
			params.push_back(str.substr(0, pos));
			str.erase(0, pos + str_delimiter.length());
		}
	}
	if (str.substr(0, str.length()).length() > 0) {
		params.push_back(str.substr(0, str.length()));
	}

	return params;
}

/* from string(containing a 2 characters hex number) to 8 bit number*/
uint8_t Assembler::fromStringToHex8(std::string str) {
	uint8_t x = 0x00;

	if (str.length() != 2) {
		std::cout << "ERROR - string Hex 8 " << std::endl;
		return 0x00;
	}

	std::string conversion = "0123456789ABCDEF";
	uint8_t mask = 4;
	for (int i = 0; i < 2; i++) {
		char c = str[i];
		uint8_t c_dec = conversion.find_first_of(c, 0);
		x = x + (c_dec << mask);
		mask -= 4;
	}

	return x;
}

/* from string(containing a 4 characters hex number) to 16 bit number*/
uint16_t Assembler::fromStringToHex16(std::string str) {
	uint16_t x = 0x0000;
	
	if (str.length() != 4) {
		std::cout << "ERROR - string Hex 16 " << std::endl;
		return 0x00;
	}

	std::string conversion = "0123456789ABCDEF";
	uint16_t mask = 12;
	for (int i = 0; i < 4; i++) {
		char c = str[i];
		uint16_t c_dec = (uint16_t) conversion.find_first_of(c, 0);
		x = x + (c_dec << mask);
		mask -= 4;
	}

	return x;
}

/* print 16 bit number to Screen in Hex (4 characters) */
void Assembler::printRegister16(uint16_t reg) {
	char hexString[50];
	sprintf_s(hexString, "%.4X\n", reg);
	std::cout << hexString;
}

/* from 16 bit number to 4 character String of the number in Hex*/
std::string Assembler::register16(uint16_t reg) {
	char hexString[10];
	sprintf_s(hexString, "%.4X", reg);
	return hexString;
}

/* Check if routine exists in vector */
bool Assembler::checkExists(std::vector<routine> routines, routine x) {
	for (routine r : routines) {
		if (r.name == x.name) {
			return true;
		}
	}
	return false;
}

/* set <Base_Address> of an existing routine <Name> */
int Assembler::setRoutineParams(std::vector<routine> &routines, std::string name, uint16_t baseAddr) {
	routine x; 
	x.name = name;
	x.baseAddr = baseAddr;

	if (!checkExists(routines, x)) return -1;

	for (int i = 0; i < routines.size(); i++) {
		if (routines.begin()[i].name == x.name) {
			routines.begin()[i].baseAddr = x.baseAddr;
		}
	}

	return 0;
}

/* get <Base_Address> of a routine <Name> */
uint16_t Assembler::getRoutineBaseAddress(std::vector<routine> routines, std::string name) {
	for (routine r : routines) {
		if (r.name == name) {
			return r.baseAddr;
		}
	}

	return 0x0000;
}

/* Capitalize String */
std::string Assembler::capitalizeString(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return toupper(c); });
	return s;
}


