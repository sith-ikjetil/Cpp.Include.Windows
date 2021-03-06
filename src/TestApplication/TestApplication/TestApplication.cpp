//
// #include
//
#include <iostream>
#include "../../include/itsoftware.h"
//#include "../../include/itsoftware-cli.h" // C++/CLI mode only
#include "../../include/itsoftware-com.h"
#include "../../include/itsoftware-exceptions.h"
#include "../../include/itsoftware-win.h"

//
// using
//
using std::wcout;
using std::endl;

//
// Function Prototypes
//
void TestToNumber();
void TestToString();
void TestRandom();
void TestTime();
void TestString();
// TestTimer
// TestBase64

//
// Function: main
//
int main()
{
    wcout << "### Cpp.Include.Windows - Test Application ###" << endl << endl;
    
    TestToNumber();
    TestToString();
    TestRandom();
    TestTime();
    TestString();

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