// TestCOM.h : Declaration of the CTestCOM

#pragma once
#include "resource.h"       // main symbols



#include "TestCOMServer_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CTestCOM

class ATL_NO_VTABLE CTestCOM :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTestCOM, &CLSID_TestCOM>,
	public IDispatchImpl<ITestCOM, &IID_ITestCOM, &LIBID_TestCOMServerLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CTestCOM()
	{
	}

DECLARE_REGISTRY_RESOURCEID(106)

DECLARE_NOT_AGGREGATABLE(CTestCOM)

BEGIN_COM_MAP(CTestCOM)
	COM_INTERFACE_ENTRY(ITestCOM)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

	//
	// ITestCOM
	//
	STDMETHOD(GetMessage)(/*[out, retval]*/ BSTR* message);

};

OBJECT_ENTRY_AUTO(__uuidof(TestCOM), CTestCOM)
