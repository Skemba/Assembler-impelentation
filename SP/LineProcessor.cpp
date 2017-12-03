#include "LineProcessor.h"

bool LineProcessor::hasToBeSection = false;
bool LineProcessor::lastLineORG = false;
bool LineProcessor::errorOccured = false;
bool LineProcessor::endOccured = false;
SymbolTable* LineProcessor::symTable = new SymbolTable();
RelocationTable* LineProcessor::relTable = new RelocationTable();
InstructionHolder* LineProcessor::holder = new InstructionHolder();
Section LineProcessor::previousSection = S_NONE;
Section LineProcessor::currentSection = S_NONE;
unsigned int LineProcessor::locationCounter = 0;
unsigned int LineProcessor::ORGLocation = string::npos;
string LineProcessor::currentSectionName = "";
list<string> LineProcessor::labelsLeft;
list<string> LineProcessor::orgDefined;

list<string> initArithmeticInstr() {
	list<string> temp;
	temp.push_back(ADD);
	temp.push_back(SUB);
	temp.push_back(MUL);
	temp.push_back(DIV);
	temp.push_back(MOD);
	temp.push_back(AND);
	temp.push_back(OR);
	temp.push_back(XOR);
	temp.push_back(NOT); // NOT has two operands
	temp.push_back(ASL);
	temp.push_back(ASR);

	return temp;
}

list<string> init4BList() {
	list<string> temp;
	temp.push_back(INT);
	temp.push_back(PUSH);
	temp.push_back(POP);
	temp.push_back(ADD);
	temp.push_back(SUB);
	temp.push_back(MUL);
	temp.push_back(DIV);
	temp.push_back(MOD);
	temp.push_back(AND);
	temp.push_back(OR);
	temp.push_back(XOR);
	temp.push_back(NOT);
	temp.push_back(ASL);
	temp.push_back(ASR);
	temp.push_back(RET);

	return temp;
}

list<string> init8BOneOpList() {
	list<string> temp;
	temp.push_back(JMP);
	temp.push_back(CALL);

	return temp;
}

list<string> init8BTwoOpList() {
	list<string> temp;
	temp.push_back(JZ);
	temp.push_back(JNZ);
	temp.push_back(JGZ);
	temp.push_back(JGEZ);
	temp.push_back(JLZ);
	temp.push_back(JLEZ);
	temp.push_back(LOAD);
	temp.push_back(STORE);
	temp.push_back(LOADUB);
	temp.push_back(LOADSB);
	temp.push_back(LOADUW);
	temp.push_back(LOADSW);
	temp.push_back(STOREB);
	temp.push_back(STOREW);

	return temp;
}

map<string, unsigned int> LineProcessor::createOpMap()
{
	map<string, unsigned int> map;
	map[RET] = 0x01;
	map[INT] = 0x00;
	map[PUSH] = 0x20;
	map[POP] = 0x21;
	map[ADD] = 0x30;
	map[SUB] = 0x31;
	map[MUL] = 0x32;
	map[DIV] = 0x33;
	map[MOD] = 0x34;
	map[AND] = 0x35;
	map[OR] = 0x36;
	map[XOR] = 0x37;
	map[NOT] = 0x38;
	map[ASL] = 0x39;
	map[ASR] = 0x3A;
	map[JMP] = 0x02;
	map[CALL] = 0x03;
	map[JZ] = 0x04;
	map[JNZ] = 0x05;
	map[JGZ] = 0x06;
	map[JGEZ] = 0x07;
	map[JLZ] = 0x08;
	map[JLEZ] = 0x09;
	map[LOAD] = 0x10;
	map[LOADUB] = 0x10;
	map[LOADSB] = 0x10;
	map[LOADUW] = 0x10;
	map[LOADSW] = 0x10;
	map[STORE] = 0x11;
	map[STOREB] = 0x11;
	map[STOREW] = 0x11;

	return map;
}

map<string, unsigned int> LineProcessor::createAddressCodeMap()
{
	map<string, unsigned int> map;
	map[IMM] = 0x04;
	map[REGDIR] = 0x00;
	map[MEMDIR] = 0x06;
	map[REGIND] = 0x02;
	map[REGINDPOM] = 0x07;

	return map;
}

map<string, unsigned int> LineProcessor::createRegisterCodeMap()
{
	map<string, unsigned int> map;
	map[R0] = 0x0;
	map[R1] = 0x1;
	map[R2] = 0x2;
	map[R3] = 0x3;
	map[R4] = 0x4;
	map[R5] = 0x5;
	map[R6] = 0x6;
	map[R7] = 0x7;
	map[R8] = 0x8;
	map[R9] = 0x9;
	map[R10] = 0xa;
	map[R11] = 0xb;
	map[R12] = 0xc;
	map[R13] = 0xd;
	map[R14] = 0xe;
	map[R15] = 0xf;
	map[SP] = 0x10;
	map[PC] = 0x11;

	return map;
}

map<string, unsigned int> LineProcessor::createDataTypeCodeMap()
{
	map<string, unsigned int> map;
	map[LONG] = 0x00;
	map[WORD] = 0x01;
	map[WORDSIGNED] = 0x05;
	map[BYTE] = 0x03;
	map[BYTESIGNED] = 0x07;
	return map;
}

bool LineProcessor::hasErrors()
{
	return errorOccured;
}

bool LineProcessor::hasEnded()
{
	return endOccured;
}

const list<string> LineProcessor::instr4B(init4BList());
const list<string> LineProcessor::instr8BOneOp(init8BOneOpList());
const list<string> LineProcessor::instr8BTwoOp(init8BTwoOpList());
const list<string> LineProcessor::arithmeticInstr(initArithmeticInstr());
const map<string, unsigned int> LineProcessor::opMap = LineProcessor::createOpMap();
const map<string, unsigned int> LineProcessor::addressCodeMap = LineProcessor::createAddressCodeMap();
const map<string, unsigned int> LineProcessor::registerCodeMap = LineProcessor::createRegisterCodeMap();
const map<string, unsigned int> LineProcessor::dataTypeCodeMap = LineProcessor::createDataTypeCodeMap();

LineProcessor::LineProcessor(string line1)
{
	line = line1;
	wordInLine = 0;
	latestSymbol = "";
	currentDirORG = false;
}

LineProcessor::~LineProcessor()
{
}

bool LineProcessor::firstPass()
{
	string word;
	currentLineStream << line;

	while (currentLineStream >> word)
	{
		wordInLine++;
		
		if (wordInLine == 1 && has_prefix(word, "."))
		{
			if (word.substr(0, 4) == BSS) 
			{
				callBeforeSegmentAddMethods();
				currentSection = S_BSS;
				callAfterSegmentAddMethods(word);
			}
			else if (word.substr(0, 4) == ".end") 
			{
				callBeforeSegmentAddMethods();
				endOccured = true;
				return false;
			}
			else if (word.substr(0, 5) == DATA) 
			{
				callBeforeSegmentAddMethods();
				currentSection = S_DATA;
				callAfterSegmentAddMethods(word);
			}
			else if (word.substr(0, 5) == TEXT) 
			{
				callBeforeSegmentAddMethods();
				currentSection = S_TEXT;
				callAfterSegmentAddMethods(word);
			}
			else if (word.substr(0, 7) == RODATA) 
			{
				callBeforeSegmentAddMethods();
				currentSection = S_RODATA;
				callAfterSegmentAddMethods(word);
			}
			else if (word.substr(0, 7) == GLOBAL)
			{
				// do nothing in first pass
			}
			else 
			{
				cout << "[ERROR] Directive not allowed";
				errorOccured = true;
				return false;
			}
		} else if (wordInLine == 1 && has_suffix(word, ":"))
		{
			string labelName = word.substr(0, word.size() - 1);
			if (currentLineStream.rdbuf()->in_avail() == 0) 
			{
				labelsLeft.push_front(labelName);
			}
			else
			{
				unsigned int segmentRowNumber = symTable->getSegRowId(currentSectionName);

				if (segmentRowNumber == string::npos)
				{
					cout << "[ERROR] Wrong section name: " + currentSectionName;
					errorOccured = true;
					return false;
				}

				symTable->addSymbolRow(labelName, segmentRowNumber, locationCounter, "L");
			}
		}
		else if ((wordInLine == 1 || wordInLine == 2) && word == BYTE) 
		{
			if (!checkForLeftLabels())
			{
				return false;
			}
			getline(currentLineStream, word);
			unsigned int found = word.find(";");
			if (found != string::npos)
			{
				word = word.substr(0, found);
			}
			unsigned int dupIndex = word.find(DUP);
			unsigned int numOfTimes = 1;
			if (dupIndex != string::npos)
			{
				string numOfTimesString = word.substr(0, dupIndex - 1);
				numOfTimesString.erase(std::remove_if(numOfTimesString.begin(), numOfTimesString.end(), ::isspace), numOfTimesString.end());
				numOfTimes = evalInstrExpr(numOfTimesString, false, true);
			}
			locationCounter += numOfTimes;
			return true;
		}
		else if ((wordInLine == 1 || wordInLine == 2) && word == WORD)
		{
			if (!checkForLeftLabels())
			{
				return false;
			}
			getline(currentLineStream, word);
			unsigned int found = word.find(";");
			if (found != string::npos)
			{
				word = word.substr(0, found);
			}
			unsigned int dupIndex = word.find(DUP);
			unsigned int numOfTimes = 1;
			if (dupIndex != string::npos)
			{
				string numOfTimesString = word.substr(0, dupIndex - 1);
				numOfTimesString.erase(std::remove_if(numOfTimesString.begin(), numOfTimesString.end(), ::isspace), numOfTimesString.end());
				numOfTimes = evalInstrExpr(numOfTimesString, false, true);
			}
			locationCounter += numOfTimes * 2;
			return true;
		}
		else if ((wordInLine == 1 || wordInLine == 2) && word == LONG) 
		{
			if (!checkForLeftLabels())
			{
				return false;
			}
			getline(currentLineStream, word);
			unsigned int found = word.find(";");
			if (found != string::npos)
			{
				word = word.substr(0, found);
			}
			unsigned int dupIndex = word.find(DUP);
			unsigned int numOfTimes = 1;
			if (dupIndex != string::npos)
			{
				string numOfTimesString = word.substr(0, dupIndex - 1);
				numOfTimesString.erase(std::remove_if(numOfTimesString.begin(), numOfTimesString.end(), ::isspace), numOfTimesString.end());
				numOfTimes = evalInstrExpr(numOfTimesString, false, true);
			}
			locationCounter += numOfTimes * 4;
			return true;
		}
		else if (word == DEF && latestSymbol != "")
		{
			if (!checkForLeftLabels())
			{
				return false;
			}
			symTable->addSymbolRow(latestSymbol, -1, 0, "L");
		}
		else if ((wordInLine == 1 || wordInLine == 2))
		{
			if (!lookForInstruction(word, false)) {
				latestSymbol = word;
			}
			else {
				return true;
			}
		}

	}

	return true;
}

bool LineProcessor::checkForLeftLabels()
{
	if (labelsLeft.size() != 0)
	{
		while (labelsLeft.size() > 0)
		{
			unsigned int segmentRowNumber = symTable->getSegRowId(currentSectionName);

			if (segmentRowNumber == string::npos)
			{
				cout << "[ERROR] Wrong section name";
				errorOccured = true;
				return false;
			}

			string label = labelsLeft.front();

			symTable->addSymbolRow(label, segmentRowNumber, locationCounter, "L");
			labelsLeft.pop_front();
		}
	}

	return true;
}

void LineProcessor::initSecondPass()
{
	locationCounter = 0;
	previousSection = S_NONE;
	currentSection = S_NONE;
	currentSectionName = "";
	latestSymbol = "";
	endOccured = false;
}

bool LineProcessor::lookForInstruction(string word, bool secondPass)
{
	if (find(instr4B.begin(), instr4B.end(), word) != instr4B.end())
	{
		if (!checkForLeftLabels())
		{
			return false;
		}
		string operation = word;
		locationCounter += 4;

		if (secondPass) {
			string opCode = hexByteToString(opMap.find(operation)->second);
			if (operation == RET) {
				string instrCode = opCode + "000000000000000000000000";
				int count = 0;
				string bef = "";
				for (int i = 0; i < instrCode.size(); i++)
				{
					if (instrCode[i] == '0')
						count++;
					else
						break;
				}
				bool parity = false;
				if (count > 4 && count < 8)
				{
					bef = "0";
					parity = true;

				}
				else if (count > 8 && count < 16)
				{
					bef = "00 ";
				}
				else if (count > 16)
				{
					bef = "00 00 ";
				}
				holder->addRow(currentSectionName, bef + bits32ToHex(instrCode.substr(0, 32), false));
			}
			else if (operation == INT || operation == PUSH || operation == POP) {
				currentLineStream >> word;
				string operand = word;
				// INT is : opcode(8)+"000"+Reg0(5)+Unused(16)
				string instrCode = opCode + "000" + fiveBitToString(registerCodeMap.find(operand)->second) + "0000000000000000";
				int count = 0;
				string bef = "";
				for (int i = 0; i < instrCode.size(); i++)
				{
					if (instrCode[i] == '0')
						count++;
					else
						break;
				}
				bool parity = false;
				if (count > 4 && count < 8)
				{
					bef = "0";
					parity = true;

				}
				else if (count > 8 && count < 16)
				{
					bef = "00 ";
				}
				else if (count > 16)
				{
					bef = "00 00 ";
				}
				holder->addRow(currentSectionName, bef + bits32ToHex(instrCode.substr(0, 32), false));

			}
			else if (find(arithmeticInstr.begin(), arithmeticInstr.end(), operation) != arithmeticInstr.end()) {
				// read 3 operands unless operation is NOT
				string firstOp;
				string secondOp;
				string thirdOp;
				string rest;
				currentLineStream >> word;
				size_t pos = word.find(",");
				if (pos != string::npos && pos != word.size() - 1) {
					firstOp = word.substr(0, pos);
					if (operation == NOT) {
						secondOp = word.substr(pos + 1, word.size());
					}
					else {
						rest = word.substr(pos + 1, word.size());
						pos = rest.find(",");
						if (pos != string::npos && pos != rest.size() - 1) {
							secondOp = rest.substr(0, pos);
							thirdOp = rest.substr(pos + 1, rest.size());
						}
						else if (pos == rest.size() - 1) {
							secondOp = rest.substr(0, rest.size() - 1);
							currentLineStream >> rest;
							thirdOp = rest;
						}
						else
						{
							cout << "[ERROR] Number of operands mismatch";
							errorOccured = true;
							return false;
						}
					}
				}
				else if (pos == word.size() - 1) {
					firstOp = word.substr(0, word.size() - 1);
					currentLineStream >> word;
					if (operation == NOT) {
						secondOp = word;
					}
					else {
						rest = word;
						pos = rest.find(",");
						if (pos != string::npos && pos != rest.size() - 1) {
							secondOp = rest.substr(0, pos);
							thirdOp = rest.substr(pos + 1, rest.size());
						}
						else if (pos == rest.size() - 1) {
							secondOp = rest.substr(0, rest.size() - 1);
							currentLineStream >> rest;
							thirdOp = rest;
						}
						else
						{
							cout << "[ERROR] Number of operands mismatch";
							errorOccured = true;
							return false;
						}
					}
				}
				else
				{
					cout << "[ERROR] Number of operands mismatch";
					errorOccured = true;
					return false;
				}

				if (operation == NOT) {
					thirdOp = "R0";
				}

				string reg0 = fiveBitToString(registerCodeMap.find(firstOp)->second);
				string reg1 = fiveBitToString(registerCodeMap.find(secondOp)->second);
				string reg2 = fiveBitToString(registerCodeMap.find(thirdOp)->second);

				// aritmetic instructions are : opcode(8)+"000"+Reg0(5)+Reg1(5)+Reg2(5)+Unused(6)
				string instrCode = opCode + "000" + reg0 + reg1 + reg2 + "000000";
				holder->addRow(currentSectionName, bits32ToHex(instrCode.substr(0, 32)));

			}
			else
			{
				cout << "[ERROR] Missing implementation of instruction: " + operation;
				errorOccured = true;
				return false;
			}
		}
		return true;
	} else if (find(instr8BOneOp.begin(), instr8BOneOp.end(), word) != instr8BOneOp.end())
	{
		if (!checkForLeftLabels())
		{
			return false;
		}
		string operation = word;
		currentLineStream >> word;
		string firstOp = word;
		if (firstOp.find('[') == string::npos)
		{
			string restOfLine;
			getline(currentLineStream, restOfLine);
			firstOp += restOfLine;
			unsigned int found = firstOp.find(";");
			if (found != string::npos)
			{
				firstOp = firstOp.substr(0, found);
			}
		}

		while (firstOp.find('[') != string::npos && firstOp.find(']') == string::npos)
		{
			currentLineStream >> word;
			firstOp = firstOp + word;

		}
		if (!isRegInd(word) && !isRegDir(word)) {
			locationCounter += 8;
		}
		else {
			locationCounter += 4;
		}

		if (secondPass) {
			string addrType;
			int offset;
			firstOp.erase(std::remove_if(firstOp.begin(), firstOp.end(), ::isspace), firstOp.end());
			if (isRegIndPom(firstOp)) {
				addrType = REGINDPOM;
				string whole = firstOp;
				firstOp = findRegister(firstOp);
				offset = evalInstrExpr(findOffset(whole), false);
			}
			else if (isRegInd(firstOp))
			{
				addrType = REGIND;
				firstOp = findRegister(firstOp);
			}
			else
			{
				if (has_prefix(firstOp, "$"))
				{
					addrType = REGINDPOM;
					firstOp = firstOp.substr(1);
					offset = evalInstrExpr(firstOp, true);
					firstOp = findRegister("PC");
				}
				else
				{
					addrType = MEMDIR;
					offset = evalInstrExpr(firstOp, false);
				}
			}

			if (!errorOccured)
			{

				// opcode(8)+addr(3)+reg0(5)+unused(5)+unused(11)
				// check for errors so you wouldnt pass to hexByteToString and other funcs
				string opCode = hexByteToString(opMap.find(operation)->second);
				string addrCode = threeBitToString(addressCodeMap.find(addrType)->second);
				string reg0 = fiveBitToString(0x00);
				if (addrType == REGIND || addrType == REGINDPOM)
				{
					reg0 = fiveBitToString(registerCodeMap.find(firstOp)->second);
				}
				string reg1 = fiveBitToString(0x00);
				string addr;
				string unused = fiveBitToString(0x00);

				string instrCode = opCode + addrCode + reg0 + unused + "00000000000";
				int count = 0;
				string bef = "";
				for (int i = 0; i < instrCode.size(); i++)
				{
					if (instrCode[i] == '0')
						count++;
					else
						break;
				}
				bool parity = false;
				if (count > 4 && count < 8)
				{
					bef = "0";
					parity = true;

				}
				else if (count > 8 && count < 16)
				{
					bef = "00 ";
				}
				else if (count > 16)
				{
					bef = "00 00 ";
				}
				if (addrType == REGINDPOM || addrType == MEMDIR)
				{
					addr = hex4BytesToString(offset);
					instrCode = instrCode + " " + bitAddressToFileHex(addr);
				}

				if (instrCode.size() > 32)
				{
					holder->addRow(currentSectionName, bef + bits32ToHex(instrCode.substr(0, 32), false) + " " + bits32ToHex(instrCode.substr(33)));
				}
				else
				{
					holder->addRow(currentSectionName, bef + bits32ToHex(instrCode.substr(0, 32), false));
				}
			}
		}

		return true;
	}
	else if (find(instr8BTwoOp.begin(), instr8BTwoOp.end(), word) != instr8BTwoOp.end()) 
	{
		if (!checkForLeftLabels())
		{
			return false;
		}
		string operation = word;
		string firstOp;
		string secondOp;
		currentLineStream >> word;
		size_t pos = word.find(",");
		if (pos != string::npos && pos != word.size() - 1) {
			firstOp = word.substr(0, pos);
			secondOp = word.substr(pos + 1, word.size());
			if (!isRegInd(secondOp) && !isRegDir(secondOp)) {
				locationCounter += 8;
			}
			else {
				locationCounter += 4;
			}
		}
		else if(pos == word.size() - 1) {
			firstOp = word.substr(0, word.size() - 1);
			currentLineStream >> word;
			secondOp = word;
			if (!isRegInd(secondOp) && !isRegDir(secondOp)) {
				locationCounter += 8;
			}
			else {
				locationCounter += 4;
			}
		}
		else
		{
			cout << "[ERROR] Number of operands mismatch";
			errorOccured = true;
			return false;
		}

		if (secondOp.find('[') == string::npos)
		{
			string restOfLine;
			getline(currentLineStream, restOfLine);
			secondOp += restOfLine;
			unsigned int found = secondOp.find(";");
			if (found != string::npos)
			{
				secondOp = secondOp.substr(0, found);
			}
		}

		while (secondOp.find('[') != string::npos && secondOp.find(']') == string::npos)
		{
			currentLineStream >> word;
			secondOp = secondOp + word;
		}

		if (secondPass) {
			string addrType;
			int offset;
			string immString;
			int immValue;
			secondOp.erase(std::remove_if(secondOp.begin(), secondOp.end(), ::isspace), secondOp.end());
			if (isRegIndPom(secondOp)) {
				addrType = REGINDPOM;
				string whole = secondOp;
				secondOp = findRegister(secondOp);
				offset = evalInstrExpr(findOffset(whole), false);
			}
			else if (isImm(secondOp)) {
				addrType = IMM;
				immString = secondOp.substr(1, secondOp.size());
				offset = evalInstrExpr(immString, false);
			}
			else if (isRegInd(secondOp)) {
				addrType = REGIND;
				secondOp = findRegister(secondOp);
			}
			else if (isRegDir(secondOp)) {
				addrType = REGDIR;
			}
			else {
				if (has_prefix(secondOp, "$"))
				{
					addrType = REGINDPOM;
					secondOp = secondOp.substr(1);
					offset = evalInstrExpr(secondOp, true);
					secondOp = findRegister("PC");
				}
				else
				{
					addrType = MEMDIR;
					offset = evalInstrExpr(secondOp, false);
				}
			}

			// check for errors so you wouldnt pass to hexByteToString and other funcs
			string opCode = hexByteToString(opMap.find(operation)->second);
			string addrCode = threeBitToString(addressCodeMap.find(addrType)->second);
			string reg0;
			string reg1;
			string addr;
			string unused = fiveBitToString(0x00);
			string unused3 = threeBitToString(0x00);
			string type;

			if (!errorOccured)
			{
				// LOAD and STORE instructions converter opcode(8)+addr(3)+reg0(5)+reg1(5)+unused(5)+type(3)+Unused(3)
				if (operation == LOAD || operation == LOADSB || operation == LOADUB || operation == LOADUW || operation == LOADSW) {
					reg1 = fiveBitToString(registerCodeMap.find(firstOp)->second);
					reg0 = fiveBitToString(0x00);
					if (addrType == REGDIR || addrType == REGINDPOM || addrType == REGIND) {
						reg0 = fiveBitToString(registerCodeMap.find(secondOp)->second);
					}
					if (operation == LOAD) {
						type = threeBitToString(dataTypeCodeMap.find(LONG)->second);
					}
					else if (operation == LOADSB) {
						type = threeBitToString(dataTypeCodeMap.find(BYTESIGNED)->second);
					}
					else if (operation == LOADUB) {
						type = threeBitToString(dataTypeCodeMap.find(BYTE)->second);
					}
					else if (operation == LOADUW) {
						type = threeBitToString(dataTypeCodeMap.find(WORD)->second);
					}
					else {
						type = threeBitToString(dataTypeCodeMap.find(WORDSIGNED)->second);
					}

					// remove spaces when done testing
					string instrCode = opCode + addrCode + reg0 + reg1 + unused + type + unused3;
					if (addrType == REGINDPOM || addrType == MEMDIR || addrType == IMM) {
						addr = hex4BytesToString(offset);
						instrCode = instrCode + " " + bitAddressToFileHex(addr);
					}
					if (instrCode.size() > 32)
					{
						holder->addRow(currentSectionName, bits32ToHex(instrCode.substr(0, 32)) + " " + bits32ToHex(instrCode.substr(33)));
					}
					else
					{
						holder->addRow(currentSectionName, bits32ToHex(instrCode.substr(0, 32)));
					}
				}
				else if (operation == STORE || operation == STOREB || operation == STOREW) {
					reg1 = fiveBitToString(registerCodeMap.find(firstOp)->second);
					reg0 = fiveBitToString(0x00);
					if (addrType == REGDIR || addrType == REGINDPOM || addrType == REGIND) {
						reg0 = fiveBitToString(registerCodeMap.find(secondOp)->second);
					}
					if (addrType == IMM) {
						cout << "[ERROR] IMM not allowed with STORE instructions";
						errorOccured = true;
						return false;
					}
					if (operation == STORE) {
						type = threeBitToString(dataTypeCodeMap.find(LONG)->second);
					}
					else if (operation == STOREB) {
						type = threeBitToString(dataTypeCodeMap.find(BYTE)->second);
					}
					else {
						type = threeBitToString(dataTypeCodeMap.find(WORD)->second);
					}

					// remove spaces when done testing
					string instrCode = opCode + addrCode + reg0 + reg1 + unused + type + unused3;
					if (addrType == REGINDPOM || addrType == MEMDIR) {
						addr = hex4BytesToString(offset);
						instrCode = instrCode + " " + bitAddressToFileHex(addr);
					}

					if (instrCode.size() > 32)
					{
						holder->addRow(currentSectionName, bits32ToHex(instrCode.substr(0, 32)) + " " + bits32ToHex(instrCode.substr(33)));
					}
					else
					{
						holder->addRow(currentSectionName, bits32ToHex(instrCode.substr(0, 32)));
					}
				}
				else if (operation == JZ || operation == JNZ || operation == JGEZ || operation == JGZ || operation == JLZ || operation == JLEZ) {
					reg1 = fiveBitToString(registerCodeMap.find(firstOp)->second);
					reg0 = fiveBitToString(0x00);
					if (addrType == REGDIR || addrType == REGINDPOM || addrType == REGIND) {
						reg0 = fiveBitToString(registerCodeMap.find(secondOp)->second);
					}
					// opcode(8)+addr(3)+reg0(5)+reg1(5)+unused(11)
					string instrCode = opCode + addrCode + reg0 + reg1 + "00000000000";
					int count = 0;
					string bef = "";
					for (int i = 0; i < instrCode.size(); i++)
					{
						if (instrCode[i] == '0')
							count++;
						else
							break;
					}
					bool parity = false;
					if (count > 4 && count < 8)
					{
						bef = "0";
						parity = true;

					}
					else if (count > 8 && count < 16)
					{
						bef = "00 ";
					}
					else if (count > 16)
					{
						bef = "00 00 ";
					}
					if (addrType == REGINDPOM || addrType == MEMDIR) {
						addr = hex4BytesToString(offset);
						instrCode = instrCode + " " + bitAddressToFileHex(addr);
					}
					if (instrCode.size() > 32)
					{
						holder->addRow(currentSectionName, bef + bits32ToHex(instrCode.substr(0, 32), false) + " " + bits32ToHex(instrCode.substr(33)));
					}
					else
					{
						holder->addRow(currentSectionName, bef + bits32ToHex(instrCode.substr(0, 32), false));
					}
				}
			}
		}


		return true;
	}
	
	return false;
}

SymbolTable* LineProcessor::getSymTable()
{
	return symTable;
}

RelocationTable * LineProcessor::getRelTable()
{
	return relTable;
}

InstructionHolder * LineProcessor::getInstrHolder()
{
	return holder;
}

void LineProcessor::callAfterSegmentAddMethods(string word)
{
	currentSectionName = word;
	locationCounter = 0;
	symTable->addSegmentRow(currentSectionName, locationCounter);
}

void LineProcessor::callBeforeSegmentAddMethods()
{
	if (currentSection != S_NONE) {
		symTable->updateSegmentRowSize(locationCounter, currentSectionName);
	}
}

void LineProcessor::changeSegmentStartAddr()
{
	if (lastLineORG && ORGLocation != string::npos)
	{
		symTable->updateSymbolValue(ORGLocation, currentSectionName);
		symTable->addToAllSegmentValues(currentSectionName, ORGLocation);
		orgDefined.push_back(currentSectionName);
		locationCounter = ORGLocation;
		ORGLocation = string::npos;
	}
}

bool LineProcessor::secondPass() 
{
	string word;
	currentLineStream << line;

	while (currentLineStream >> word)
	{
		wordInLine++;

		if (wordInLine == 1 && has_prefix(word, "."))
		{
			if (word.substr(0, 4) == BSS) {
				currentSectionName = word;
				locationCounter = 0;
				changeSegmentStartAddr();
				currentSection = S_BSS;
			}
			else if (word.substr(0, 4) == ".end")
			{
				// ignore rest of the file
				endOccured = true;
				return false;
			}
			else if (word.substr(0, 5) == TEXT) 
			{
				currentSectionName = word;
				locationCounter = 0;
				changeSegmentStartAddr();
				currentSection = S_TEXT;
			}
			else if (word.substr(0, 5) == DATA)
			{
				currentSectionName = word;
				locationCounter = 0;
				changeSegmentStartAddr();
				currentSection = S_DATA;
			}
			else if (word.substr(0, 7) == RODATA) 
			{
				currentSectionName = word;
				locationCounter = 0;
				changeSegmentStartAddr();
				currentSection = S_RODATA;
			}
			else if (word.substr(0, 7) == GLOBAL)
			{
				currentLineStream >> word;
				size_t pos = word.find(",");
				string rest = "";
				if (pos == string::npos)
				{
					if (!symTable->checkAndUpdateFlag(word, "G"))
					{
						symTable->addSymbolRow(word, 0, 0, "G");
					}
				}
				else
				{
					while (pos != string::npos) {
						string first = word.substr(0, pos);
						if (!symTable->checkAndUpdateFlag(first, "G"))
						{
							symTable->addSymbolRow(first, 0, 0, "G");
						}
						if (pos != word.size() - 1 && pos != string::npos)
						{
							rest = word.substr(pos + 1, word.size());
							word = rest;
							pos = rest.find(",");
						}
						else if (pos == word.size() - 1)
						{
							currentLineStream >> word;
							rest = word;
							if (first == rest.substr(0, 1))
							{
								cout << "[ERROR] There was an error declaring global symbols, either there is no symbol after ',' or symbol has been duplicated";
								errorOccured = true;
								return false;
							}
							pos = word.find(",");
						}
					}
					if (rest != "") {
						if (!symTable->checkAndUpdateFlag(rest, "G"))
						{
							symTable->addSymbolRow(rest, 0, 0, "G");
						}
					}
				}
			}
			else {
				cout << "[ERROR] Directive not allowed";
				errorOccured = true;
				return false;
			}
		}
		else if (wordInLine == 1 && has_suffix(word, ":"))
		{
			// already done in first pass
		}
		else if ((wordInLine == 1 || wordInLine == 2) && word == BYTE)
		{
			string restOfLine;
			getline(currentLineStream, restOfLine);
			unsigned int found = restOfLine.find(";");
			if (found != string::npos)
			{
				restOfLine = restOfLine.substr(0, found);
			}

			unsigned int dupIndex = restOfLine.find(DUP);
			unsigned int numOfTimes = 1;
			if (dupIndex != string::npos)
			{
				string numOfTimesString = restOfLine.substr(0, dupIndex - 1);
				numOfTimesString.erase(std::remove_if(numOfTimesString.begin(), numOfTimesString.end(), ::isspace), numOfTimesString.end());
				numOfTimes = evalInstrExpr(numOfTimesString, false, true);
				restOfLine = restOfLine.substr(dupIndex + 4);
			}

			restOfLine.erase(std::remove_if(restOfLine.begin(), restOfLine.end(), ::isspace), restOfLine.end());
			unsigned int result = evalInstrExpr(restOfLine, false, true);
			for (int i = 0; i < numOfTimes; i++)
			{
				holder->addRow(currentSectionName, bits8ToHex(hexByteToString(result), false), true);
			}
			locationCounter += numOfTimes;
			return true;
		}
		else if ((wordInLine == 1 || wordInLine == 2) && word == WORD)
		{
			string restOfLine;
			getline(currentLineStream, restOfLine);
			unsigned int found = restOfLine.find(";");
			if (found != string::npos)
			{
				restOfLine = restOfLine.substr(0, found);
			}

			unsigned int dupIndex = restOfLine.find(DUP);
			unsigned int numOfTimes = 1;
			if (dupIndex != string::npos)
			{
				string numOfTimesString = restOfLine.substr(0, dupIndex - 1);
				numOfTimesString.erase(std::remove_if(numOfTimesString.begin(), numOfTimesString.end(), ::isspace), numOfTimesString.end());
				numOfTimes = evalInstrExpr(numOfTimesString, false, true);
				restOfLine = restOfLine.substr(dupIndex + 4);
			}

			restOfLine.erase(std::remove_if(restOfLine.begin(), restOfLine.end(), ::isspace), restOfLine.end());
			unsigned int result = evalInstrExpr(restOfLine, false, true);
			for (int i = 0; i < numOfTimes; i++)
			{
				holder->addRow(currentSectionName, hexReverse(bits16ToHex(hex2BytesToString(result), false)), true);
			}

			locationCounter += 2 * numOfTimes;
			return true;
		}
		else if ((wordInLine == 1 || wordInLine == 2) && word == LONG)
		{
			string restOfLine;
			getline(currentLineStream, restOfLine);
			unsigned int found = restOfLine.find(";");
			if (found != string::npos)
			{
				restOfLine = restOfLine.substr(0, found);
			}

			unsigned int dupIndex = restOfLine.find(DUP);
			unsigned int numOfTimes = 1;
			if (dupIndex != string::npos)
			{
				string numOfTimesString = restOfLine.substr(0, dupIndex - 1);
				numOfTimesString.erase(std::remove_if(numOfTimesString.begin(), numOfTimesString.end(), ::isspace), numOfTimesString.end());
				numOfTimes = evalInstrExpr(numOfTimesString, false, true);
				restOfLine = restOfLine.substr(dupIndex + 4);
			}

			restOfLine.erase(std::remove_if(restOfLine.begin(), restOfLine.end(), ::isspace), restOfLine.end());
			unsigned int result = evalInstrExpr(restOfLine, false, true);
			for (int i = 0; i < numOfTimes; i++)
			{
				holder->addRow(currentSectionName, hexReverse(bits32ToHex(hex4BytesToString(result), false)), true);
			}

			locationCounter += 4 * numOfTimes;
			return true;
		}
		else if (word == DEF && latestSymbol != "")
		{
			string expression;
			getline(currentLineStream, expression);
			symTable->updateSymbolValue(evalInstrExpr(expression, false, true), latestSymbol);
		}
		else if (wordInLine == 1 && word == ORG)
		{
			currentDirORG = true;
			lastLineORG = true;
			string expression;
			getline(currentLineStream, expression);
			ORGLocation = evalInstrExpr(expression, false);
		}
		else if ((wordInLine == 1 || wordInLine == 2))
		{
			if (!lookForInstruction(word, true)) {
				latestSymbol = word;
			}
			else {
				return true;
			}
		}
	}

	if (!currentDirORG)
	{
		lastLineORG = false;
	}

	return true;
}

int LineProcessor::evalInstrExpr(string expression, bool isRelative, bool isData)
{
	replaceHexAndOct(expression);

	list<string> literalList = splitWithDelimiters(expression, "+-*/)(");
	while (literalList.size() > 0)
	{
		std::string word = literalList.front();

		if (word.find_first_not_of("0123456789+-*/)(") != string::npos) {
			// word is not a number
			unsigned int sectionRowNumber = symTable->getSegmentByName(currentSectionName);

			if (sectionRowNumber == string::npos)
			{
				cout << "[ERROR] Couldn't find segment";
				errorOccured = true;
				return false;
			}

			int addr = symTable->getSymbolValueForSection(word, sectionRowNumber);

			if (addr != -1 && addr != -2 && addr != string::npos)
			{
				bool skip = false;
				for (list<string>::iterator it = orgDefined.begin(); it != orgDefined.end(); it++)
				{
					if (*it == currentSectionName)
					{
						skip = true;
					}
				}

				if (skip)
				{
					if (isRelative)
					{
						unsigned int offset = addr - locationCounter;
						expression = replaceString(expression, word, to_string(offset));
					}
					else
					{
						expression = replaceString(expression, word, to_string(addr));
					}
				}
				else
				{

					unsigned int sectionRowNum = symTable->getSegmentFromSymbol(word);

					if (sectionRowNum == string::npos)
					{
						cout << "[ERROR] Symbol not in symbol table: " + word;
						errorOccured = true;
						return false;
					}

					if (sectionRowNum == string::npos - 1)
					{
						expression = replaceString(expression, word, to_string(symTable->getRelocationInstrInfo(word, false)));
					}
					else
					{
						if (isData)
						{
							relTable->addAbsoulteRow(locationCounter, currentSectionName, sectionRowNum);
							expression = replaceString(expression, word, to_string(symTable->getRelocationInstrInfo(word, false)));
						}
						else
						{
							if (isRelative)
							{
								unsigned int offset = addr - locationCounter;
								expression = replaceString(expression, word, to_string(offset));
							}
							else
							{
								relTable->addAbsoulteRow(locationCounter - 4, currentSectionName, sectionRowNum);
								expression = replaceString(expression, word, to_string(symTable->getRelocationInstrInfo(word, false)));
							}
						}
					}
				}
			}
			else if (addr == -2)
			{
				unsigned int sectionRowNum = symTable->getSegmentFromSymbol(word);

				if (sectionRowNum == string::npos)
				{
					cout << "[ERROR] Symbol not in symbol table: " + word;
					errorOccured = true;
					return false;
				}

				if (sectionRowNum == string::npos - 1)
				{
					expression = replaceString(expression, word, to_string(symTable->getRelocationInstrInfo(word, false)));
				}
				else
				{

					if (isData)
					{
						relTable->addAbsoulteRow(locationCounter, currentSectionName, sectionRowNum);
						expression = replaceString(expression, word, to_string(symTable->getRelocationInstrInfo(word, false)));
					}
					else
					{
						if (isRelative)
						{
							relTable->addRelativeRow(locationCounter - 4, currentSectionName, sectionRowNum);
							expression = replaceString(expression, word, to_string(symTable->getRelocationInstrInfo(word, true)));
						}
						else
						{
							relTable->addAbsoulteRow(locationCounter - 4, currentSectionName, sectionRowNum);
							expression = replaceString(expression, word, to_string(symTable->getRelocationInstrInfo(word, false)));
						}
					}
				}
			}
			else if (addr == -1)
			{
				unsigned int symbolRowNum = symTable->getSymbolRowNumber(word);
				
				if (symbolRowNum == string::npos)
				{
					cout << "[ERROR] Symbol not in symbol table: " + word;
					errorOccured = true;
					return false;
				}


				if (isData)
				{
					relTable->addAbsoulteRow(locationCounter, currentSectionName, symbolRowNum);
					expression = replaceString(expression, word, to_string(symTable->getRelocationInstrInfo(word, false)));
				}
				else
				{
					if (isRelative)
					{
						relTable->addRelativeRow(locationCounter - 4, currentSectionName, symbolRowNum);
						expression = replaceString(expression, word, to_string(symTable->getRelocationInstrInfo(word, true)));
					}
					else
					{
						relTable->addAbsoulteRow(locationCounter - 4, currentSectionName, symbolRowNum);
						expression = replaceString(expression, word, to_string(symTable->getRelocationInstrInfo(word, false)));
					}
				}

			}
			else
			{
				cout << "[ERROR] Relocation not possible for symbol: " + word;
				errorOccured = true;
				return false;
			}
		}

		literalList.pop_front();
	}

	char *cstr = new char[expression.length() + 1];
	strcpy(cstr, expression.c_str());
	return evaluateExpr(cstr);
}