/*****************************************************************************
TITLE: Claims
AUTHOR:         SAI SRAVANTH DATTI
ROLL NUMBER:    2101AI29
Declaration of Authorship
This txt file, claims.txt, is part of the assignment of CS210 at the
Department of Computer Science and Engg, IIT Patna .
*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Initiation of The stack pointer, program counter, registers and memory
int SP, PC, S1, S2;
int mem[16777216];
// DEFINING ASSEMBLY FUNCTIONS AND MNEMONICS
char* mnemonic[] = {"ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT"};

void ldc(int val) {
	S2 = S1;
	S1 = val;
}

void adc(int val) {
	S1 += val;
}

void ldl(int offset) {
	S2 = S1;
	S1 = mem[SP + offset];
}

void stl(int offset) {
	mem[SP + offset] = S1;
	S1 = S2;
}

void ldnl(int offset) {
	S1 = mem[S1 + offset];
}

void stnl(int offset) {
	mem[S1 + offset] = S2;
}

void add() {
	S1 += S2;
}

void sub() {
	S1 = S2 - S1;
}

void shl() {
	S1 = S2 << S1;
}

void shr() {
	S1 = S2 >> S1;
}

void adj(int val) {
	SP += val;
}

void a2sp() {
	SP = S1;
	S1 = S2;
}

void sp2a() {
	S2 = S1;
	S1 = SP;
}

void call(int offset) {
	S2 = S1;
	S1 = PC;
	PC += offset;
}

void RETURN() {
	PC = S1;
	S1 = S2;
}

void brz(int offset) {
	if (S1 == 0) PC += offset;
}

void brlz(int offset) {
	if (S1 < 0) PC += offset;
}

void br(int offset) {
	PC += offset;
}

//UTILITY FUNCTIONS
// Defining Return instruction type. 1 = value, 2 = offset, 0 = not required
char* ExtractType(int mnem) {
	if (mnem < 4 || mnem == 10) return "value";
	else if (mnem == 4 || mnem == 5) return "value";
	else if (mnem == 15 || mnem == 17) return "offset";
	else if (mnem == 13 || mnem == 16) return "offset";
	return "";
}

// Function To Print ISA
void isa() {
	fprintf(stderr, "Opcode\tMnemonic\toperand\n");
	for (int i = 0; i <= 18; i++) {
		fprintf(stderr, "%d\t%s\t%s\n", i, mnemonic[i], ExtractType(i));
	}
	fprintf(stderr, "\tSET\tvalue\n");
	return;
}

//Function to give Memory Dump until the number on instructions/data provided
void print_to_Mem(int line_number, bool Before) {
	if (Before){
        fprintf(stderr, "\nMemory dump before execution\n");
	}
	else {
    fprintf(stderr, "\nMemory dump after execution\n");
	}
	for (int i = 0; i < line_number; i++) {
		if (i % 4 == 0) fprintf(stderr, "\n%08x ", i);
		fprintf(stderr, "%08x ", mem[i]);
	}
	fprintf(stderr, "\n");
	return;
}

//Function to give Memory Dump in the mem dump file until the number on instructions/data provided
void send_mem_dump_to_file(int line_number, FILE* file) {
	for (int i = 0; i < line_number; i++) {
		if (i % 4 == 0) fprintf(file, "\n%08x ", i);
		fprintf(file, "%08x ", mem[i]);
	}
	fprintf(file, "\n");
	return;
}

// Simple function which converts a 32 bit binary number to decimal number
int binary_to_number(char* bin) {
	int num = 0;
	for (int i = 0; i < 32; i++) {
		num *= 2;
		if (bin[i] == '1') {
            num += 1;
		}
	}
	return num;
}
// To Extract mnemonic code from instruction
int ExtractMnemonicCode(int Inst) {
	return ((1 << 8) - 1) & Inst;
}

// To Extract mnemonic code from instruction
int ExtractOpr(int Inst) {
	return (Inst >> 8);
}

// To Prints trace details
void printTrace(int opcode, int opr) {
	fprintf(stderr, "PC=%08x, SP=%08x, A=%08x, B=%08x %s\t%08x\n", PC, SP, S1, S2, mnemonic[opcode], opr);
}

// Print executable options after input if the iput is right
void printOptions() {
	fprintf(stderr, "usage: emu [options] file.o\n");
	fprintf(stderr, "-trace\tshow instruction trace\n");
	fprintf(stderr, "-before\tshow memory dump before execution\n");
	fprintf(stderr, "-after\tshow memory dump after execution\n");
	fprintf(stderr, "-isa\tdisplay ISA\n");
}

// This function sets the opcode for various functions
int executeInst(int opcode, int opr) {
	if (opcode == 0) ldc(opr);
	if (opcode == 1) adc(opr);
	if (opcode == 2) ldl(opr);
	if (opcode == 3) stl(opr);
	if (opcode == 4) ldnl(opr);
	if (opcode == 5) stnl(opr);
	if (opcode == 6) add();
	if (opcode == 7) sub();
	if (opcode == 8) shl();
	if (opcode == 9) shr();
	if (opcode == 10) adj(opr);
	if (opcode == 11) a2sp();
	if (opcode == 12) sp2a();
	if (opcode == 13) call(opr);
	if (opcode == 14) RETURN();
	if (opcode == 15) brz(opr);
	if (opcode == 16) brlz(opr);
	if (opcode == 17) br(opr);
	else return 0;
	return 1;
}

// Executes the machine code and, if necessary, prints the trace.
// if Invalid Opcode return 1
int execute(bool trace) {
	SP = (1 << 23) -1;
	int opr = 0, opcode = 0;
	int Inst_num = 0;

	while ((opcode = ExtractMnemonicCode(mem[PC])) <= 18) {

		opr = ExtractOpr(mem[PC]);

		if (trace) printTrace(opcode, opr);
		if (opcode == 18) {
            break;
		}
		// Since the opcode greater than 18 are impossible
		if (opcode > 18) {
        return 1;
		}
        //increment of PC address by  to check next line
		PC += 1;
		executeInst(opcode, opr);
		Inst_num += 1;
	}
	fprintf(stderr, "\n%d instructions executed.\n", Inst_num);
	return 0;
}
// This is the main Driver program
int main(int argc, char* argv[]) { // argc is the number of arguments and argv is the arguments in string format
	// To Open assembly file
	FILE* object;
	FILE* dump;
    // If no arguments
	if (argc < 3) {
		printOptions();
		return 1;
	}
	//dynamic allocation to store the names of the files
    char* file_name = (char*) malloc(1024);
	char* dump_name = (char*) malloc(1024);

	//extraction of name from argv
	strcpy(file_name, argv[2]); // function to copy the file name from argv[2] to variable file name
	char* ptr = strchr(file_name, '.');
	if (ptr != NULL) *ptr = '\0';
    object = fopen(argv[2], "rb");//the object file is opened in read binary mode

	//Assigning file name to dump file
	strcat(dump_name, file_name); // function to catenate filename to dumpname
	strcat(dump_name, "_memdump.txt");
    dump = fopen(dump_name, "w");// the dump file is opened in write mode
    //Defining variables for trace,before,after,ISA individually
	int trace;
	int before;
	int after;
	int ISA;
    //functions to check argv[1] for trace,before,after,isa
	if(strcmp(argv[1], "-trace") == 0){
    trace = 1; //stores 1 in variable trace if argv[1] is -trace else 0
    } else {
             0;}
    if(strcmp(argv[1], "-before") == 0){
    trace = 1;//stores 1 in variable before if argv[1] is -before else 0
    } else {
             0;}
    if(strcmp(argv[1], "-after") == 0){
    trace = 1;//stores 1 in variable before if argv[1] is -after else 0
    } else {
             0;}
    if(strcmp(argv[1], "-isa") == 0){
    trace = 1;//stores 1 in variable before if argv[1] is -isa else 0
    } else {
             0;}

	// checking for the case of invalid
	if ((trace == 0 && before == 0 && after == 0 && ISA == 0)) { // if argv[1] is other than trace,before,after,isa return 1
		printOptions();
		return 1;
	}
	// read the machine code in the object file into memory
    char* Instructions = (char*) malloc(36);
	int line_number = 0;   // variable to store the count of instructions
	while (fgets(Instructions, 36, object)) {
		mem[line_number] = binary_to_number(Instructions);

		line_number += 1;
	}
    int Instructions_count = line_number;  // store the number of instructions in instructions count variable

	// If argv[1] is -before then print
	if (before == 1) {
            //if argv[1] is -before then print in memory dump for all instructions
            print_to_Mem(Instructions_count, 1);
	}
	if (ISA== 1){ //if argv[1] -is isa then print ISA
        isa();
	}
    //function call to execute the machine code and if necessary prints the trace
	execute(trace);

	if (after == 1) {
           //if argv[1] is after then print in memory dump for all instructions
        print_to_Mem(Instructions_count, 0);
	}
	// function call to print the mem dump in the dump file
	send_mem_dump_to_file(Instructions_count, dump);

	// To Close the assembly file
	fclose(dump);
	fclose(object);
}
