#pragma once
#ifndef _HELPFUNCTIONS_H_
#define _HELPFUNCTIONS_H_

#include<iostream>
#include <string>
#include <regex>
#include <sstream>
#include <bitset>
#include <cctype>
#include <list>
#include <algorithm>


bool has_suffix(const std::string &str, const std::string &suffix);
bool has_prefix(const std::string &str, const std::string &prefix);
bool is_number(const std::string& s);

bool isImm(std::string op);
bool isRegDir(std::string op);
bool isRegInd(std::string op);
bool isRegIndPom(std::string op);

bool checkIfExpression(std::string str);

std::string threeBitToString(unsigned int hexValue);
std::string fiveBitToString(unsigned int hexValue);
std::string hexByteToString(unsigned int hexValue);
std::string hex2BytesToString(int hexValue);
std::string hex4BytesToString(int hexValue);
std::string bits32ToHex(std::string bits, bool parity=false);
std::string bits16ToHex(std::string bits, bool parity = false);
std::string bits8ToHex(std::string bits, bool parity = false);
std::string hexReverse(std::string hex);
std::string findRegister(std::string expr);
std::string twoByTwo(std::string hexExpr);
std::string bitAddressToFileHex(std::string expr);

std::string findOffset(std::string expr);

int expression(char* &expr);
unsigned int evaluateExpr(char* &expr);

void replaceHexAndOct(std::string &expr);
std::list<std::string> splitWithDelimiters(std::string expr, char* del);
std::string replaceString(std::string subject, const std::string& search, const std::string& replace);

std::string removeBeginZeroes(std::string str);
#endif