#include "RelocationTable.h"

InstructionHolder* RelocationTable::holder = NULL;

RelocationTable::~RelocationTable()
{
	// delete row list
}

void RelocationTable::addHolder(InstructionHolder * instrHolder)
{
	holder = instrHolder;
}

// only in case of $
void RelocationTable::addRelativeRow(unsigned int addr, string sectName, unsigned int symTablRowNum)
{
	RelRow* newRow = new RelRow(REL, addr, sectName, symTablRowNum);
	rows.push_back(*newRow);
	delete newRow;
}

void RelocationTable::addAbsoulteRow(unsigned int addr, string sectName, unsigned int symTablRowNum)
{
	RelRow* newRow = new RelRow(ABS, addr, sectName, symTablRowNum);
	rows.push_back(*newRow);
	delete newRow;
}

void RelocationTable::printRows()
{
	cout << '\n';
	cout << "The contents of rows are: \n";
	cout << "Tip" << "\t" << "Offset" << "\t" << "Section name" << "\t\t" << "Redni broj simbola:" << endl;
	for (list<RelRow>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		cout << it->getRowTypeString() << "\t" << it->address << "\t\t" << it->sectionName << "\t\t" << it->symbolTableRowNumber << "\t\t\t" << endl;
	}

	cout << '\n';
}

void RelocationTable::writeToFile(ofstream &fileStream)
{
	string lastSection = "";
	for (list<RelRow>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		stringstream stream;
		stream << "0x" << hex << it->address;
		string address(stream.str());

		if (lastSection != it->sectionName) {
			if (lastSection != "")
			{
				fileStream << endl;
				holder->writeSegmentToFile(fileStream, lastSection);
				holder->addToWrittenRelocations(lastSection);
				fileStream << endl;
			}
			fileStream << "#rel" << it->sectionName << endl;
			lastSection = it->sectionName;
		}

		if (it->rowType == REL)
		{
			fileStream << address << " " << "R" << " " << it->symbolTableRowNumber << endl;
		}
		else 
		{
			fileStream << address << " " << "A" << " " << it->symbolTableRowNumber << endl;
		}
	}

	if (lastSection != "")
	{
		fileStream << endl;
		holder->writeSegmentToFile(fileStream, lastSection);
		holder->addToWrittenRelocations(lastSection);
		fileStream << endl;
	}

	holder->writeAllLeft(fileStream);
}

string RelRow::getRowTypeString()
{
	if (rowType == ABS) {
		return "A";
	}

	return "R";
}
