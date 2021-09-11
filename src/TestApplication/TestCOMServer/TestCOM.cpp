// TestCOM.cpp : Implementation of CTestCOM

#include "pch.h"
#include "TestCOM.h"


// CTestCOM
STDMETHODIMP CTestCOM::GetMessage(/*[out, retval]*/ BSTR* message)
{
	*message = CComBSTR("Message from TestCOM.GetMessage!");
	return S_OK;
}

