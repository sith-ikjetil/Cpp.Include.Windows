// dllmain.h : Declaration of module class.

class CTestCOMServerModule : public ATL::CAtlDllModuleT< CTestCOMServerModule >
{
public :
	DECLARE_LIBID(LIBID_TestCOMServerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_TESTCOMSERVER, "{118932a2-d9c6-42e3-8b0f-f1822958c001}")
};

extern class CTestCOMServerModule _AtlModule;
