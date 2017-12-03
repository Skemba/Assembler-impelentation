#pragma once

#ifndef _INSTRUCTIONHOLDER_H_
#define _INSTRUCTIONHOLDER_H_

#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include "HelpFunctions.h"
using namespace std;

struct InstructionRow {
	string sectionName;
	string data;
	bool isData;

	InstructionRow(string sN, string d, bool iD) {
		data = d;
		sectionName = sN;
		isData = iD;
	}
};

class InstructionHolder {
public:
	void addRow(string sectName, string data, bool isData = false);
	void printRows();
	void writeToFile(ofstream &fileStream);
	void writeSegmentToFile(ofstream &fileStream, string segmentName);
	void writeInstruction(ofstream &fileStream, string data, bool isData);
	void addToWrittenRelocations(string segmentName);
	void writeAllLeft(ofstream &fileStream);

private:
	list<InstructionRow> rows;
	list<string> writtenRelocations;
	int printed;
};

#endif
