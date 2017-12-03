#include "SymbolTable.h"

unsigned int SymbolTable::id = 1;

SymbolTable::~SymbolTable() 
{
	// delete row list
}

void SymbolTable::addSegmentRow(string name,  unsigned int size)
{
	unsigned int secId = id++;
	Row* newRow = new Row(SEG, secId, name, secId, 0, size, "L");
	rows.push_back(*newRow);
	delete newRow;
}

void SymbolTable::addSegmentRowWithAddr(string name, unsigned int size, unsigned int addr)
{
	unsigned int secId = id++;
	Row* newRow = new Row(SEG, secId, name, secId, addr, size, "L");
	rows.push_back(*newRow);
	delete newRow;
}

void SymbolTable::addSymbolRow(string name, int sectionNumber, unsigned int value, string flg)
{
	Row* newRow = new Row(SYM, id++, name, sectionNumber, value, flg);
	rows.push_back(*newRow);
	delete newRow;
}

int SymbolTable::getSymbolValueForSection(string symbolName, int sectionNumber)
{
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->rowName == symbolName)
		{
			if (it->flags == "L")
			{
				if (sectionNumber != it->sectionRowNumber) {
					// add row in relocation table for it->sctionRowNumber
					return -2;
				}

				return it->address;
			}
			else
			{
				//create realocation table row
				return -1;
			}
		}
	}

	// greska, ne postoji simbol
	return string::npos;
}

int SymbolTable::getSegmentFromSymbol(string symbolName)
{
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->rowName == symbolName)
		{
			if (it->sectionRowNumber != -1)
			{
				return it->sectionRowNumber;
			}

			return string::npos - 1;
		}
	}

	// greska, ne postoji simbol
	return string::npos;
}

unsigned int SymbolTable::getSegmentByName(string segName)
{
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->rowName == segName)
		{
			return it->rowNumber;
		}
	}

	// greska, ne postoji simbol
	return string::npos;
}

unsigned int SymbolTable::getSymbolRowNumber(string symbolName)
{
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->rowName == symbolName)
		{
			return it->rowNumber;
		}
	}

	// greska, ne postoji simbol
	return string::npos;
}

int SymbolTable::getSymbolValue(string symbolName)
{
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->rowName == symbolName)
		{
			return it->address;
		}
	}

	// greska, ne postoji simbol
	return string::npos;
}

void SymbolTable::updateSymbolValue(unsigned int value, string symbolName)
{
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->rowName == symbolName)
		{
			it->address = value;
			return;
		}
	}
}

void SymbolTable::addToAllSegmentValues(string segName, int toAdd)
{
	unsigned int sectionNum = 0;
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->rowName == segName)
		{
			sectionNum = it->rowNumber;
		}
	}

	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->sectionRowNumber == sectionNum && it->rowName != segName)
		{
			it->address += toAdd;
		}
	}
}

void SymbolTable::printRows()
{
	cout << '\n';
	cout << "The contents of rows are: \n";
	cout << "Tip" << "\t" << "Redni broj" << "\t" << "Ime" << "\t\t" << "Redni broj sekcije" << "\t" << "Adresa/vrednost" << "\t\t" << "Velicina sekcije" << "\t" << "fleg" << endl;
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{		
		cout << it->getRowTypeString() << "\t" << it->rowNumber << "\t\t" << it->rowName << "\t\t" << it->sectionRowNumber << "\t\t\t" << it->address << "\t\t\t"  << it->sectionSize << "\t\t\t" << it->flags << endl;
	}

	cout << '\n';
}

void SymbolTable::writeToFile(ofstream &fileStream)
{
	fileStream << "#TabelaSimbola" << endl;
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		stringstream stream;
		stream << "0x" << hex << it->address;
		string address(stream.str());

		std::stringstream stream1;
		stream1 << "0x" << std::hex << it->sectionSize;
		std::string size(stream1.str());

		if (it->rowType == SEG)
		{
			// QUESTION : What flags??
			fileStream << "SEG" << " " << it->rowNumber << " " << it->rowName << " ";
			fileStream << it->sectionRowNumber << " " << address << " " << size << " " << it->flags << endl;
		}
		else
		{
			fileStream << "SYM" << " " << it->rowNumber << " " << it->rowName << " ";
			fileStream << it->sectionRowNumber << " " << address << " " << it->flags << endl;
		}
	}

	fileStream << endl;
}

int SymbolTable::getSegRowId(string segName)
{
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->rowName == segName)
		{
			return it->rowNumber;
		}
	}

	return string::npos;
}

void SymbolTable::updateSegmentRowSize(unsigned int newSize, string segName)
{
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->rowName == segName)
		{
			it->sectionSize = newSize;
			return;
		}
	}
}

bool SymbolTable::checkAndUpdateFlag(string name, string flag)
{
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->rowName == name)
		{
			it->flags = flag;
			return true;
		}
	}

	return false;
}

unsigned int SymbolTable::getRowAddr(string name)
{
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->rowName == name)
		{
			return it->address;
		}
	}

	// greska, ne postoji simbol
	return string::npos;
}

unsigned int SymbolTable::getRelocationAddrInfo(string name)
{
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->rowName == name)
		{
			if (it->flags == "G")
			{
				return it->rowNumber;
			}
			else if (it->flags == "L")
			{
				return it->sectionRowNumber;
			}
		}
	}

	//error
	return string::npos;
}

int SymbolTable::getRelocationInstrInfo(string name, bool isRelative)
{
	for (list<Row>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->rowName == name)
		{
			if (it->flags == "G")
			{
				// if absolute rel, return 00 00 00 00
				// -4 is ok for $
				// done in relation to 0
				if (isRelative)
				{
					return -4;
				}
				else
				{
					return 0;
				}
			}
			else if (it->flags == "L")
			{
				// if absolute rel, it->addr
				// if relative then it->addr - 4
				// done in relation to local address in section
				if (isRelative)
				{
					return it->address - 4;
				}
				else
				{
					return it->address;
				}
			}
		}
	}

	//error
	return string::npos;
}

string Row::getRowTypeString()
{
	if (rowType == SEG) {
		return "SEG";
	}

	return "SYM";
}

unsigned int operator+(int constant, const Row& row) {
	return constant + row.address;
}

unsigned int operator+(const Row& row1, const Row& row2) {
	return row1.address + row2.address;
}
