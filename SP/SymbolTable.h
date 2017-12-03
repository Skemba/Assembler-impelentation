#pragma once
#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_

#include <string>
#include <iostream>
#include <list>
#include <fstream>
#include "HelpFunctions.h"

using namespace std;

enum RowType {
	SEG = 0, // used for section symbols
	SYM // used for all other symbols
};

struct Row {
	RowType rowType;
	unsigned int rowNumber;
	string rowName;
	int sectionRowNumber;
	unsigned int address; // Value for SYM
	unsigned int sectionSize; // Only for SEG
	string flags;

	Row(RowType type, unsigned int number, string name, int sectionNumber, unsigned int addr, unsigned int size, string flg) {
		rowType = type;
		rowNumber = number;
		rowName = name;
		sectionRowNumber = sectionNumber;
		address = addr;
		sectionSize = size;
		flags = flg;
	}

	Row(RowType type, unsigned int number, string name, int sectionNumber, unsigned int value, string flg) {
		rowType = type;
		rowNumber = number;
		rowName = name;
		sectionRowNumber = sectionNumber;
		address = value;
		sectionSize = 0;
		flags = flg;
	}

	string getRowTypeString();
};

class SymbolTable {

public:

	~SymbolTable();

	void addSegmentRow(string name, unsigned int size);
	void addSegmentRowWithAddr(string name, unsigned int size, unsigned int addr);
	void addSymbolRow(string name, int sectionNumber, unsigned int value, string flg);
	void printRows();
	void writeToFile(ofstream &fileStream);
	int getSegRowId(string segName);
	int getSymbolValue(string symbolName);
	int getSymbolValueForSection(string symbolName, int sectionNumber);
	int getSegmentFromSymbol(string symbolName);
	unsigned int getSegmentByName(string segName);
	unsigned int getSymbolRowNumber(string symbolName);
	void updateSegmentRowSize(unsigned int newSize, string segName);
	void updateSymbolValue(unsigned int value, string symbolName);
	void addToAllSegmentValues(string segName, int toAdd);
	bool checkAndUpdateFlag(string name, string flag);
	unsigned int getRowAddr(string name);
	unsigned int getRelocationAddrInfo(string name);
	int getRelocationInstrInfo(string name, bool isRelative);

private:
	list<Row> rows;
	static unsigned int id;
};

#endif