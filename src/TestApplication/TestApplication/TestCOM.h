#pragma once
//
// #include
//
#include <Windows.h>
#include <string>

//
// using
//
using std::wstring;

//
// function prototypes
//
void PostPotentialQuitMessageToMainThread();
void PrintLineToConsole(wstring text);

