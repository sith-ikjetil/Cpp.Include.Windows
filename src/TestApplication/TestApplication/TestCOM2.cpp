//
// #include
//
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include "../../include/itsoftware.h"
#include "../../include/itsoftware-com.h"
#include "../../include/itsoftware-win.h"
#include "../../include/itsoftware-exceptions.h"
#include "../TestCOMServer/TestCOMServer_i.h"
#include "../TestCOMServer/TestCOMServer_i.c"
#include "TestCOM.h"

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
using std::mutex;
using std::lock_guard;
using std::vector;
using std::unique_ptr;
using std::make_unique;

//
// function prototypes
//
void TestCOM2();
DWORD WINAPI COM2_THREAD1(LPVOID arg);
DWORD WINAPI COM2_THREAD2(LPVOID arg);
void COM2_THREAD3(void* arg);

//
// extern
//
extern HWND g_hWndMainThread;
extern bool g_bThread1Fin;
extern bool g_bThread2Fin;
extern bool g_bThread3Fin;

//
// main
//
void TestCOM2()
{
	//
	// initialize global common variables
	//
	g_hWndMainThread = NULL;
	g_bThread1Fin = false;
	g_bThread2Fin = false;
	g_bThread3Fin = false;

	//
	// enter apartment
	//
	ComRuntime runtime(ComApartment::ApartmentThreaded);

	wcout << endl;
	wcout << L"## Test COM 2 ItsMarshalPtr ________________________________________________" << endl;

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

	unique_handle_handle	hThread1(::CreateThread(NULL, 1024 * 1024, COM2_THREAD1, (LPVOID)&pMarshal1, 0, &dwThreadID1));
	unique_handle_handle	hThread2(::CreateThread(NULL, 1024 * 1024, COM2_THREAD2, (LPVOID)&pMarshal2, 0, &dwThreadID2));
	unique_ptr<thread>		pThread3 = make_unique<thread>(COM2_THREAD3, (void*)&pMarshal3);

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

	PrintLineToConsole(L"WM_QUIT received and exits message loop");
	PrintLineToConsole(L"");

	//
	// join thread 3
	//
	pThread3->detach();
}

//
// Thread 1 - STA
//
DWORD WINAPI COM2_THREAD1(LPVOID pArg)
{
	ComRuntime runtime(ComApartment::ApartmentThreaded);

	ItsMarshalPtr<ITestCOM, &IID_ITestCOM>* pM = static_cast<ItsMarshalPtr<ITestCOM, &IID_ITestCOM>*>(pArg);

	CComPtr<ITestCOM> pIStaObject;
	pM->UnMarshal(&pIStaObject);

	CComBSTR bstr;
	HRESULT hr = pIStaObject->GetMessage(&bstr);
	if (FAILED(hr)) {
		PrintLineToConsole( L"ERROR STA Thread 1: " + ItsError::GetErrorDescription(hr) );
		g_bThread1Fin = true;
		PostPotentialQuitMessageToMainThread();
		return 1;
	}

	wstring str(L"Message from STA Thread 1: ");
	str.append(bstr.operator LPWSTR());
	PrintLineToConsole( str );

	g_bThread1Fin = true;

	PostPotentialQuitMessageToMainThread();

	return 0;
}

//
// Thread 2 - MTA
//
DWORD WINAPI COM2_THREAD2(LPVOID pArg)
{
	ComRuntime runtime(ComApartment::MultiThreaded);

	ItsMarshalPtr<ITestCOM, &IID_ITestCOM>* pM = static_cast<ItsMarshalPtr<ITestCOM, &IID_ITestCOM>*>(pArg);

	CComPtr<ITestCOM> pIStaObject;
	pM->UnMarshal(&pIStaObject);

	CComBSTR bstr;
	HRESULT hr = pIStaObject->GetMessage(&bstr);
	if (FAILED(hr)) {
		PrintLineToConsole(L"ERROR MTA Thread 2: " + ItsError::GetErrorDescription(hr));
		g_bThread2Fin = true;
		PostPotentialQuitMessageToMainThread();
		return 1;
	}

	wstring str(L"Message from MTA Thread 2: ");
	str.append(bstr.operator LPWSTR());
	PrintLineToConsole(str);

	g_bThread2Fin = true;

	PostPotentialQuitMessageToMainThread();

	return 0;
}

//
// Thread 3 - STA
//
void COM2_THREAD3(void* pArg)
{
	ComRuntime runtime(ComApartment::ApartmentThreaded);

	ItsMarshalPtr<ITestCOM, &IID_ITestCOM>* pM = static_cast<ItsMarshalPtr<ITestCOM, &IID_ITestCOM>*>(pArg);

	CComPtr<ITestCOM> pIStaObject;
	pM->UnMarshal(&pIStaObject);

	CComBSTR bstr;
	HRESULT hr = pIStaObject->GetMessage(&bstr);
	if (FAILED(hr)) {
		PrintLineToConsole( L"ERROR STA Thread 3: " + ItsError::GetErrorDescription(hr) );
		g_bThread3Fin = true;
		PostPotentialQuitMessageToMainThread();
		return;
	}

	wstring str(L"Message from STA Thread 3: ");
	str.append(bstr.operator LPWSTR());
	PrintLineToConsole(str);

	g_bThread3Fin = true;

	PostPotentialQuitMessageToMainThread();
}
