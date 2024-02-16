// pch.cpp: source file corresponding to the pre-compiled header
#include "pch.h"
#include "myfuncs.h"

// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.

//this function is exported in myfunc.h
int Add(int num1, int num2) {
	return num1 + num2;
}

BSTR GetText() {
	const wchar_t* text = L"this is a sample text from CPP";
	return SysAllocString(text);
}