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
#include <vector>
#include <string>
#include <limits>
#include "../../include/itsoftware.h"
#include "../../include/itsoftware-com.h"
#include "../../include/itsoftware-exceptions.h"
#include "../../include/itsoftware-win.h"

//
// using
//
using std::wcout;
using std::endl;
using std::ends;
using std::thread;
using std::vector;
using std::wstring;
using std::wstringstream;
using std::unique_ptr;
using std::make_unique;
using ItSoftware::Win::ItsTimer;
using ItSoftware::Win::ItsTextFile;
using ItSoftware::Win::ItsFileOpenCreation;
using ItSoftware::Win::ItsFileTextType;
using ItSoftware::Win::ItsFile;
using ItSoftware::Win::ItsEvent;
using ItSoftware::Win::ItsPath;
using ItSoftware::Win::ItsDirectory;
using ItSoftware::Win::ItsGuid;
using ItSoftware::Win::ItsError;
using ItSoftware::Win::ItsGuidFormat;
using ItSoftware::ItsTime;
using ItSoftware::ItsConvert;
using ItSoftware::ItsString;
using ItSoftware::ItsExpandDirection;
using ItSoftware::ItsRandom;
using ItSoftware::ItsDateTime;
using ItSoftware::ItsLog;
using ItSoftware::ItsLogType;
using ItSoftware::ItsDataSizeStringType;
using ItSoftware::ItsDateTime;
using ItSoftware::ItsID;
using ItSoftware::ItsCreateIDOptions;

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
void TestPath();
void TestDirectory();
void TestGuid();
void TestLog();
void TestDateTime();
void TestID();
void ExitFn();
void PrintTestHeader(wstring txt);

//
// global variables
//
ItsTimer g_timer;
ItsEvent g_event;
wstring g_filenameText(L"D:\\ItsTextFile.txt");
wstring g_filenameBinary(L"D:\\ItsFile.bin");
thread g_eventThread;
wstring g_path1(L"C:\\");
wstring g_path2(L"Temp\\test.html");
wstring g_validPath(L"C:\\Temp\\test.html");
wstring g_invalidPath(L"C:\\?Temp\\test:\a.html");
wstring g_directoryRoot(L"C:\\Temp");
wstring g_creatDir(L"C:\\Temp\\CREATDIR");

//
// Function: ExitFn
//
// (i): Print exit message.
// 
void ExitFn()
{
    wcout << endl;
    wcout << L"> Test Application - Exited <" << endl;
}

// 
// Function: wmain
//
int wmain(int argc, wchar_t* argv[])
{
    atexit(ExitFn);

    wcout << L"> Test Application - Started <" << endl;

    TestTimerStart();
    TestEventStart();
    TestToNumber();
    TestToString();
    TestRandom();
    TestTime();
    TestString();
    TestCOM1();
    TestCOM2();
    TestFileText();
    TestFileBinary();
    TestPath();
    TestDirectory();
    TestGuid();
    TestLog();
    TestDateTime();
    TestID();
    TestEventStop();
    
    if (g_eventThread.joinable()) { g_eventThread.join(); }
    TestTimerStop();
    
    return EXIT_SUCCESS;
}

//
// Function: PrintTestHeader
//
// (i): Prints a tests header.
//
void PrintTestHeader(wstring txt)
{
    wcout << endl;
    wcout << ItsString::WidthExpand(txt, 80, '_', ItsExpandDirection::Right) << endl;
}

//
// Function: TestToNumber
//
// (i): Test numerics as string converted to primitive data types.
//
void TestToNumber()
{
    PrintTestHeader(L"## Test ItsConvert::ToNumber ");

    wcout << LR"(ItsConvert::ToNumber<int>(L"-1234") = )" << ItsConvert::ToNumber<int>(L"-1234") << endl;
    wcout << LR"(ItsConvert::ToNumber<unsigned int>(L"1234") = )" << ItsConvert::ToNumber<unsigned int>(L"1234") << endl;
    wcout << LR"(ItsConvert::ToNumber<long>(L"-1234") = )" << ItsConvert::ToNumber<long>(L"-1234") << endl;
    wcout << LR"(ItsConvert::ToNumber<unsigned long>(L"123456789") = )" << ItsConvert::ToNumber<unsigned long>(L"12346789") << endl;
    wcout << LR"(ItsConvert::ToNumber<long long>(L"12345678900") = )" << ItsConvert::ToNumber<long long>(L"12345678900") << endl;
    wcout << LR"(ItsConvert::ToNumber<float>(L"-12.34") = )" << ItsConvert::ToNumber<float>(L"-12.34") << endl;
    wcout << LR"(ItsConvert::ToNumber<double>(L"1.234") = )" << ItsConvert::ToNumber<double>(L"1.234") << endl;
    wcout << LR"(ItsConvert::ToNumber<short>(L"1234") = )" << ItsConvert::ToNumber<short>(L"1234") << endl;
    wcout << LR"(ItsConvert::ToNumber<unsigned short>(L"40001") = )" << ItsConvert::ToNumber<unsigned short>(L"40001") << endl;

    wcout << endl;
}

//
// Function: TestToString
//
// (i): Tests primitive data types to string. I.e. numeric types.
//
void TestToString()
{
    PrintTestHeader(L"## Test ItsConvert::ToString ");

    wcout << LR"(ItsConvert::ToString<int>(-1234) = ")" << ItsConvert::ToString<int>(-1234) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<unsigned int>(1234) = ")" << ItsConvert::ToString<unsigned int>(1234) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<long>(-1234) = ")" << ItsConvert::ToString<long>(-1234) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<unsigned long>(123456789) = ")" << ItsConvert::ToString<unsigned long>(12346789) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<long long>(12345678900) = ")" << ItsConvert::ToString<long long>(12345678900) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<float>(-12.34) = ")" << ItsConvert::ToString<float>(-12.34f) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<double>(1.234) = ")" << ItsConvert::ToString<double>(1.234) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<short>(1234) = ")" << ItsConvert::ToString<short>(1234) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<unsigned short>(40001) = ")" << ItsConvert::ToString<unsigned short>(40001) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToStringFormatted(256810246) = ")" << ItsConvert::ToStringFormatted(256810246) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToStringFormatted(256810246, L" " = ")" << ItsConvert::ToStringFormatted(256810246, L" ") << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToDataSizeString(1024, 2) = ")" << ItsConvert::ToDataSizeString(1024, 2) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToDataSizeString(200100400, 0) = ")" << ItsConvert::ToDataSizeString(200100400, 0) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToDataSizeString(200100400, 2) = ")" << ItsConvert::ToDataSizeString(200100400, 2) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToDataSizeString(9807804606, 3) = ")" << ItsConvert::ToDataSizeString(9807804606, 3, ItsDataSizeStringType::IEC) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToDataSizeString(1000, 3) = ")" << ItsConvert::ToDataSizeString(1000, 3, ItsDataSizeStringType::IEC) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToDataSizeString(size_t max, 2) = ")" << ItsConvert::ToDataSizeString(SIZE_MAX, 2) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToDataSizeString(size_t max, 2) = ")" << ItsConvert::ToDataSizeString(SIZE_MAX, 2, ItsDataSizeStringType::IEC) << LR"(")" << endl;
    
    wcout << endl;
}

//
// Function: TestRandom
//
// (i): Test random integers and floats.
//
void TestRandom()
{
    PrintTestHeader(L"## Test ItsRandom ");

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
// (i): Test rendering of time in milliseconds.
//
void TestTime()
{
    PrintTestHeader(L"## Test ItsTime ");

    wcout << L"ItsTime::RenderMsToFullString(92481379, false)" << L" = " << ItsTime::RenderMsToFullString(92481379, false) << endl;
    wcout << L"ItsTime::RenderMsToFullString(92481379, true)" << L" = " << ItsTime::RenderMsToFullString(92481379, true) << endl;
    wcout << L"ItsDateTime::Now().ToString(\"s\")" << L" = " << ItsDateTime::Now().ToString() << endl;
    
    wcout << endl;
}

//
// Function: TestString
//
// (i): Test string manipulation routines.
//
void TestString()
{
    PrintTestHeader(L"## Test ItsString ");

    wstring test(L"Ab12Cd");
    wstring testTrim(L"  Ab12Cd  ");
    wstring testReplace(L"__ ABCDEF __");
    wstring testSplit(L"A;BC;DEF;G");
    wcout << LR"(test = ")" << test << LR"(")" << endl;
    wcout << LR"(testTrim = ")" << testTrim << LR"(")" << endl;
    wcout << LR"(testReplace = ")" << testReplace << LR"(")" << endl;
    wcout << LR"(testSplit = ")" << testSplit << LR"(")" << endl;
    wcout << LR"(ItsString::Left(test,4) = ")" << ItsString::Left(test, 4) << LR"(")" << endl;
    wcout << LR"(ItsString::Right(test,4) = ")" << ItsString::Right(test, 4) << LR"(")" << endl;
    wcout << LR"(ItsString::Mid(test,2,2) = ")" << ItsString::Mid(test, 2, 2) << LR"(")" << endl;
    wcout << LR"(ItsString::ToLowerCase(test) = ")" << ItsString::ToLowerCase(test) << LR"(")" << endl;
    wcout << LR"(ItsString::ToUpperCase(test) = ")" << ItsString::ToUpperCase(test) << LR"(")" << endl;
    wcout << LR"(ItsString::Trim(testTrim) = ")" << ItsString::Trim(testTrim) << LR"(")" << endl;
    wcout << LR"(ItsString::Replace(testReplace) = ")" << ItsString::Replace(testReplace, L"__", L"##") << endl;
   
    auto vs = ItsString::Split(testSplit, L";");
    wstringstream ss;
    for (auto s : vs)
    {
        ss << s << L" ";
    }
    ss << ends;
    wcout << LR"(ItsString::Split(testSplit,L";") = )" << ss.str() << endl;

    wstring name(L"Kjetil");
    wcout << L"ItsString::WidthExpand (ItsExpandDirection:Left,Middle,Right) Below" << endl;
    wcout << ItsString::WidthExpand(name, 30, '_', ItsExpandDirection::Left) << endl;
    wcout << ItsString::WidthExpand(name, 30, '_', ItsExpandDirection::Middle) << endl;
    wcout << ItsString::WidthExpand(name, 30, '_', ItsExpandDirection::Right) << endl;
    
    wcout << endl;
}

//
// Function: TestTimerStart
//
// (i): Starts timer
//
void TestTimerStart()
{
    PrintTestHeader(L"## Test ItsTimer::Start ");

    g_timer.Start();
    wcout << L"Timer started..." << endl;
    
    wcout << endl;
}

//
// Function: TestTimerStop
//
// (i): Stops timer
//
void TestTimerStop()
{
    PrintTestHeader(L"## Test ItsTimer::Stop ");

    g_timer.Stop();

    wcout << L"Time elapsed: " << ItsTime::RenderMsToFullString(g_timer.GetMilliseconds(), true) << endl;
    wcout << endl;
}

//
// Function: TestFileText
//
// (i): Tests files
//
void TestFileText()
{
    PrintTestHeader(L"## Test ItsTextFile ");

    ItsTextFile file{};
    bool bResult = file.OpenOrCreateText(g_filenameText, L"rw", L"", ItsFileOpenCreation::CreateAlways, ItsFileTextType::UTF8NoBOM);
    if (!bResult) {
        wcout << L"ItsTextFile OpenOrCreateText failed" << endl;
        return;
    }

    bResult = file.WriteText(L"Small step for man." + ItsTextFile::LineDelimiterUnix() + L"Large leap for mankind.");
    if (!bResult) {
        wcout << L"ItsTextFile WriteText failed" << endl << endl;
        return;
    }

    bResult = file.Close();
    if (!bResult) {
        wcout << L"ItsTextFile Close failed" << endl << endl;
        return;
    }

    wcout << L"ItsTextFile successfully written to: " << g_filenameText << endl;
    wcout << endl;
}

//
// Function: TestFileBinary
//
// (i): Tests files
//
void TestFileBinary()
{
    PrintTestHeader(L"## Test ItsFile ");

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

    wcout << L"ItsFile successfully written to: " << g_filenameBinary << endl;
    wcout << endl;
}

//
// Function: TestEventStart
//
// (i): Start testing event
//
void TestEventStart()
{
    g_event.Clear();

    PrintTestHeader(L"## Test ItsEvent Start "); 
    wcout << L"Event is cleared" << endl << endl;
    
    g_eventThread = thread([] {
        g_event.Wait(-1);
        PrintTestHeader(L"## Test ItsEvent Completed ");
        wcout << L"Event works as expected" << endl << endl;
        });
}

//
// Function: TestEventStop
//
// (i): Stop testing event.
//
void TestEventStop()
{
    g_event.Signal();

    PrintTestHeader(L"## Test ItsEvent Stop ");
    wcout << L"Event is Signaled" << endl;
    
    wcout << endl;
}

//
// Function: TestPath
//
// (i): Test ItsPath
//
void TestPath()
{
    PrintTestHeader(L"## Test ItsPath ");

    wstring path = ItsPath::Combine(g_path1, g_path2);
    if (ItsPath::Exists(path)) {
        wcout << L"Path: " << path << L" Exists" << endl;
    }
    else {
        wcout << L"Path: " << path << L" Does not exist" << endl;
    }

    wcout << L"Volume: " << ItsPath::GetVolume(path) << endl;
    wcout << L"Directory: " << ItsPath::GetDirectory(path) << endl;
    wcout << L"Filename: " << ItsPath::GetFilename(path) << endl;
    wcout << L"Extension: " << ItsPath::GetExtension(path) << endl;
    wcout << L"Has Extension (.html): " << ((ItsPath::HasExtension(path, L".html")) ? L"true" : L"false") << endl;
    wcout << L"Has Extension (.js): " << ((ItsPath::HasExtension(path, L".js")) ? L"true" : L"false") << endl;
    wcout << L"Change Extension (.html to .js): " << ItsPath::ChangeExtension(path, L".js") << endl;
    wcout << L"Is valid path valid: " << ((ItsPath::IsPathValid(path)) ? L"true" : L"false") << endl;
    wcout << L"Is invalid path valid: " << ((ItsPath::IsPathValid(g_invalidPath)) ? L"true" : L"false") << endl;

    wcout << endl;
}

//
// Function: TestDirectory
//
// (i): Test ItsDirectory
//
void TestDirectory()
{
    PrintTestHeader(L"## Test ItsDirectory ");

    auto result = ItsDirectory::GetDirectories(g_directoryRoot);
    if (result.size() > 0) {
        wcout << L"Directory: " << g_directoryRoot << " found " << result.size() << L" sub-directories" << endl;
        for (auto r : result) {
            wcout << L"Directory Found: " << r << endl;
        }
    }

    auto result2 = ItsDirectory::GetFiles(g_directoryRoot);
    if (result2.size() > 0) {
        wcout << L"Directory: " << g_directoryRoot << " found " << result2.size() << L" files" << endl;
        for (auto r : result2) {
            wcout << L"File Found: " << r << endl;
        }
    }

    auto result3 = ItsDirectory::GetLogicalDrives();
    if (result3.size() > 0) {
        wcout << L"Drives found: " << result3.size() << endl;
        for (auto r : result3) {
            wcout << L"Drive found: " << r << endl;
        }
    }

    auto cdir = g_creatDir;
    bool bResult = ItsDirectory::CreateDirectory(cdir);
    if (!bResult) {
        wcout << L"Error creating: " << cdir << endl;
        wcout << endl;
        return;
    }

    wcout << L"Successfully created: " << cdir << endl;

    bResult = ItsDirectory::RemoveDirectory(cdir);
    if (!bResult) {
        wcout << L"Error removing: " << cdir << endl;
        wcout << endl;
        return;
    }

    wcout << L"Successfully removed: " << cdir << endl;

    wcout << endl;
}

//
// Function: TestGuid
//
// (i): Test ItsGuid
//
void TestGuid()
{
    PrintTestHeader(L"## Test ItsGuid ");

    wcout << L"Guid1: " << ItsGuid::CreateGuid() << endl;
    
    GUID guid{ 0 };
    if (ItsGuid::CreateGuid(&guid))
    {
        ItsGuidFormat fmt;
        wcout << L"Guid2 RegistryFormat: " << ItsGuid::ToString(guid,fmt.RegistryFormat) << endl;
        wcout << L"Guid2 RegistryFormatStripped: " << ItsGuid::ToString(guid, fmt.RegistryFormatStripped) << endl;
        wcout << L"Guid2 ConstFormat: " << ItsGuid::ToString(guid,fmt.ConstFormat) << endl;
        wcout << L"Guid2 CompactFormat: " << ItsGuid::ToString(guid,fmt.CompactFormat) << endl;
        wcout << L"Guid2 PrefixedCompactFormat: " << ItsGuid::ToString(guid,fmt.PrefixedCompactFormat) << endl;
    }
    else {
        wcout << L"Guid2 Error: " << ItsError::GetLastErrorDescription() << endl;
    }


    wcout << endl;
}

//
// Function: TestLog
//
// (i): Test ItsLog
//
void TestLog()
{
    PrintTestHeader(L"## Test ItsLog ");

    ItsLog log;
    log.LogInformation(L"This is an information log item");
    log.LogWarning(L"This is an warning log item");
    log.LogError(L"This is an error log item");
    log.LogOther(L"This is an other log item");
    log.LogDebug(L"This is an debug log item");
    
    wcout << L"__ ToFriendlyString __" << endl;
    wcout << log.ToFriendlyString() << endl;

    wcout << L"__ ToString __" << endl;
    wcout << log.ToString() << endl;

    wcout << endl;
}

//
// Function: TestDateTime
//
// (i): Test ItsDateTime
//
void TestDateTime()
{
    PrintTestHeader(L"## Test ItsDateTime ");

    auto now = ItsDateTime::Now();
    wcout << L"ItsDateTime.Now(): " << now.ToString() << endl;

    now.AddDays(7);
    now.AddHours(1);
    now.AddMinutes(1);
    now.AddSeconds(1);
    wcout << L"Added Days(7), Hours(1), Minutes(1) and Seconds(1): " << now.ToString() << endl;

    now.SubtractDays(7);
    now.SubtractHours(1);
    now.SubtractMinutes(1);
    now.SubtractSeconds(1);
    wcout << L"Subtracted Days(7), Hours(1), Minutes(1) and Seconds(1): " << now.ToString() << endl;

    wcout << L"Custom: " << now.ToString(L"dd.MM.yyyy HH:mm:ss") << endl;

    wcout << endl;
}

//
// Function: TestID
//
// (i): Tests ItsID.
//
void TestID()
{
    PrintTestHeader(L"## Test ItsID ");

    wcout << L"ID (12): " << ItsID::CreateID(12, ItsCreateIDOptions::LowerAndUpperCase, false) << endl;
    wcout << L"ID (10): " << ItsID::CreateID(10, ItsCreateIDOptions::LowerAndUpperCase, true) << endl;
    wcout << L"ID (8) : " << ItsID::CreateID(8, ItsCreateIDOptions::UpperCase, false) << endl;
    wcout << L"ID (6) : " << ItsID::CreateID(6, ItsCreateIDOptions::UpperCase, true) << endl;
    wcout << L"ID (14): " << ItsID::CreateID(14, ItsCreateIDOptions::LowerCase, false) << endl;
    wcout << L"ID (16): " << ItsID::CreateID(16, ItsCreateIDOptions::LowerCase, true) << endl;
    wcout << L"ID (64): " << ItsID::CreateID(64, ItsCreateIDOptions::LowerAndUpperCase, false) << endl;
    wcout << L"ID (64): " << ItsID::CreateID(64, ItsCreateIDOptions::LowerAndUpperCase, true) << endl;

    wcout << endl;
}