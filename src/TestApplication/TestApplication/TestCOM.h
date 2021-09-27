///////////////////////////////////////////////////////////////////
//: Title        : TestCOM.h
//: Product:     : Cpp.Include.Windows
//: Date         : 2020-05-01
//: Author       : "Kjetil Kristoffer Solberg" <post@ikjetil.no>
//: Version      : 1.0.0.0
//: Descriptions : Implementation of COM part of Cpp.Include.Windows Test Application.
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

