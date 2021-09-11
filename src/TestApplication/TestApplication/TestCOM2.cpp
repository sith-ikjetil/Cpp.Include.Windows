//
// #include
//
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include "../../include/itsoftware.h"
#include "../../include/itsoftware-com.h"
#include "../../include/itsoftware-win.h"
#include "../../include/itsoftware-exceptions.h"
#include "../TestCOMServer/TestCOMServer_i.h"
#include "../TestCOMServer/TestCOMServer_i.c"

//
// using 
// 
using ItSoftware::COM::ComRuntime;
using ItSoftware::COM::ComApartment;
using ItSoftware::COM::ItsMarshalPtr;
using ItSoftware::Win::ItsError;
using ItSoftware::Win::unique_handle_handle;
using std::wcout;
using std::endl;
using std::thread;

//
// function prototypes
//
void TestCOM2();
DWORD WINAPI THREAD1_2(LPVOID arg);
DWORD WINAPI THREAD2_2(LPVOID arg);
void MyThread3_2(void* arg);
void PostPotentialQuitMessageToMainThread_2();

//
// global variables
//
HWND g_hWndMainThread2 = nullptr;
bool g_bThread1Fin2 = false;
bool g_bThread2Fin2 = false;
bool g_bThread3Fin2 = false;
CRITICAL_SECTION g_cs2;

//
// main
//
void TestCOM2()
{
	ComRuntime runtime(ComApartment::ApartmentThreaded);

	wcout << L"## Test COM 2 ItsMarshalPtr ________________________________________________" << endl;

	InitializeCriticalSection(&g_cs2);

	CComPtr<ITestCOM> pIStaObject;
	HRESULT hr = pIStaObject.CoCreateInstance(CLSID_TestCOM);
	if (FAILED(hr)) {
		wcout << ItsError::GetErrorDescription(hr) << endl;
		return;
	}

	ItsMarshalPtr<ITestCOM, &IID_ITestCOM> pMarshal1;
	pMarshal1.Marshal(pIStaObject);

	ItsMarshalPtr<ITestCOM, &IID_ITestCOM> pMarshal2;
	pMarshal2.Marshal(pIStaObject);

	ItsMarshalPtr<ITestCOM, &IID_ITestCOM> pMarshal3;
	pMarshal3.Marshal(pIStaObject);

	DWORD dwThreadID1(0);
	DWORD dwThreadID2(0);

	unique_handle_handle	hThread1(::CreateThread(NULL, 1024 * 1024, THREAD1_2, (LPVOID)&pMarshal1, 0, &dwThreadID1));
	unique_handle_handle	hThread2(::CreateThread(NULL, 1024 * 1024, THREAD2_2, (LPVOID)&pMarshal2, 0, &dwThreadID2));
	unique_ptr<thread>		pThread3 = make_unique<thread>(MyThread3_2, (void*)&pMarshal3);

	//
	// message loop msg object
	//
	MSG msg = {};

	//
	// get hwnd for current thread
	//
	::GetMessage(&msg, NULL, 0, 0);
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	g_hWndMainThread2 = msg.hwnd;

	//
	// process calls to object created on this thread from other threads in other apartments
	//
	while (::GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	wcout << L"WM_QUIT received and exits message loop" << endl << endl;

	//
	// join thread 3
	//
	pThread3->detach();
}

//
// Thread 1 - STA
//
DWORD WINAPI THREAD1_2(LPVOID pArg)
{
	ComRuntime runtime(ComApartment::ApartmentThreaded);

	ItsMarshalPtr<ITestCOM, &IID_ITestCOM>* pM = static_cast<ItsMarshalPtr<ITestCOM, &IID_ITestCOM>*>(pArg);

	CComPtr<ITestCOM> pIStaObject;
	pM->UnMarshal(&pIStaObject);

	CComBSTR bstr;
	HRESULT hr = pIStaObject->GetMessage(&bstr);
	if (FAILED(hr)) {
		wcout << L"ERROR STA Thread 1: " << ItsError::GetErrorDescription(hr) << endl;
		g_bThread1Fin2 = true;
		PostPotentialQuitMessageToMainThread_2();
		return 1;
	}

	wstring str(L"Message from STA Thread 1: ");
	str.append(bstr.operator LPWSTR());
	wcout << str << endl;

	g_bThread1Fin2 = true;

	PostPotentialQuitMessageToMainThread_2();

	return 0;
}

//
// Thread 2 - MTA
//
DWORD WINAPI THREAD2_2(LPVOID pArg)
{
	ComRuntime runtime(ComApartment::MultiThreaded);

	ItsMarshalPtr<ITestCOM, &IID_ITestCOM>* pM = static_cast<ItsMarshalPtr<ITestCOM, &IID_ITestCOM>*>(pArg);

	CComPtr<ITestCOM> pIStaObject;
	pM->UnMarshal(&pIStaObject);

	CComBSTR bstr;
	HRESULT hr = pIStaObject->GetMessage(&bstr);
	if (FAILED(hr)) {
		wcout << L"ERROR MTA Thread 2: " << ItsError::GetErrorDescription(hr) << endl;
		g_bThread2Fin2 = true;
		PostPotentialQuitMessageToMainThread_2();
		return 1;
	}

	wstring str(L"Message from MTA Thread 2: ");
	str.append(bstr.operator LPWSTR());
	wcout << str << endl;

	g_bThread2Fin2 = true;

	PostPotentialQuitMessageToMainThread_2();

	return 0;
}

//
// Thread 3 - STA
//
void MyThread3_2(void* pArg)
{
	ComRuntime runtime(ComApartment::ApartmentThreaded);

	ItsMarshalPtr<ITestCOM, &IID_ITestCOM>* pM = static_cast<ItsMarshalPtr<ITestCOM, &IID_ITestCOM>*>(pArg);

	CComPtr<ITestCOM> pIStaObject;
	pM->UnMarshal(&pIStaObject);

	CComBSTR bstr;
	HRESULT hr = pIStaObject->GetMessage(&bstr);
	if (FAILED(hr)) {
		wcout << L"ERROR STA Thread 3: " << ItsError::GetErrorDescription(hr) << endl;
		g_bThread3Fin2 = true;
		PostPotentialQuitMessageToMainThread_2();
		return;
	}

	wstring str(L"Message from STA Thread 3: ");
	str.append(bstr.operator LPWSTR());
	wcout << str << endl;

	g_bThread3Fin2 = true;

	PostPotentialQuitMessageToMainThread_2();
}

//
// PostPotentialQuitMessageToMainThread
//
void PostPotentialQuitMessageToMainThread_2()
{
	EnterCriticalSection(&g_cs2);

	if (g_bThread1Fin2 && g_bThread2Fin2 && g_bThread3Fin2) {
		PostMessage(g_hWndMainThread2, WM_QUIT, 0, 0);
		LeaveCriticalSection(&g_cs2);
		DeleteCriticalSection(&g_cs2);
		return;
	}

	LeaveCriticalSection(&g_cs2);
}