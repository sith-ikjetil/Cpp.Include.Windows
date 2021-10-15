// TestCOM.cpp : Implementation of CTestCOM

#include "pch.h"
#include "TestCOM.h"


// CTestCOM
STDMETHODIMP CTestCOM::GetMessage(/*[out, retval]*/ BSTR* message)
{
	CComBSTR bstr(L"Message from TestCOM.GetMessage!");
	*message = bstr.Detach();
	return S_OK;
}

