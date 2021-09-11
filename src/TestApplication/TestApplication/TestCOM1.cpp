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
using ItSoftware::COM::ItsMarshalGITPtr;
using ItSoftware::Win::ItsError;
using ItSoftware::Win::unique_handle_handle;
using std::wcout;
using std::endl;
using std::thread;

//
// function prototypes
//
void TestCOM();
DWORD WINAPI THREAD1(LPVOID arg);
DWORD WINAPI THREAD2(LPVOID arg);
void MyThread3(void* arg);
void PostPotentialQuitMessageToMainThread();

//
// global variables
//
HWND g_hWndMainThread = nullptr;
bool g_bThread1Fin = false;
bool g_bThread2Fin = false;
bool g_bThread3Fin = false;
CRITICAL_SECTION g_cs;

//
// main
//
void TestCOM1()
{
	ComRuntime runtime(ComApartment::ApartmentThreaded);

	wcout << L"## Test COM 1 ItsMarshalGITPtr ________________________________________________" << endl;

	InitializeCriticalSection(&g_cs);

	CComPtr<ITestCOM> pIStaObject;
	HRESULT hr = pIStaObject.CoCreateInstance(CLSID_TestCOM);
	if (FAILED(hr)) {
		wcout << ItsError::GetErrorDescription(hr) << endl;
		return;
	}

	ItsMarshalGITPtr<ITestCOM> pMarshal1;
	pMarshal1.Marshal(IID_ITestCOM, pIStaObject);
	wcout << L"Cookie 1: " << pMarshal1.GetCookie() << endl;

	ItsMarshalGITPtr<ITestCOM> pMarshal2;
	pMarshal2.Marshal(IID_ITestCOM, pIStaObject);
	wcout << L"Cookie 2: " << pMarshal2.GetCookie() << endl;

	ItsMarshalGITPtr<ITestCOM> pMarshal3;
	pMarshal3.Marshal(IID_ITestCOM, pIStaObject);
	wcout << L"Cookie 3: " << pMarshal3.GetCookie() << endl;

	DWORD dwThreadID1(0);
	DWORD dwThreadID2(0);

	unique_handle_handle	hThread1(::CreateThread(NULL, 1024 * 1024, THREAD1, (LPVOID)&pMarshal1, 0, &dwThreadID1));
	unique_handle_handle	hThread2(::CreateThread(NULL, 1024 * 1024, THREAD2, (LPVOID)&pMarshal2, 0, &dwThreadID2));
	unique_ptr<thread>		pThread3 = make_unique<thread>(MyThread3, (void*)&pMarshal3);

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
	g_hWndMainThread = msg.hwnd;

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
DWORD WINAPI THREAD1(LPVOID pArg)
{
	ComRuntime runtime(ComApartment::ApartmentThreaded);

	ItsMarshalGITPtr<ITestCOM>* pM = static_cast<ItsMarshalGITPtr<ITestCOM>*>(pArg);

	CComPtr<ITestCOM> pIStaObject;
	pM->UnMarshal(&pIStaObject);

	CComBSTR bstr;
	HRESULT hr = pIStaObject->GetMessage(&bstr);
	if (FAILED(hr)) {
		wcout << L"ERROR STA Thread 1: " << ItsError::GetErrorDescription(hr) << endl;
		g_bThread1Fin = true;
		PostPotentialQuitMessageToMainThread();
		return 1;
	}

	wstring str(L"Message from STA Thread 1: ");
	str.append(bstr.operator LPWSTR());
	wcout << str << endl;

	g_bThread1Fin = true;

	PostPotentialQuitMessageToMainThread();

	return 0;
}

//
// Thread 2 - MTA
//
DWORD WINAPI THREAD2(LPVOID pArg)
{
	ComRuntime runtime(ComApartment::MultiThreaded);

	ItsMarshalGITPtr<ITestCOM>* pM = static_cast<ItsMarshalGITPtr<ITestCOM>*>(pArg);

	CComPtr<ITestCOM> pIStaObject;
	pM->UnMarshal(&pIStaObject);

	CComBSTR bstr;
	HRESULT hr = pIStaObject->GetMessage(&bstr);
	if (FAILED(hr)) {
		wcout << L"ERROR MTA Thread 2: " << ItsError::GetErrorDescription(hr) << endl;
		g_bThread2Fin = true;
		PostPotentialQuitMessageToMainThread();
		return 1;
	}

	wstring str(L"Message from MTA Thread 2: ");
	str.append(bstr.operator LPWSTR());
	wcout << str << endl;

	g_bThread2Fin = true;

	PostPotentialQuitMessageToMainThread();

	return 0;
}

//
// Thread 3 - STA
//
void MyThread3(void* pArg)
{
	ComRuntime runtime(ComApartment::ApartmentThreaded);

	ItsMarshalGITPtr<ITestCOM>* pM = static_cast<ItsMarshalGITPtr<ITestCOM>*>(pArg);

	CComPtr<ITestCOM> pIStaObject;
	pM->UnMarshal(&pIStaObject);

	CComBSTR bstr;
	HRESULT hr = pIStaObject->GetMessage(&bstr);
	if (FAILED(hr)) {
		wcout << L"ERROR STA Thread 3: " << ItsError::GetErrorDescription(hr) << endl;
		g_bThread3Fin = true;
		PostPotentialQuitMessageToMainThread();
		return;
	}

	wstring str(L"Message from STA Thread 3: ");
	str.append(bstr.operator LPWSTR());
	wcout << str << endl;

	g_bThread3Fin = true;

	PostPotentialQuitMessageToMainThread();
}

//
// PostPotentialQuitMessageToMainThread
//
void PostPotentialQuitMessageToMainThread()
{
	EnterCriticalSection(&g_cs);

	if (g_bThread1Fin && g_bThread2Fin && g_bThread3Fin) {
		PostMessage(g_hWndMainThread, WM_QUIT, 0, 0);
		LeaveCriticalSection(&g_cs);
		DeleteCriticalSection(&g_cs);
		return;
	}

	LeaveCriticalSection(&g_cs);
}