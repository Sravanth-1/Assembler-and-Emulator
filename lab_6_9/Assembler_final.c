/*****************************************************************************
TITLE: Claims
AUTHOR:         SAI SRAVANTH DATTI
ROLL NUMBER:    2101AI29
Declaration of Authorship
This file is part of the assignment of CS210 at the
Department of Computer Science and Engg, IIT Patna .
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
// Defining Symbol Table

// Creation of a Structure for each node in symbol table
 typedef struct SymTableNode {
	char* symbol;    // to store symbols
	int address;    // to store address
	struct SymTableNode* next;
}SymTableNode;

// Creation of a Structure for operations with symbol table
typedef struct SymTable {
	SymTableNode* head;
    //to insert a new symbol into symbol table
	int (*ist)(struct SymTable*, char*);
	//to assign the symbol in symbol table with a address
	int (*assg)(struct SymTable*, char*, int);
	//to get the address of a symbol in symbol table
	int (*getaddr)(const struct SymTable*, char*);
	//check if any unassigned symbol presents or not without an address
	bool (*anything_unassigned)(const struct SymTable*);
	// for the case of set we update the address of a symbol to a new value
	bool (*update)(struct SymTable*, char*, int);
}SymTable;

// Function to create a new node of symbol table for further progress
SymTableNode* makeSymTableNode(char* label, int address) {
    SymTableNode* childNode = (SymTableNode*) malloc(sizeof(SymTableNode));

	childNode -> symbol = (char*) malloc(1024);
	childNode -> address = address;
	childNode -> next = NULL;
	strcpy(childNode -> symbol, label);
    return childNode; //this step returns the newly created childNode of the parentNode
}

// Function for Introducing a label that occurs as an operand
// if the label does not already exist in the table.
int istSymTable(SymTable* symbol_tbl,char* label) {
    if (symbol_tbl -> head == NULL) {
		symbol_tbl -> head = makeSymTableNode(label, -1);;
		return 1;  // returns 1 if we head newly
	}
    SymTableNode* head = symbol_tbl -> head;
   if (strcmp(label, head -> symbol) == 0) {
        return 0; // returns zero is label and the first node are same
   }
       while (head -> next != NULL) {
		if (strcmp(label, (head -> next) -> symbol) == 0) {
                return 0;
		}
		head = head -> next;
	}

	head -> next = makeSymTableNode(label, -1);
	return 1;
}
//assignment function which assigns the PC address to a label if it is already present in the symbol taable
int assgSymTable(SymTable* symbol_tbl,char* label, int PC) {
    (*symbol_tbl).ist(symbol_tbl, label);
	   SymTableNode* head = symbol_tbl -> head;
     while (strcmp(head -> symbol, label) != 0) {
		head = head -> next;
	}
    if (head -> address != -1) {     //if the nodes Address is already updated that means duplicate label is found
            //so returns 1 so that we can print the error in log file
            return 1;
    }
    head -> address = PC;
	return 0;  // returns zero normally if it assigns the PC value to symbol
}
//Function to return the address of a symbol in the symbol table
// Return -1 if the address does not exist.
int getaddr(const SymTable* symbol_tbl, char* label) {
    SymTableNode* head = symbol_tbl -> head;
    while (head != NULL && strcmp(head -> symbol, label) != 0) {
		head = head -> next;
	}
    if (head == NULL){     // returns -1 if there is nothing updated as the address of the symbol
        return -1;
	}
	return head -> address;
}
// This function checks for the symbols with unassigned address if any
bool anything_unassigned(const SymTable* symbol_tbl) {
    SymTableNode* head = symbol_tbl -> head;
	while (head != NULL) {
		if (head -> address == -1){ // checking for -1 in address
                return 1;            // returns 1 if there is any symbol with unassigned address
		}
		head = head -> next;
	}
	return 0;     // returns 0 if no unassigned symbol presents
}
//function for the case of set-since,set updates the values of variables or address for labels
bool update(SymTable* symbol_tbl, char* label, int address) {
    SymTableNode* head = symbol_tbl -> head;

	while (head != NULL && strcmp(head -> symbol, label) != 0) {
		head = head -> next;
	}
    if (head == NULL){
            return 0;
    }
	head -> address = address;
	return 1;
}
//make symbol tale
SymTable symbol_tbl = {NULL, istSymTable, assgSymTable, getaddr, anything_unassigned, update};

//Required Definitions for Mnemonics
//Defining a structure for Mnemonic
typedef struct Mnemonic {
	char* mnemonic; //This is the name of the Mnemonic
	char* opcode;   //This is the opcode of the Mnemonic
	int operands_requirement; //This is the code to distinguish which type of Mnemonic is required
} Mnemonic;
//This array stores the struct details of all Mnemonics
Mnemonic mnemonics[] = {{"ldc", "00000000", 1},{"adc", "00000001", 1},{"ldl", "00000010", 4},{"stl", "00000011", 4},{"ldnl","00000100", 4},
	                    {"stnl", "00000101", 4},{"add", "00000110", 0},{"sub", "00000111", 0},{"shl", "00001000", 0},{"shr", "00001001", 0},
	                    {"adj", "00001010", 1},{"a2sp","00001011", 0},{"sp2a","00001100", 0},{"call","00001101", 4},{"return","00001110", 0},
                        {"brz", "00001111", 4},{"brlz","00010000", 4},{"br", "00010001", 4},{"HALT","00010010", 0},{"data", NULL, 2},
                        {"SET", NULL, 3}};
//if the operands_requirement = 0 : No Operand, operands_requirement = 1 : Value Operand,operands_requirement = 2 : 'data',
//if the operands_requirement = 3 : 'set',      operands_requirement = 4 : Offset Operand

//function to scan the word and return -1 if it is empty
int Scanning(char** line, char* next) {
	int reline;
	int look_error = sscanf((*line), "%s%n", next, &reline);
	if (look_error != -1) (*line) += reline;
	return look_error;
}
//function to check the word is a valid label or not
// Conditions: A label should be alphanumeric and cannot start with numerical values
bool check_valid_label(char* label) {
	int length = strlen(label);
	int p = length;
	if (p == 0 || (!isalpha(label[0]) && label[0] != '_')){
            return 0; // returns zero if it is not a valid label
	}
	for (int i = 0; i < p; i++) {
		if (!isalnum(label[i]) && label[i] != '_') { //if the label does not start with any alphanumerical value or _
                return 0;                            //then return 0
		}
	}
	return 1;//returns 1 if this a valid label
}
//Function to check the word is a valid number or not
bool check_valid_number(char* next) {
	char* endptr = NULL;
	errno = 0;
	int num = strtol(next, &endptr, 0);
	if (num == 0 && (next == endptr || errno != 0|| (errno == 0 && next && *endptr != 0))) {
		return 0;   // returns 0 if it is not a valid number
	}
	return 1; // returns 1 if this is a valid number
}

// Function to return number conversion of given valid string
int string_to_number(char* next) {
	char* endptr = NULL;
	return strtol(next, &endptr, 0);
}
// Function to Return the 8-bit opcode of the given mnemonic
int  find_opcode(char** opcode, char* mnemonic) {
	for (int i = 0; i < 20; i++) {
		if (strcmp(mnemonics[i].mnemonic, mnemonic) == 0) { // checks the mnemonic array for the required mnemonic
			*opcode = mnemonics[i].opcode; // assigns the required opcode here
			return 1;            // returns 1 upon successful execution
		}
	}
	return 0; // returns 0 if the function cant find the opcode for this mnemonic
}
// This function returns 1 if the mnemonic requires offset
bool reqOffset(char* mnemonic) {
	char* offsetMnemonics[] = {"ldl", "stl", "ldnl", "stnl", "call", "brz", "brlz", "br"};//these mnemonics requires offset
	for (int i = 0; i < 8; i++) {
		if (strcmp(offsetMnemonics[i], mnemonic) == 0) { // here it returns 1 if the mnemonic matches the mnemonics which requires offset
                return 1;
		}
	}
	return 0; // else return 0
}

//function to convert a number into 24 bit code
char* convert_into_24bits(int x) {
	char str[] = "000000000000000000000000";
	for (int i = 23; i >= 0; i--) {
		int k = x >> i;
		if (k & 1)
			str[23 - i] = '1';
		else
			str[23 - i] = '0';
	}
    char* total = (char*) malloc(40);
    strcpy(total, str);
    return total;
}
//function to convert a number into 32 bit code
char* convert_into_32bits(int x) {
	char str[] = "00000000000000000000000000000000";
	for (int i = 31; i >= 0; i--) {
		int k = x >> i;
		if (k & 1)
			str[31 - i] = '1';
		else
			str[31 - i] = '0';
	}
    char* total = (char*) malloc(40);
    strcpy(total, str);
    return total;
}

// Function to Set the 24-bit (32 bit in case of Data)operands code for the machine code.
int set_Operands_Code(char** operands_code, char* next, char* mnemonic,int type_of_mnemonic, int Program_Counter) {
	int num = 0;
	// For 'data'
	if (type_of_mnemonic == 3) { // since, type of mnemonic is equals 3 for data
		*operands_code = convert_into_32bits(string_to_number(next)); // for data we a 32 bit code because of the absence of the 8 bit opcode
		return 1;
	}
    //if the word is a valid number then convert it into a number and store it in num
	if (check_valid_number(next)){
        num = string_to_number(next);
	}
	else if (check_valid_label(next)) {//if the word is a valid label,then get the address assigned to this label from symbol table
            num = symbol_tbl.getaddr(&symbol_tbl, next);
	}
	else {
         return 0;
	}
    //if it is the offset required mnemonic case then subtract the present PC address from the address assigned to the label in symbol table
	if (reqOffset(mnemonic) && check_valid_label(next)) {
            num -= Program_Counter+ 1;
	}
	*operands_code = convert_into_24bits(num);//convert it into 24 bit opr code

	return 1;
}
// function to convert Binary to Hexadecimal
char* Hexa_decimal_from_binary(char* bin, int mode) {
	char* a = bin;
	int num = 0;
	do {
	    int b = *a == '1'? 1 : 0;
	    num = (num << 1) | b;
	    a++;
	} while (*a);
	char* hex = (char*) malloc(10);
	if (mode) sprintf(hex, "%08x", num);
	else sprintf(hex, "%x", num);
	return hex;
}
int validmnemonic(char* mnemonic){//function to check the valid mnemonic
    int i;
    for(int i=0;i<21;i++){
        if(strcmp(mnemonics[i].mnemonic,mnemonic)==0){
            return mnemonics[i].operands_requirement+1;
        }//comparing the given word with the mnemonics given if it is present return 1 else return 0
    }
    return 0;
}


// Function for second pass
// Used to convert given assmebly to machine code and along with that identify more errors
int secondPass(FILE *assembly, FILE *object, FILE *list, FILE* log) {
    //Declaration of required variables
	int Program_counter = 0; // variable to know the present PC address
    int error = 0;           // Variable to store if any error is found or not
    int line_number = 0;     // variable to store the present line number we are processing
    char* line = (char*) malloc(1024);
    //To move file pointer associated with a given file to heading
	fseek(assembly, 0, SEEK_SET);
	//similar to the first pass we take the input complete line by line and process them
	while (fgets(line, 1024, assembly)) {
		line_number += 1;// Increment of line number by 1 to go to  next line in next iteration
    // let us clear the comments if they are present
    char* find_comment = strchr(line, ';'); // this search for the ;,because comment is initiated with ;
	if (find_comment != NULL) *find_comment = '\0'; // removing comment
	//Declaration of required variables for the second pass with dynamic memory allocation
	char* label = (char*) malloc(1024); // to store labels
    char* mnemonic = (char*) malloc(10);// to store the mnemonics
    char* next = (char*) malloc(1024);  // to store if any operand is present after mnemonics
    char* opcode = (char*) malloc(10);  // to store the opcode of mnemonic
    char* operands_code = (char*) malloc(26); // to store the operands code
    char* machine_code = (char*) malloc(36); // this variable is to store the final machine code output for each line
    machine_code[0] = '\0'; // Initiation

    //checks for the presence of a label by checking for presence of colon
    char* for_colon = strchr(line, ':');
	if (for_colon != NULL) { // if label presents then check for conditions exits move to next step
		*for_colon = ' ';    // replace colon if found
    // Here we wont check for the valid conditions for the label because the program will come till here
    // if and only if the label is a valid one.
    (Scanning(&line, label));// to change the address to next word
     // print the label and the PC address side by side in the list file
     fprintf(list, "%08x\t\t\t\t%s:\n", Program_counter, label);
}
    if (Scanning(&line, mnemonic) == -1){ // if there is no mnemonic then continue to next line
        continue;
    }
     //Declaration of a variable to store type of a Mnemonic
    int type_of_mnemonic = validmnemonic(mnemonic);
    find_opcode(&opcode, mnemonic);// to store the opcode of the encountered mnemonic
    // The type of mnemonic is 4 for set,since we wont need to print the machine code in object file we continue
    if (type_of_mnemonic == 4)continue;
       //if the type of mnemonic is greater than 1 that means that the mnemonic requires an operand
		if (type_of_mnemonic > 1) { //if the mnemonic requires operand
		    Scanning(&line,next);   //the use of this scanning is make the pointer line to point the next word.i.e operand
		    //function call to function to set the opr code for the given operands
		    set_Operands_Code(&operands_code, next, mnemonic, type_of_mnemonic, Program_counter);
         //if the mnemonic does not require operands then set the operand code with 0 in place of word
		}else{set_Operands_Code(&operands_code,"0", mnemonic, type_of_mnemonic, Program_counter);
		}
		//using an inbuilt function to catenate the 24 bit(32 bit in case of data) into the machinecode
		strcat(machine_code, operands_code);
		//if the mnemonic is not data then catenate the 8 bit opcode to make the machine code 32 bit
		if (type_of_mnemonic != 3) {
                strcat(machine_code, opcode); // catenation of opcode with machine code
		}
		// print the machine code in the object file
		fprintf(object, "%s\n", machine_code);
		// to print the required data in the list file
		fprintf(list, "%08x\t%s\t%s\t0x%s\n", Program_counter, Hexa_decimal_from_binary(machine_code, 1), mnemonic, Hexa_decimal_from_binary(operands_code, 0));
		Program_counter += 1; // increment of the PC address by 1 to go to next line in the next iteration
	}
	// to print no errors after the completion of pass two
	fprintf(log, "No Errors found in the second pass\n");
	return 1;// returns 1 upon successful execution
}
// This is the main Driver program
int main(int argc,char* argv[]){ // argc is the number of arguments and argv is the arguments in string format
    //if the number of arguments is not equal to two then print conditions
    if(argc != 2)
	{
		printf( "Invalid Command, Check Format : ./asm filename.asm");
	}
FILE *assembly, *object, *list, *log;
//dynamic allocation to store the names of the files
char* file_name = (char*) malloc(1024);
char* object_file_name = (char*) malloc(1024);
char* list_file_name = (char*) malloc(1024);
char* log_file_name = (char*) malloc(1024);

strcpy(file_name, argv[1]);//extraction of name from argv
char* something = strchr(file_name, '.');
if (something != NULL) *something = '\0';
assembly = fopen(argv[1], "r");//opening assembly file in read mode
//assigning name to object file
object_file_name[0] = '\0';
strcat(object_file_name, file_name);
strcat(object_file_name, ".o");
object = fopen(object_file_name, "wb");//opening object file in write binary mode

//assigning name to listing file
list_file_name[0] = '\0';
strcat(list_file_name, file_name);
strcat(list_file_name, ".lst");
list = fopen(list_file_name, "w");//opening list file in write mode

//assigning name to log file
log_file_name[0] = '\0';
strcat(log_file_name, file_name);
strcat(log_file_name, ".log");
log = fopen(log_file_name, "w");//opening log file in write mode

// First pass

int Program_counter = 0;// A variable to store the current PC address
int error = 0;          // A variable to know if errors are present or not
int line_number = 0;    // A variable to store the current line number
char* line = (char*) malloc(1024);
//to read the assembly file line to line
while (fgets(line, 1024, assembly)) {
		line_number += 1; //incrementation of line number
    // To clear comments if they are present
    char* find_comment = strchr(line, ';'); // looks for a ; since comment heads with ;
	if (find_comment != NULL) {
            *find_comment = '\0';
	}
        //Dynamic memory allocation to label and next word
		char* next = (char*) malloc(1024);
		char* label = (char*) malloc(1024);
	//checks for the presence of a label by checking for presence of colon
    char* for_colon = strchr(line, ':');
	if (for_colon != NULL) { // if label presents then check for conditions exits move to next step
		*for_colon = ' ';    // replace colon if found
		if (Scanning(&line, label) == -1) {
        //if it is empty print the corresponding error in log file and continue
        fprintf(log, "ERROR: Line %d: This is not a valid Label\n", line_number);
        error++;
				continue;
			}
			// check that if the label is valid label or not and if it is valid go further
			if (check_valid_label(label)) {
                //assign the PC address to the label in symbol table,
                // if it is already assigned then print error and exit the loop
				if (symbol_tbl.assg(&symbol_tbl, label, Program_counter)) { // returns 1 if it is already assigned
                // To print the required error in the log file
				fprintf(log, "ERROR: Line %d: This label is already encountered.This is a Duplicate Label\n", line_number);
                    error++;
					continue;
				}
			}
			//if the label is not a valid label then print the error message in the log file
			else {
				fprintf(log, "ERROR: Line %d: This is not a valid Label\n", line_number);
                error++;
				continue;
			}
	}
    //scan for the next word,if nothing present then just continue
    if (Scanning(&line, next) == -1) {
				continue;
			}
    //Declaration of a variable to store type of a Mnemonic
    int type_of_mnemonic = validmnemonic(next);

    if (!type_of_mnemonic) { //if type of mnemonic is zero then the word is not a valid mnemonic
            //print error and continue
			fprintf(log, "ERROR: Line %d: This is not a valid Mnemonic\n", line_number);
			error++;
			continue;
		}

 //if type of mnemonic is greater than 1 then,the mnemonic requires operand
    if (type_of_mnemonic>1) {
            //scan for the next word
			if (Scanning(&line, next) == -1) {
			    //if the next word is empty then print the error message and continue
				fprintf(log, "ERROR: Line %d: Operand is missing here\n", line_number);
			    error++;
				continue;
			}
                //check the next word for if it is a number or not
			if (!check_valid_label(next) && !check_valid_number(next)) {
                //if the next word is not a number then print the error message in the log file
			    fprintf(log, "ERROR: Line %d: The Operand is not a number\n", line_number);
			    error++;
				continue;
			}
                //check the next word is a valid label or not
			if (check_valid_label(next)){
                // insert the new word into the symbol table if it is a valid label
				symbol_tbl.ist(&symbol_tbl, next);
			}
                //check for if the next word is a valid number or not
			else if (check_valid_number(next)) { //if it is a valid number then check for set
				// If 'set' update value for the given label
				if (type_of_mnemonic == 4) {
					symbol_tbl.update(&symbol_tbl, label, string_to_number(next));
					continue;
				}
			}
		}// If something is present after mnemonic for which operand not required then print error
		else if (Scanning(&line, next) != -1) {
            //if the next word is not empty then print the error message in log file
			fprintf(log, "ERROR: Line %d: An Unexpected Operand encountered\n", line_number);
            error++;
			continue;
		}
    //Check for any extra words are present after operands
		if (Scanning(&line, next) != -1) {
			fprintf(log, "ERROR: Line %d: Encountered Extra words at the end of the line \n", line_number);
            error++;
			continue;
		}
    //completion of working with a line in the input
    //Increase the PC by 1
		Program_counter += 1;

  }
  //Completion of while loop
  if(symbol_tbl.anything_unassigned(&symbol_tbl)) { // Scanning the symbol table for the presence of any unassigned symbols
        //print the error message in the log file
		fprintf(log, "ERROR: Line %d: No such label is present\n", line_number);
        error++;
	}
  //If there are any errors then no need to go to pass two
  if(error){
    return 0; // simply return 0 if there are any errors
  } else{
    fprintf(log, "No Errors found in the first pass\n");
    // if no errors are present then proceed to pass two
    //Function call to pass two with all the files open
    secondPass(assembly, object, list, log);
  }
  // Close the Assembly File
	fclose(log);
	fclose(list);
	fclose(object);
	fclose(assembly);

}

