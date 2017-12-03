#pragma once

#include <string>

using namespace std;

// Definition directives
const string DEF = "DEF";
const string ORG = "ORG";

// Segment directives
const string BSS = ".bss";
const string DATA = ".data";
const string RODATA = ".rodata";
const string TEXT = ".text";

// Inport-export directive
const string GLOBAL = ".global";

// End directive
const string END = ".end";

// Data definition
const string LONG = "DD";
const string WORD = "DW";
const string WORDSIGNED = "DW_S";
const string BYTE = "DB";
const string BYTESIGNED = "DB_S";

// Data repetition
const string DUP = "DUP";

// Address modes
const string IMM = "imm";
const string REGDIR = "regdir";
const string MEMDIR = "memdir";
const string REGIND = "regind";
const string REGINDPOM = "regindpom";

// Registers
const string R0 = "R0";
const string R1 = "R1";
const string R2 = "R2";
const string R3 = "R3";
const string R4 = "R4";
const string R5 = "R5";
const string R6 = "R6";
const string R7 = "R7";
const string R8 = "R8";
const string R9 = "R9";
const string R10 = "R10";
const string R11 = "R11";
const string R12 = "R12";
const string R13 = "R13";
const string R14 = "R14";
const string R15 = "R15";
const string SP = "SP";
const string PC = "PC";
// Instructions

// none
const string RET = "RET";

// regdir
const string INT	= "INT";
const string PUSH = "PUSH";
const string POP = "POP";
const string ADD = "ADD";
const string SUB = "SUB";
const string MUL = "MUL";
const string DIV = "DIV";
const string MOD = "MOD";
const string AND = "AND";
const string OR = "OR";
const string XOR = "XOR";
const string NOT = "NOT";
const string ASL = "ASL";
const string ASR = "ASR";

// memdir, rgind, regind + pom (za uslovne za prvi operand i regdir)
const string JMP	= "JMP";
const string CALL	= "CALL";
const string JZ = "JZ";
const string JNZ = "JNZ";
const string JGZ = "JGZ";
const string JGEZ = "JGEZ";
const string JLZ = "JLZ";
const string JLEZ = "JLEZ";

// all 
const string LOAD	= "LOAD";
const string STORE	= "STORE";
const string LOADUB	= "LOADUB";
const string LOADSB	= "LOADSB";
const string LOADUW	= "LOADUW";
const string LOADSW = "LOADSW";
const string STOREB	= "STOREB";
const string STOREW = "STOREW";

