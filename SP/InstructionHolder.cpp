#include "InstructionHolder.h"

void InstructionHolder::addRow(string sectName, string data, bool isData)
{
	InstructionRow* newRow = new InstructionRow(sectName, data, isData);
	rows.push_back(*newRow);
	delete newRow;
}

void InstructionHolder::addToWrittenRelocations(string segmentName)
{
	writtenRelocations.push_back(segmentName);
}

void InstructionHolder::writeAllLeft(ofstream &fileStream)
{
	for (list<InstructionRow>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (find(begin(writtenRelocations), end(writtenRelocations), it->sectionName) == end(writtenRelocations))
		{
			writeSegmentToFile(fileStream, it->sectionName);
			writtenRelocations.push_back(it->sectionName);
		}
	}
}

void InstructionHolder::printRows()
{
	string lastSection = "";
	printed = 0;
	for (list<InstructionRow>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		it->data = removeBeginZeroes(it->data);

		if (lastSection != it->sectionName) {
			cout << endl << it->sectionName << endl;
			lastSection = it->sectionName;
			printed = 0;
		}

		size_t n = std::count(it->data.begin(), it->data.end(), ' ');
		int total = n + 1;
		int left = 0;
		if (printed + total > 16) {
			left = printed + total - 16;
		}

		if (left != 0) 
		{
			int pos = (16 - printed)*2 + (16-printed-1);
			cout << it->data.substr(0, pos) << endl;
			printed = 0;
			cout << it->data.substr(pos+1) + " ";
			printed = left;
		}
		else
		{
			cout << it->data + " ";
			printed = printed + total;
			if (printed == 16)
			{
				cout << endl;
				printed = 0;
			}
		}
	}

	cout << endl;
}

void InstructionHolder::writeToFile(ofstream &fileStream)
{
	string lastSection = "";
	printed = 0;
	for (list<InstructionRow>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		it->data = removeBeginZeroes(it->data);

		if (lastSection != it->sectionName) {
			fileStream << endl << it->sectionName << endl;
			lastSection = it->sectionName;
			printed = 0;
		}

		writeInstruction(fileStream, it->data, it->isData);
	}

	fileStream << '\n';
}

void InstructionHolder::writeInstruction(ofstream &fileStream, string data, bool isData)
{
	size_t n = std::count(data.begin(), data.end(), ' ');
	int total = n + 1;
	int left = 0;
	if (printed + total > 16) {
		left = printed + total - 16;
	}

	if (left != 0)
	{
		int pos = (16 - printed) * 2 + (16 - printed - 1);
		fileStream << data.substr(0, pos) << endl;
		printed = 0;
		fileStream << data.substr(pos + 1) + " ";
		printed = left;
	}
	else
	{
		fileStream << data + " ";
		printed = printed + total;
		if (printed == 16)
		{
			fileStream << endl;
			printed = 0;
		}
	}
}

void InstructionHolder::writeSegmentToFile(ofstream &fileStream, string segmentName)
{
	printed = 0;
	fileStream << segmentName << endl;
	for (list<InstructionRow>::iterator it = rows.begin(); it != rows.end(); it++)
	{
		if (it->sectionName == segmentName)
		{
			it->data = removeBeginZeroes(it->data);
			writeInstruction(fileStream, it->data, it->isData);
		}
	}

	fileStream << endl;
}