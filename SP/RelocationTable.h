#pragma once
#ifndef _RELOCATIONTABLE_H_
#define _RELOCATIONTABLE_H_

#include <string>
#include <sstream>
#include <iostream>
#include <list>
#include <fstream>
#include "InstructionHolder.h"

using namespace std;

enum RelRowType {
	REL = 0, // relative relocation
	ABS // absolute relocation
};

struct RelRow {
	RelRowType rowType;
	unsigned int address;
	string sectionName;
	unsigned int symbolTableRowNumber;

	RelRow(RelRowType type, unsigned int addr, string sectName, unsigned int symTablRowNum) {
		rowType = type;
		address = addr;
		sectionName = sectName;
		symbolTableRowNumber = symTablRowNum;
	}

	string getRowTypeString();
};

class RelocationTable {

public:

	~RelocationTable();

	void addRelativeRow(unsigned int addr, string sectName, unsigned int symTablRowNum);
	void addAbsoulteRow(unsigned int addr, string sectName, unsigned int symTablRowNum);
	void printRows();
	void writeToFile(ofstream &fileStream);

	static void addHolder(InstructionHolder* holder);

private:
	list<RelRow> rows;
	static InstructionHolder* holder;
};

#endif