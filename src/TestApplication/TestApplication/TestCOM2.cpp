///////////////////////////////////////////////////////////////////
//: Title        : TestCOM2.cpp
//: Product:     : Cpp.Include.Windows
//: Date         : 2020-05-01
//: Author       : "Kjetil Kristoffer Solberg" <post@ikjetil.no>
//: Version      : 1.0.0.0
//: Descriptions : Implementation of Cpp.Include.Windows.
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
#include "../../include/itsoftware-win-core.h"
#include "../../include/itsoftware-exceptions.h"
#include "../TestCOMServer/TestCOMServer_i.h"
#include "../TestCOMServer/TestCOMServer_i.c"
#include "TestCOM.h"

namespace ItSoftware::CppIncludeWindows::TestApplication
{
	//
	// using 
	// 
	using ItSoftware::COM::ItsComRuntime;
	using ItSoftware::COM::ItsComApartment;
	using ItSoftware::COM::ItsMarshalPtr;
	using ItSoftware::COM::ItsError;
	using ItSoftware::Win::Core::unique_handle_handle;
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
	extern void PrintTestHeader(wstring text);

	//
	// main
	//
	void TestCOM2()
	{
		//
		// enter apartment
		//
		ItsComRuntime runtime(ItsComApartment::ApartmentThreaded);
		{
			PrintTestHeader(L"ItsMarshalPtr");

			//
			// initialize global common variables
			//
			g_hWndMainThread = NULL;
			g_bThread1Fin = false;
			g_bThread2Fin = false;
			g_bThread3Fin = false;

			//
			// Create COM object
			//
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

			//
			// Create threads
			//
			DWORD dwThreadID1(0);
			DWORD dwThreadID2(0);

			unique_handle_handle	hThread1(::CreateThread(NULL, 1024 * 1024, COM2_THREAD1, (LPVOID)&pMarshal1, 0, &dwThreadID1));
			unique_handle_handle	hThread2(::CreateThread(NULL, 1024 * 1024, COM2_THREAD2, (LPVOID)&pMarshal2, 0, &dwThreadID2));
			thread					hThread3(COM2_THREAD3, (void*)&pMarshal3);

			MSG msg = {};
			::GetMessage(&msg, NULL, 0, 0);
			DispatchMessage(&msg);
			g_hWndMainThread = msg.hwnd;
			while (::GetMessage(&msg, NULL, 0, 0) != 0) {
				DispatchMessage(&msg);
			}

			PrintLineToConsole(L"WM_QUIT received and exits message loop");
			PrintLineToConsole(L"");

			if (hThread3.joinable()) {
				wcout << L"Calling .join()...";
				hThread3.join();
				wcout << L" COMPLETED!" << endl;
			}
		}
	}

	//
	// Thread 1 - STA
	//
	DWORD WINAPI COM2_THREAD1(LPVOID pArg)
	{
		ItsComRuntime runtime(ItsComApartment::ApartmentThreaded);
		{
			ItsMarshalPtr<ITestCOM, &IID_ITestCOM>* pM = static_cast<ItsMarshalPtr<ITestCOM, &IID_ITestCOM>*>(pArg);
			{
				CComPtr<ITestCOM> pIStaObject;
				HRESULT hr = pM->UnMarshal(&pIStaObject);

				CComBSTR bstr;
				hr = pIStaObject->GetMessage(&bstr);
				if (FAILED(hr)) {
					PrintLineToConsole(L"ERROR STA Thread 1: " + ItsError::GetErrorDescription(hr));
					g_bThread1Fin = true;
					PostPotentialQuitMessageToMainThread();
					return 1;
				}

				wstring str(L"Message from STA Thread 1: ");
				wstring str2 = str.append(bstr.operator LPWSTR());
				PrintLineToConsole(str2);
			}

			g_bThread1Fin = true;

			PostPotentialQuitMessageToMainThread();
		}
		return 0;
	}

	//
	// Thread 2 - MTA
	//
	DWORD WINAPI COM2_THREAD2(LPVOID pArg)
	{
		ItsComRuntime runtime(ItsComApartment::MultiThreaded);
		{
			ItsMarshalPtr<ITestCOM, &IID_ITestCOM>* pM = static_cast<ItsMarshalPtr<ITestCOM, &IID_ITestCOM>*>(pArg);
			{
				CComPtr<ITestCOM> pIStaObject;
				HRESULT hr = pM->UnMarshal(&pIStaObject);

				CComBSTR bstr;
				hr = pIStaObject->GetMessage(&bstr);
				if (FAILED(hr)) {
					PrintLineToConsole(L"ERROR MTA Thread 2: " + ItsError::GetErrorDescription(hr));
					g_bThread2Fin = true;
					PostPotentialQuitMessageToMainThread();
					return 1;
				}

				wstring str(L"Message from MTA Thread 2: ");
				wstring str2 = str.append(bstr.operator LPWSTR());
				PrintLineToConsole(str2);
			}

			g_bThread2Fin = true;

			PostPotentialQuitMessageToMainThread();
		}
		return 0;
	}

	//
	// Thread 3 - STA
	//
	void COM2_THREAD3(void* pArg)
	{
		ItsComRuntime runtime(ItsComApartment::ApartmentThreaded);
		{
			ItsMarshalPtr<ITestCOM, &IID_ITestCOM>* pM = static_cast<ItsMarshalPtr<ITestCOM, &IID_ITestCOM>*>(pArg);
			{
				CComPtr<ITestCOM> pIStaObject;
				HRESULT hr = pM->UnMarshal(&pIStaObject);

				CComBSTR bstr;
				hr = pIStaObject->GetMessage(&bstr);
				if (FAILED(hr)) {
					PrintLineToConsole(L"ERROR STA Thread 3: " + ItsError::GetErrorDescription(hr));
					g_bThread3Fin = true;
					PostPotentialQuitMessageToMainThread();
					return;
				}

				wstring str(L"Message from STA Thread 3: ");
				wstring str2 = str.append(bstr.operator LPWSTR());

				PrintLineToConsole(str2);
			}

			g_bThread3Fin = true;

			PostPotentialQuitMessageToMainThread();
		}
	}
}