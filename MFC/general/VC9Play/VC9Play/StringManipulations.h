#pragma once
#include "windows.h"
#include "iostream"
using System::String;

ref class StringManipulations
{
public:

	StringManipulations(void)
	{
	}

	static String^ AsciiToChar(int val)
	{
		char value[2];
		value[0] = val;
		value[1] = '\0';
		String^ returnString = gcnew String(value);
		return returnString;
	}

	static char * StringToCharArray(String^ val)
	{
		char * returnVal = new char[val->Length];
		strcpy(returnVal,"");
		for(int i=0;i<val->Length;i++)
			returnVal[i] = val[i];
		return returnVal;
	}
};
