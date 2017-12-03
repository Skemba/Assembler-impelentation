#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include "LineProcessor.h"

using namespace std;

int main(int argc, char**argv)
{
	ifstream file;
	file.open(argv[1]);
	string outName = argv[2];
	LineProcessor *lp;

	string line = "";

	while (getline(file, line)) {
		if (line.empty()) {
			continue;
		}

		lp = new LineProcessor(line);
		if (!lp->firstPass() && LineProcessor::hasEnded())
			break;

		delete lp;
	}
	
	file.clear();
	file.seekg(0, ios::beg);
	lp->initSecondPass();

	while (getline(file, line)) {
		if (line.empty()) {
			continue;
		}

		lp = new LineProcessor(line);
		if (!lp->secondPass() && LineProcessor::hasEnded())
			break;

		delete lp;
	}

	if (!LineProcessor::hasErrors())
	{
		ofstream outFile;
		outFile.open(outName);

		SymbolTable* symTable = LineProcessor::getSymTable();
		symTable->writeToFile(outFile);

		RelocationTable* relTable = LineProcessor::getRelTable();
		InstructionHolder* holder = LineProcessor::getInstrHolder();
		relTable->addHolder(holder);
		relTable->writeToFile(outFile);

		outFile << "#end";

		outFile.close();
	}

	delete lp;
}