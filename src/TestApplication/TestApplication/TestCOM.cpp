///////////////////////////////////////////////////////////////////
//: Title        : TestCOM.cpp
//: Product:     : Cpp.Include.Windows
//: Date         : 2020-05-01
//: Author       : "Kjetil Kristoffer Solberg" <post@ikjetil.no>
//: Version      : 1.0.0.0
//: Descriptions : Implementation of TestCOM.h.
//
// #include
//
#include <mutex>
#include <iostream>
#include "TestCOM.h"

//
// using 
//
using std::mutex;
using std::lock_guard;
using std::wcout;
using std::endl;

//
// global variables
//
HWND g_hWndMainThread = NULL;
bool g_bThread1Fin = false;
bool g_bThread2Fin = false;
bool g_bThread3Fin = false;

//
// PostPotentialQuitMessageToMainThread
//
void PostPotentialQuitMessageToMainThread()
{
	static mutex m;
	lock_guard<mutex> g(m);

	if (g_bThread1Fin && g_bThread2Fin && g_bThread3Fin) {
		PostMessage(g_hWndMainThread, WM_QUIT, 0, 0);
		return;
	}
}

//
// PrintLineToConsole
//
void PrintLineToConsole(wstring text)
{
	static mutex m;
	lock_guard<mutex> guard(m);

	wcout << text << endl;
}