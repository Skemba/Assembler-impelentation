#pragma once
#ifndef _LINEPROCESSOR_H_
#define _LINEPROCESSOR_H_

#include <iostream>
#include <list>
#include <algorithm>
#include <map>
#include "Constants.h"
#include "SymbolTable.h"
#include "RelocationTable.h"
#include "InstructionHolder.h"

using namespace std;

enum CommandType {
	NONE = 0,
	DIR,
	INSTR,
	LABEL,
	SECT
};

enum Section {
	S_BSS = 0,
	S_DATA,
	S_RODATA,
	S_TEXT,
	S_NONE
};

class LineProcessor
{
public:

	LineProcessor(string line);

	~LineProcessor();

	CommandType extractType();

	bool firstPass();
	void initSecondPass();
	bool secondPass();

	bool lookForInstruction(string word, bool secondPass);
	int evalInstrExpr(string expression, bool isRelative, bool isData = false);

	static SymbolTable* getSymTable();
	static RelocationTable* getRelTable();
	static InstructionHolder* getInstrHolder();
	static void callAfterSegmentAddMethods(string word);
	static void callBeforeSegmentAddMethods();
	static void changeSegmentStartAddr();
	static bool checkForLeftLabels();
	static map<string, unsigned int> createOpMap();
	static map<string, unsigned int> createAddressCodeMap();
	static map<string, unsigned int> createRegisterCodeMap();
	static map<string, unsigned int> createDataTypeCodeMap();
	static bool hasErrors();
	static bool hasEnded();

private:
	string line;
	int wordInLine;
	stringstream currentLineStream;
	string latestSymbol;
	bool currentDirORG;
	
	static SymbolTable* symTable;
	static RelocationTable* relTable;
	static InstructionHolder* holder;
	static bool hasToBeSection;
	static bool lastLineORG;
	static bool errorOccured;
	static bool endOccured;
	static list<string> labelsLeft;
	static Section currentSection;
	static string currentSectionName;
	static Section previousSection;
	static unsigned int locationCounter;
	static unsigned int ORGLocation;
	static list<string> InstructionBits;
	static const list<string> arithmeticInstr;
	static const list<string> instr4B;
	static const list<string> instr8BOneOp;
	static const list<string> instr8BTwoOp;
	static const map<string, unsigned int> opMap;
	static const map<string, unsigned int> addressCodeMap;
	static const map<string, unsigned int> registerCodeMap;
	static const map<string, unsigned int> dataTypeCodeMap;
	static list<string> orgDefined;



};

#endif