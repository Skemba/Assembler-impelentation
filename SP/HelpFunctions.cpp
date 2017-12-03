#include "HelpFunctions.h"

bool has_suffix(const std::string &str, const std::string &suffix)
{
	return str.size() >= suffix.size() &&
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool has_prefix(const std::string &str, const std::string &prefix)
{
	return str.compare(0, prefix.size(), prefix) == 0;
}

bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

bool isRegDir(std::string op)
{
	std::string regexString = "(^R([0-9]|1[0-5])$)|^SP$|^PC$";
	std::regex r(regexString);
	std::smatch match;
	if (std::regex_search(op, match, r))
	{
		return true;
	}

	return false;
}
bool isRegInd(std::string op)
{
	// std::string regexString = "(^R([0-9]|1[0-5])$)|^SP$|^PC$|(^\[(R([0-9]|1[0-5])|SP|PC)\]$)";
	std::string regexString = "^(\\[(R([0-9]|1[0-5])|SP|PC)\\])$";
	std::regex r(regexString);
	std::smatch match;
	if (std::regex_search(op, match, r))
	{
		return true;
	}

	return false;
}
bool isRegIndPom(std::string op)
{
	// std::string regexString = "(^R([0-9]|1[0-5])$)|^SP$|^PC$|(^\[(R([0-9]|1[0-5])|SP|PC)\]$)";
	op.erase(remove_if(op.begin(), op.end(), ::isspace), op.end());
	std::string regexString = "^(\\[(R([0-9]|1[0-5])|SP|PC)[ \\t]*(\\+|\\-)..*\\])$";
	std::regex r(regexString);
	std::smatch match;
	if (std::regex_search(op, match, r))
	{
		return true;
	}

	return false;
}

bool isImm(std::string op)
{
	std::string regexString = "^#\\S\\S*$";
	std::regex r(regexString);
	std::smatch match;
	if (std::regex_search(op, match, r))
	{
		return true;
	}

	return false;
}

std::string threeBitToString(unsigned int hexValue)
{
	std::stringstream ss;
	ss << std::hex << hexValue;
	unsigned n;
	ss >> n;
	std::bitset<3> b(n);
	return b.to_string();
}

std::string fiveBitToString(unsigned int hexValue)
{
	std::stringstream ss;
	ss << std::hex << hexValue;
	unsigned n;
	ss >> n;
	std::bitset<5> b(n);
	return b.to_string();
}

std::string hexByteToString(unsigned int hexValue)
{
	std::stringstream ss;
	ss << std::hex << hexValue;
	unsigned n;
	ss >> n;
	std::bitset<8> b(n);
	return b.to_string();
}

std::string hex2BytesToString(int hexValue)
{
	std::stringstream ss;
	ss << std::hex << hexValue;
	unsigned n;
	ss >> n;
	std::bitset<16> b(n);
	return b.to_string();
}

std::string hex4BytesToString(int hexValue)
{
	std::stringstream ss;
	ss << std::hex << hexValue;
	unsigned n;
	ss >> n;
	std::bitset<32> b(n);
	return b.to_string();
}

std::string bits32ToHex(std::string bits, bool parity)
{
	if (bits.find(" ") != std::string::npos) {
		return bits;
	}
	std::bitset<32> b(bits);

	std::stringstream res;
	res << std::hex << b.to_ulong();
	std::string resString = res.str();

	while (resString.size() < 8)
	{
		resString.insert(0, "0");
	}

	if(parity) 
	{
		resString.insert(1, " ");
		resString.insert(4, " ");
		resString.insert(7, " ");
	}
	else
	{
		resString.insert(2, " ");
		resString.insert(5, " ");
		resString.insert(8, " ");
	}

	return resString;
}

std::string bits16ToHex(std::string bits, bool parity)
{
	if (bits.find(" ") != std::string::npos) {
		return bits;
	}
	std::bitset<16> b(bits);

	std::stringstream res;
	res << std::hex << b.to_ulong();
	std::string resString = res.str();

	while (resString.size() < 4)
	{
		resString.insert(0, "0");
	}

	if (parity)
	{
		resString.insert(1, " ");
	}
	else
	{
		resString.insert(2, " ");
	}

	return resString;
}

std::string bits8ToHex(std::string bits, bool parity)
{
	if (bits.find(" ") != std::string::npos) {
		return bits;
	}
	std::bitset<8> b(bits);

	std::stringstream res;
	res << std::hex << b.to_ulong();
	std::string resString = res.str();

	while (resString.size() < 2)
	{
		resString.insert(0, "0");
	}

	return resString;
}

std::string bits32LocToHex(std::string bits)
{
	std::bitset<32> b(bits);

	std::stringstream res;
	res << std::hex << b.to_ulong();

	std::string resString = res.str();
	return resString;
}

std::string hexReverse(std::string hex)
{
	int i = 0, lli = hex.length();
	for (; i < lli; i++, lli--)
	{
		std::swap(hex[i], hex[lli - 1]);
	}

	for (i = 0; i < lli; i += 2)
	{
		std::swap(hex[i], hex[i + 1]);
		i++;
	}
	return hex;
}

std::string findRegister(std::string expr)
{
	std::string regexString = "R([0-9]|1[0-5])|SP|PC";
	std::regex r(regexString);
	std::smatch match;
	if (std::regex_search(expr, match, r))
	{
		return match[0];
	}

	return "";
}

std::string twoByTwo(std::string hexExpr)
{
	for (unsigned int i = 2; i < hexExpr.size(); i += 2)
	{
		hexExpr = hexExpr.substr(0, i) + " " + hexExpr.substr(i);
		i++;
	}
	
	return hexExpr;
}

std::string bitAddressToFileHex(std::string expr)
{
	std::string finalString = twoByTwo(hexReverse(bits32LocToHex(expr)));

	if (finalString.size() == 1) {
		finalString = "0" + finalString;
	}

	if (finalString.size() == 4 || finalString.size() == 7 || finalString.size() == 9) {
		finalString = finalString + "0";
		std::swap(finalString[finalString.size() - 1], finalString[finalString.size() - 2]);
	}

	while (finalString.size() != 11) {
		finalString += " 00";
	}

	return finalString;
}

std::string findOffset(std::string offset)
{
	size_t pos = offset.find("+");
	size_t endPos = offset.find("]");
	std::string result = offset.substr(pos + 1, endPos - 1 - pos);
	result.erase(std::remove_if(result.begin(), result.end(), ::isspace), result.end());
	return result;
}

bool checkIfExpression(std::string str)
{
	size_t plus = str.find("+");
	size_t minus = str.find("-");
	if (plus != std::string::npos || minus != std::string::npos)
	{
		return true;
	}

	return false;
}

std::list<std::string> splitWithDelimiters(std::string expr, char* del)
{
	std::list<std::string> literalList;
	std::stringstream stringStream(expr);
	std::string line;
	while (std::getline(stringStream, line))
	{
		std::size_t prev = 0, pos;
		while ((pos = line.find_first_of(del, prev)) != std::string::npos)
		{
			if (pos > prev)
				literalList.push_back(line.substr(prev, pos - prev));
			prev = pos + 1;
		}
		if (prev < line.length())
			literalList.push_back(line.substr(prev, std::string::npos));
	}

	return literalList;
}

std::string replaceString(std::string subject, const std::string& search, const std::string& replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

std::string removeBeginZeroes(std::string str)
{
	std::list<std::string> literalList = splitWithDelimiters(str, " ");
	std::string newString = "";
	while (literalList.size() > 0)
	{
		std::string word = literalList.front();
		if (word.size() == 3 && word[0] == '0')
		{
			newString = newString + word.substr(1, 2) + " ";
		}
		else if (word.size() == 4 && word[0] == '0' && word[1] == '0')
		{
			newString = newString + word.substr(2, 2) + " ";
		}
		else if (word.size() == 5 && word[0] == '0' && word[1] == '0' && word[2] == '0')
		{
			newString = newString + word.substr(3, 2) + " ";
		}
		else
		{
			newString = newString + word + " ";
		}
		literalList.pop_front();
	}

	return newString.substr(0, newString.size() - 1);
}

void replaceHexAndOct(std::string &expr)
{
	expr.erase(std::remove_if(expr.begin(), expr.end(), ::isspace), expr.end());
	std::list<std::string> literalList = splitWithDelimiters(expr, "+-*/)(");
	while(literalList.size() > 0)
	{
		std::string word = literalList.front();

		if (has_prefix(word, "0x"))
		{
			int x = std::stoul(word, nullptr, 16);
			expr = replaceString(expr, word, std::to_string(x));
		}
		else if (has_prefix(word, "0") && word.size() > 1)
		{
			int x = std::stoul(word, nullptr, 8);
			expr = replaceString(expr, word, std::to_string(x));
		}

		literalList.pop_front();
	}
}

unsigned int evaluateExpr(char* &expr)
{
	return expression(expr);
}

char get(char* &expr)
{
	char result = *expr;
	expr++;
	return result;
}

int number(char* &expr)
{
	int result = get(expr) - '0';
	while (*expr >= '0' && *expr <= '9')
	{
		result = 10 * result + get(expr) - '0';
	}
	return result;
}

int factor(char* &expr)
{
	if (*expr >= '0' && *expr <= '9')
		return number(expr);
	else if (*expr == '(')
	{
		get(expr); // '('
		int result = expression(expr);
		get(expr); // ')'
		return result;
	}
	else if (*expr == '-')
	{
		get(expr);
		return -factor(expr);
	}
	return 0; // error
}

int term(char* &expr)
{
	int result = factor(expr);
	while (*expr == '*' || *expr == '/')
		if (get(expr) == '*')
			result *= factor(expr);
		else
			result /= factor(expr);
	return result;
}

int expression(char* &expr)
{
	int result = term(expr);
	while (*expr == '+' || *expr == '-')
		if (get(expr) == '+')
			result += term(expr);
		else
			result -= term(expr);
	return result;
}
