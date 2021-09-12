///////////////////////////////////////////////////////////////////
//: Title        : TestApplication.cpp
//: Product:     : Cpp.Include.Windows
//: Date         : 2020-05-01
//: Author       : "Kjetil Kristoffer Solberg" <post@ikjetil.no>
//: Version      : 1.0.0.0
//: Descriptions : Implementation of Cpp.Include.Windows Test Application.
//
// #include
//
#include <iostream>
#include <thread>
#include "../../include/itsoftware.h"
#include "../../include/itsoftware-com.h"
#include "../../include/itsoftware-exceptions.h"
#include "../../include/itsoftware-win.h"

//
// using
//
using std::wcout;
using std::endl;
using std::thread;
using ItSoftware::Win::ItsTimer;
using ItSoftware::Win::ItsTextFile;
using ItSoftware::Win::ItsFileOpenCreation;
using ItSoftware::Win::ItsFileTextType;
using ItSoftware::Win::ItsFile;
using ItSoftware::Win::ItsEvent;
using ItSoftware::ItsTime;

//
// extern
//
extern void TestCOM1();
extern void TestCOM2();

//
// Function Prototypes
//
void TestToNumber();
void TestToString();
void TestRandom();
void TestTime();
void TestString();
void TestTimerStart();
void TestTimerStop();
void TestFileText();
void TestFileBinary();
void TestEventStart();
void TestEventStop();

//
// global variables
//
ItsTimer g_timer;
ItsEvent g_event;
wstring g_filenameText(L"D:\\ItsTextFile.txt");
wstring g_filenameBinary(L"D:\\ItsFile.bin");
thread g_eventThread;

//
// Function: main
//
int wmain(int argc, wchar_t** argv)
{
    wcout << "### Cpp.Include.Windows - Test Application ###" << endl << endl;
    
    TestEventStart();

    TestTimerStart();
    TestToNumber();
    TestToString();
    TestRandom();
    TestTime();
    TestString();
    TestCOM1();
    TestCOM2();
    TestFileText();
    TestFileBinary();
    TestTimerStop();

    TestEventStop();
    
    if (g_eventThread.joinable()) { g_eventThread.join(); }

    return EXIT_SUCCESS;
}

//
// Function: TestToNumber
//
// (i) Test numerics as string converted to primitive data types.
//
void TestToNumber()
{
    wcout << endl;
    
    wcout << L"## Test ToNumber ________________________________________________" << endl;
    wcout << L"ItsConvert::ToNumber<int>(L\"-1234\") = " << ItsConvert::ToNumber<int>(L"-1234") << endl;
    wcout << L"ItsConvert::ToNumber<unsigned int>(L\"1234\") = " << ItsConvert::ToNumber<unsigned int>(L"1234") << endl;
    wcout << L"ItsConvert::ToNumber<long>(L\"-1234\") = " << ItsConvert::ToNumber<long>(L"-1234") << endl;
    wcout << L"ItsConvert::ToNumber<unsigned long>(L\"123456789\") = " << ItsConvert::ToNumber<unsigned long>(L"12346789") << endl;
    wcout << L"ItsConvert::ToNumber<float>(L\"-12.34\") = " << ItsConvert::ToNumber<float>(L"-12.34") << endl;
    wcout << L"ItsConvert::ToNumber<double>(L\"1.234\") = " << ItsConvert::ToNumber<double>(L"1.234") << endl;
    wcout << L"ItsConvert::ToNumber<short>(L\"1234\") = " << ItsConvert::ToNumber<short>(L"1234") << endl;
    wcout << L"ItsConvert::ToNumber<unsigned short>(L\"40001\") = " << ItsConvert::ToNumber<unsigned short>(L"40001") << endl;

    wcout << endl;
}

//
// Function: TestToString
//
// (i) Tests primitive data types to string. I.e. numeric types.
//
void TestToString()
{
    wcout << endl;
    
    wcout << L"## Test ToString ________________________________________________" << endl;
    wcout << L"ItsConvert::ToString<int>(-1234) = \"" << ItsConvert::ToString<int>(-1234) << "\"" << endl;
    wcout << L"ItsConvert::ToString<unsigned int>(1234) = \"" << ItsConvert::ToString<unsigned int>(1234) << "\"" << endl;
    wcout << L"ItsConvert::ToString<long>(-1234) = \"" << ItsConvert::ToString<long>(-1234) << "\"" << endl;
    wcout << L"ItsConvert::ToString<unsigned long>(123456789) = \"" << ItsConvert::ToString<unsigned long>(12346789) << "\"" << endl;
    wcout << L"ItsConvert::ToString<float>(-12.34) = \"" << ItsConvert::ToString<float>(-12.34f) << "\"" << endl;
    wcout << L"ItsConvert::ToString<double>(1.234) = \"" << ItsConvert::ToString<double>(1.234) << "\"" << endl;
    wcout << L"ItsConvert::ToString<short>(1234) = \"" << ItsConvert::ToString<short>(1234) << "\"" << endl;
    wcout << L"ItsConvert::ToString<unsigned short>(40001) = \"" << ItsConvert::ToString<unsigned short>(40001) << "\"" << endl;
    wcout << L"ItsConvert::ToStringFormatted(256810246) = \"" << ItsConvert::ToStringFormatted(256810246) << "\"" << endl;
    wcout << L"ItsConvert::ToStringFormatted(256810246, L\" \") = \"" << ItsConvert::ToStringFormatted(256810246, L" ") << "\"" << endl;
    wcout << endl;
}

//
// Function: TestRandom
//
// (i) Test random integers and floats.
//
void TestRandom()
{
    wcout << endl;
    
    wcout << L"## Test Random _________________________________________________" << endl;
    wcout << L"ItsRandom<long>(10'000, 1'000'000) = " << ItsRandom<long>(10'000, 1'000'000) << endl;
    wcout << L"ItsRandom<long>(10'000, 1'000'000) = " << ItsRandom<long>(10'000, 1'000'000) << endl;
    wcout << L"ItsRandom<float>(1, 10) = " << ItsRandom<float>(1, 10) << endl;
    wcout << L"ItsRandom<float>(1, 10) = " << ItsRandom<float>(1, 10) << endl;
    wcout << L"ItsRandom<double>(1, 2) = " << ItsRandom<double>(1, 2) << endl;
    wcout << L"ItsRandom<double>(1, 2) = " << ItsRandom<double>(1, 2) << endl;

    wcout << endl;
}

//
// Function: TestTime
//
// (i) Test rendering of time in milliseconds.
//
void TestTime()
{
    wcout << endl;

    wcout << L"## Test Time ___________________________________________________" << endl;
    wcout << L"ItsTime::RenderMsToFullString(92481379, false)" << L" = " << ItsTime::RenderMsToFullString(92481379, false) << endl;
    wcout << L"ItsTime::RenderMsToFullString(92481379, true)" << L" = " << ItsTime::RenderMsToFullString(92481379, true) << endl;
    wcout << L"ItsDateTime::Now().ToString(\"s\")" << L" = " << ItsDateTime::Now().ToString() << endl;
    wcout << endl;
}

//
// Function: TestString
//
// (i) Test string manipulation routines.
//
void TestString()
{
    wstring test(L"Ab12Cd");
    wstring testTrim(L"  Ab12Cd  ");
    wcout << endl;

    wcout << L"## Test String ________________________________________________" << endl;
    wcout << L"test = \"" << test << "\"" << endl;
    wcout << L"testTrim = \"" << testTrim << "\"" << endl;
    wcout << L"ItsString::Left(test,4) = \"" << ItsString::Left(test, 4) << "\"" << endl;
    wcout << L"ItsString::Right(test,4) = \"" << ItsString::Right(test, 4) << "\"" << endl;
    wcout << L"ItsString::Mid(test,2,2) = \"" << ItsString::Mid(test, 2, 2) << "\"" << endl;
    wcout << L"ItsString::ToLowerCase(test) = \"" << ItsString::ToLowerCase(test) << "\"" << endl;
    wcout << L"ItsString::ToUpperCase(test) = \"" << ItsString::ToUpperCase(test) << "\"" << endl;
    wcout << L"ItsString::Trim(testTrim) = \"" << ItsString::Trim(testTrim) << "\"" << endl;

    wcout << endl;
}

//
// Function: TestTimerStart
//
// (i) Starts timer
//
void TestTimerStart()
{
    wcout << L"## Test Timer Start ________________________________________________" << endl;
    wcout << L"Timer started..." << endl;
    g_timer.Start();
}

//
// Function: TestTimerStop
//
// (i) Stops timer
//
void TestTimerStop()
{
    g_timer.Stop();

    wcout << L"## Test Timer Stop ________________________________________________" << endl;
    wcout << L"Time elapsed: " << ItsTime::RenderMsToFullString(g_timer.GetMilliseconds(), true) << endl << endl;
}

//
// Function: TestFileText
//
// (i) Tests files
//
void TestFileText()
{

    wcout << L"## Test File Text ________________________________________________" << endl;

    ItsTextFile file{};
    bool bResult = file.OpenOrCreateText(g_filenameText, L"rw", L"", ItsFileOpenCreation::CreateAlways, ItsFileTextType::UTF8NoBOM);
    if (!bResult) {
        wcout << L"ItsTextFile OpenOrCreateText failed" << endl;
        return;
    }

    bResult = file.WriteText(L"Small step for man. Large leap for mankind.");
    if (!bResult) {
        wcout << L"ItsTextFile WriteText failed" << endl << endl;
        return;
    }

    bResult = file.Close();
    if (!bResult) {
        wcout << L"ItsTextFile Close failed" << endl << endl;
        return;
    }

    wcout << L"ItsTextFile successfully written to: " << g_filenameText << endl << endl;
}

//
// Function: TestFileBinary
//
// (i) Tests files
//
void TestFileBinary()
{
    wcout << L"## Test File Binary ________________________________________________" << endl;

    ItsFile file{};
    bool bResult = file.OpenOrCreate(g_filenameBinary, L"rw", L"", ItsFileOpenCreation::CreateAlways);
    if (!bResult) {
        wcout << L"ItsFile OpenOrCreate failed" << endl;
        return;
    }

    vector<BYTE> data = {0x01, 0x02, 0x03, 0x04, 0x05 };
    DWORD dwWritten(0);
    bResult = file.Write(data.data(), static_cast<DWORD>(data.size()), &dwWritten);
    if (!bResult) {
        wcout << L"ItsFile Write failed" << endl << endl;
        return;
    }

    bResult = file.Close();
    if (!bResult) {
        wcout << L"ItsFile Close failed" << endl << endl;
        return;
    }

    wcout << L"ItsFile successfully written to: " << g_filenameBinary << endl << endl;
}

//
// Function: TestEventStart
//
// (i) Start testing event
//
void TestEventStart()
{
    g_event.Clear();

    wcout << L"## Test Event _________________________________________________" << endl;
    wcout << L"Event is cleared" << endl << endl;
    
    g_eventThread = thread([] {
        g_event.Wait(-1);
        wcout << L"## Test Event _________________________________________________" << endl;
        wcout << L"Event works as expected" << endl << endl;
        });
}

//
// Function: TestEventStop
//
//
//
void TestEventStop()
{
    g_event.Signal();

    wcout << L"## Test Event _________________________________________________" << endl;
    wcout << L"Event is Signaled" << endl << endl;
}