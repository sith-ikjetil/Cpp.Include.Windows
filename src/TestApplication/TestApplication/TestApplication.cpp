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
#include <mutex>
#include <limits>
#include <chrono>
#include "../../include/itsoftware.h"
#include "../../include/itsoftware-com.h"
#include "../../include/itsoftware-exceptions.h"
#include "../../include/itsoftware-win.h"
#include "../../include/itsoftware-win-core.h"

//
// using
//
using std::wcout;
using std::endl;
using std::thread;
using std::vector;
using std::wstring;
using std::wstringstream;
using std::unique_ptr;
using std::make_unique;
using std::mutex;
using std::lock_guard;
using ItSoftware::Win::Core::ItsTimer;
using ItSoftware::Win::Core::ItsTextFile;
using ItSoftware::Win::Core::ItsFileOpenCreation;
using ItSoftware::Win::Core::ItsFileTextType;
using ItSoftware::Win::Core::ItsFile;
using ItSoftware::Win::Core::ItsEvent;
using ItSoftware::Win::Core::ItsPath;
using ItSoftware::Win::Core::ItsDirectory;
using ItSoftware::Win::Core::ItsGuid;
using ItSoftware::Win::Core::ItsError;
using ItSoftware::Win::Core::ItsGuidFormat;
using ItSoftware::Win::Core::ItsFileMonitor;
using ItSoftware::Win::Core::ItsFileMonitorMask;
using ItSoftware::Win::Core::ItsFileMonitorEvent;
using ItSoftware::ItsTime;
using ItSoftware::ItsConvert;
using ItSoftware::ItsString;
using ItSoftware::ItsExpandDirection;
using ItSoftware::ItsRandom;
using ItSoftware::ItsDateTime;
using ItSoftware::ItsLog;
using ItSoftware::ItsLogType;
using ItSoftware::ItsDataSizeStringType;
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
void TestItsConvert();
void TestItsRandom();
void TestItsTime();
void TestItsString();
void TestItsTimerStart();
void TestItsTimerStop();
void TestItsTextFile();
void TestItsFile();
void TestItsEventStart();
void TestItsEventStop();
void TestItsPath();
void TestItsDirectory();
void TestItsGuid();
void TestItsLog();
void TestItsDateTime();
void TestItsID();
void TestItsFileMonitorStart();
void TestItsFileMonitorStop();
void ExitFn();
void PrintTestHeader(wstring txt);
void PrintTestSubHeader(wstring txt);
void PrintTestApplicationEvent(wstring event);
void HandleFileEvent(ItsFileMonitorEvent& event);

//
// global variables
//
ItsTimer g_timer;
ItsEvent g_event;
wstring g_fileMonDirectory(L"D:\\");
wstring g_filenameText(L"D:\\ItsTextFile.txt");
wstring g_filenameBinary(L"D:\\ItsFile.bin");
wstring g_filenameBinaryCopyTo(L"D:\\ItsFileCopy.bin");
thread g_eventThread;
wstring g_path1(L"C:\\");
wstring g_path2(L"Temp\\test.html");
wstring g_validPath(L"C:\\Temp\\test.html");
wstring g_invalidPath(L"C:\\?Temp\\test:\a.html");
wstring g_directoryRoot(L"C:\\Temp");
wstring g_creatDir(L"C:\\Temp\\CREATDIR");
unique_ptr<ItsFileMonitor> g_fm;
vector<wstring> g_fileMonNames;

//
// Function: ExitFn
//
// (i): Print exit message.
// 
void ExitFn()
{
    wcout << endl;
    PrintTestApplicationEvent(L"Completed");
}

// 
// Function: wmain
//
int wmain(int argc, wchar_t* argv[])
{
    atexit(ExitFn);

    PrintTestApplicationEvent(L"Started");

    TestItsTimerStart();
    TestItsEventStart();
    TestItsFileMonitorStart();
    TestItsConvert();
    TestItsRandom();
    TestItsTime();
    TestItsString();
    TestCOM1();
    TestCOM2();
    TestItsTextFile();
    TestItsFile();
    TestItsPath();
    TestItsDirectory();
    TestItsGuid();
    TestItsLog();
    TestItsDateTime();
    TestItsID();
    TestItsFileMonitorStop();
    TestItsEventStop();
    
    if (g_eventThread.joinable()) { g_eventThread.join(); }
    TestItsTimerStop();
    
    return EXIT_SUCCESS;
}

//
// Function: PrintTestApplicationEvent
//
// (i): prints application event string.
//
void PrintTestApplicationEvent(wstring event)
{
    wcout << std::setw(80) << std::setfill(L'#') << std::left << L"## Test Application " << endl;
    wcout << L"> " << event << L" <" << endl;
}

//
// Function: PrintTestHeader
//
// (i): Prints a tests header.
//
void PrintTestHeader(wstring txt)
{
    static mutex m;
    lock_guard<mutex> guard(m);

    wcout << endl;

    wstringstream ss;
    ss << " " << txt << " ";
    wcout << ItsString::WidthExpand(ss.str(), 80, '_', ItsExpandDirection::Middle) << endl;
}

//
// Function: PrintTestSubHeader
//
// (i): Prints a tests sub header.
//
void PrintTestSubHeader(wstring txt)
{
    wcout << endl;
    wcout << L"__ " << txt << L" __" << endl;
}

//
// Function: TestToNumber
//
// (i): Test numerics as string converted to primitive data types.
//
void TestItsConvert()
{
    PrintTestHeader(L"ItsConvert");

    PrintTestSubHeader(L"ToNumber");
    wcout << LR"(ItsConvert::ToNumber<int>(L"-1234"))" << endl;
    wcout << L"> " << ItsConvert::ToNumber<int>(L"-1234") << endl;
    wcout << LR"(ItsConvert::ToNumber<unsigned int>(L"1234"))" << endl;
    wcout << L"> " << ItsConvert::ToNumber<unsigned int>(L"1234") << endl;
    wcout << LR"(ItsConvert::ToNumber<long>(L"-1234"))" << endl;
    wcout << L"> " << ItsConvert::ToNumber<long>(L"-1234") << endl;
    wcout << LR"(ItsConvert::ToNumber<unsigned long>(L"123456789"))" << endl;
    wcout << L"> " << ItsConvert::ToNumber<unsigned long>(L"12346789") << endl;
    wcout << LR"(ItsConvert::ToNumber<long long>(L"12345678900"))" << endl;
    wcout << L"> " << ItsConvert::ToNumber<long long>(L"12345678900") << endl;
    wcout << LR"(ItsConvert::ToNumber<unsigned long long>(L"12345678900"))" << endl;
    wcout << L"> " << ItsConvert::ToNumber<unsigned long long>(L"12345678900") << endl;
    wcout << LR"(ItsConvert::ToNumber<float>(L"-12.34"))" << endl;
    wcout << L"> " << ItsConvert::ToNumber<float>(L"-12.34") << endl;
    wcout << LR"(ItsConvert::ToNumber<double>(L"1.234"))" << endl;
    wcout << L"> " << ItsConvert::ToNumber<double>(L"1.234") << endl;
    wcout << LR"(ItsConvert::ToNumber<long double>(L"1.234"))" << endl;
    wcout << L"> " << ItsConvert::ToNumber<long double>(L"1.234") << endl;
    wcout << LR"(ItsConvert::ToNumber<short>(L"1234"))" << endl;
    wcout << L"> " << ItsConvert::ToNumber<short>(L"1234") << endl;
    wcout << LR"(ItsConvert::ToNumber<unsigned short>(L"40001"))" << endl;
    wcout << L"> " << ItsConvert::ToNumber<unsigned short>(L"40001") << endl;

    PrintTestSubHeader(L"ToString");
    wcout << LR"(ItsConvert::ToString<int>(-1234))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToString<int>(-1234) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<unsigned int>(1234))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToString<unsigned int>(1234) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<long>(-1234))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToString<long>(-1234) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<unsigned long>(123456789))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToString<unsigned long>(12346789) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<long long>(12345678900))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToString<long long>(12345678900) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<unsigned long long>(12345678900))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToString<unsigned long long>(12345678900) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<float>(-12.34))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToString<float>(-12.34f) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<double>(1.234))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToString<double>(1.234) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<long double>(1.234))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToString<long double>(1.234) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<short>(1234))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToString<short>(1234) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToString<unsigned short>(40001))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToString<unsigned short>(40001) << LR"(")" << endl;

    PrintTestSubHeader(L"ToStringFormatted");
    wcout << LR"(ItsConvert::ToStringFormatted(256810246))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToStringFormatted(256810246) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToStringFormatted(256810246, L' '))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToStringFormatted(256810246, L' ') << LR"(")" << endl;
    
    PrintTestSubHeader(L"ToDataSizeString");
    wcout << LR"(ItsConvert::ToDataSizeString(1024, 2))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToDataSizeString(1024, 2) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToDataSizeString(200100400, 0))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToDataSizeString(200100400, 0) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToDataSizeString(200100400, 2))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToDataSizeString(200100400, 2) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToDataSizeString(9807804606, 3, ItsDataSizeStringType::IEC))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToDataSizeString(9807804606, 3, ItsDataSizeStringType::IEC) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToDataSizeString(1000, 3, ItsDataSizeStringType::IEC))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToDataSizeString(1000, 3, ItsDataSizeStringType::IEC) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToDataSizeString(size_t max, 2))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToDataSizeString(SIZE_MAX, 2) << LR"(")" << endl;
    wcout << LR"(ItsConvert::ToDataSizeString(size_t max, 2, ItsDataSizeStringType::IEC))" << endl;
    wcout << LR"(> ")" << ItsConvert::ToDataSizeString(SIZE_MAX, 2, ItsDataSizeStringType::IEC) << LR"(")" << endl;
    
    PrintTestSubHeader(L"ToLongFromHex");
    wcout << LR"(ItsConvert::ToLongFromHex("0xFF3333"))" << endl;
    wcout << L"> " << ItsConvert::ToLongFromHex(L"0xFF3333") << endl;

    wcout << endl;
}

//
// Function: TestRandom
//
// (i): Test random integers and floats.
//
void TestItsRandom()
{
    PrintTestHeader(L"ItsRandom");

    wcout << L"ItsRandom<long>(10'000, 1'000'000)" << endl;
    wcout << L"> " << ItsRandom<long>(10'000, 1'000'000) << endl;
    wcout << L"ItsRandom<long>(10'000, 1'000'000)" << endl;
    wcout << L"> " << ItsRandom<long>(10'000, 1'000'000) << endl;
    wcout << L"ItsRandom<float>(1, 10)" << endl;
    wcout << L"> " << ItsRandom<float>(1, 10) << endl;
    wcout << L"ItsRandom<float>(1, 10)" << endl;
    wcout << L"> " << ItsRandom<float>(1, 10) << endl;
    wcout << L"ItsRandom<double>(1, 2)" << endl;
    wcout << L"> " << ItsRandom<double>(1, 2) << endl;
    wcout << L"ItsRandom<double>(1, 2)" << endl;
    wcout << L"> " << ItsRandom<double>(1, 2) << endl;

    wcout << endl;
}

//
// Function: TestTime
//
// (i): Test rendering of time in milliseconds.
//
void TestItsTime()
{
    PrintTestHeader(L"ItsTime");
    
    PrintTestSubHeader(L"RenderMsToFullString");
    wcout << L"ItsTime::RenderMsToFullString(92481379, false)" << endl;
    wcout << LR"(> ")" << ItsTime::RenderMsToFullString(92481379, false) << LR"(")" << endl;
    wcout << L"ItsTime::RenderMsToFullString(92481379, true)" << endl;
    wcout << LR"(> ")" << ItsTime::RenderMsToFullString(92481379, true) << LR"(")" << endl;
    
    PrintTestSubHeader(L"Now + ToString");
    wcout << L"ItsDateTime::Now().ToString()" << endl;
    wcout << LR"(> ")" << ItsDateTime::Now().ToString() << LR"(")" << endl;
    wcout << LR"(ItsDateTime.Now().ToString(L"s"))" << endl;
    wcout << LR"(> ")" << ItsDateTime::Now().ToString(L"s") << LR"(")" << endl;
    
    wcout << endl;
}

//
// Function: TestString
//
// (i): Test string manipulation routines.
//
void TestItsString()
{
    PrintTestHeader(L"ItsString");

    wcout << LR"(ItsString::Left(L"Ab12Cd",4))" << endl;
    wcout << LR"(> ")" << ItsString::Left(L"Ab12Cd", 4) << LR"(")" << endl;
    wcout << LR"(ItsString::Right(L"Ab12Cd",4))" << endl;
    wcout << LR"(> ")" << ItsString::Right(L"Ab12Cd", 4) << LR"(")" << endl;
    wcout << LR"(ItsString::Mid(L"Ab12Cd",2,2))" << endl;
    wcout << LR"(> ")" << ItsString::Mid(L"Ab12Cd", 2, 2) << LR"(")" << endl;
    wcout << LR"(ItsString::ToLowerCase(L"Ab12Cd"))" << endl;
    wcout << LR"(> ")" << ItsString::ToLowerCase(L"Ab12Cd") << LR"(")" << endl;
    wcout << LR"(ItsString::ToUpperCase(L"Ab12Cd"))" << endl;
    wcout << LR"(> ")" << ItsString::ToUpperCase(L"Ab12Cd") << LR"(")" << endl;
    wcout << LR"(ItsString::Trim(L"  Ab12Cd  "))" << endl;
    wcout << LR"(> ")" << ItsString::Trim(L"  Ab12Cd  ") << LR"(")" << endl;
    wcout << LR"(ItsString::Replace(L"__ ABCDEF __"))" << endl;
    wcout << LR"(> ")" << ItsString::Replace(L"__ ABCDEF __", L"__", L"##") << LR"(")" << endl;
    wcout << LR"(ItsString::Split(L"A;BC;DEF;G",L";"))" << endl;
    auto vs = ItsString::Split(L"A;BC;DEF;G", L";");
    wstringstream ss;
    ss << L"{";
    bool bFirst{ true };
    for (auto s : vs) {
        if (!bFirst) {
            ss << L",";
        }
        ss << LR"(")" << s << LR"(")";
        if (bFirst) {
            bFirst = false;
        }
    }
    ss << L"}";
    
    wstring temp = ss.str();
    wcout << LR"(> )" << temp << endl;
    wcout << LR"(ItsString::WidthExpand (L"Kjetil", 30, L'_', ItsExpandDirection:Left))" << endl;
    wcout << LR"(> ")" << ItsString::WidthExpand(L"Kjetil", 30, L'_', ItsExpandDirection::Left) << LR"(")" << endl;
    wcout << LR"(ItsString::WidthExpand (L"Kjetil", 30, L'_', ItsExpandDirection:Middle))" << endl;
    wcout << LR"(> ")" << ItsString::WidthExpand(L"Kjetil", 30, L'_', ItsExpandDirection::Middle) << LR"(")" << endl;
    wcout << LR"(ItsString::WidthExpand (L"Kjetil", 30, L'_', ItsExpandDirection:Right))" << endl;
    wcout << LR"(> ")" << ItsString::WidthExpand(L"Kjetil", 30, L'_', ItsExpandDirection::Right) << LR"(")" << endl;
    
    wcout << endl;
}

//
// Function: TestTimerStart
//
// (i): Starts timer
//
void TestItsTimerStart()
{
    PrintTestHeader(L"ItsTimer Start");

    g_timer.Start();
    wcout << L"Timer started..." << endl;
    
    wcout << endl;
}

//
// Function: TestTimerStop
//
// (i): Stops timer
//
void TestItsTimerStop()
{
    PrintTestHeader(L"ItsTimer Stop");

    g_timer.Stop();

    wcout << L"Time elapsed: " << ItsTime::RenderMsToFullString(g_timer.GetMilliseconds(), true) << endl;
    wcout << endl;
}

//
// Function: TestFileText
//
// (i): Tests files
//
void TestItsTextFile()
{
    PrintTestHeader(L"ItsTextFile");

    ItsTextFile file{};
    wcout << LR"(file.OpenOrCreateText(g_filenameText, L"rw", L"", ItsFileOpenCreation::CreateAlways, ItsFileTextType::UTF8NoBOM))" << endl;
    bool bResult = file.OpenOrCreateText(g_filenameText, L"rw", L"", ItsFileOpenCreation::CreateAlways, ItsFileTextType::UTF8NoBOM);
    if (!bResult) {
        wcout << L"> FAILED: " << ItsError::GetLastErrorDescription() << endl;
        wcout << endl;
        return;
    }
    wcout << LR"(> Success opening or creating )" << g_filenameText << endl;

    wcout << LR"(file.WriteText(L"Small step for man." + wstring(ItsTextFile::LineDelimiterUnix) + L"Large leap for mankind."))" << endl;
    bResult = file.WriteText(L"Small step for man." + wstring(ItsTextFile::LineDelimiterUnix) + L"Large leap for mankind.");
    if (!bResult) {
        wcout << L"> FAILED: " << ItsError::GetLastErrorDescription() << endl;
        wcout << endl;
        return;
    }
    wcout << LR"(> Success)" << endl;

    wcout << LR"(file.Close())" << endl;
    bResult = file.Close();
    if (!bResult) {
        wcout << L"> FAILED: " << ItsError::GetLastErrorDescription() << endl;
        wcout << endl;
        return;
    }
    wcout << LR"(> Success)" << endl;

    wcout << endl;
}

//
// Function: TestFileBinary
//
// (i): Tests files
//
void TestItsFile()
{
    PrintTestHeader(L"ItsFile");

    ItsFile file{};
    wcout << LR"(file.OpenOrCreate(g_filenameBinary, L"rw", L"", ItsFileOpenCreation::CreateAlways))" << endl;
    bool bResult = file.OpenOrCreate(g_filenameBinary, L"rw", L"", ItsFileOpenCreation::CreateAlways);
    if (!bResult) {
        wcout << L"> FAILED: " << ItsError::GetLastErrorDescription() << endl;
        wcout << endl;
        return;
    }
    wcout << L"> Success opening or creating " << g_filenameBinary << endl;

    vector<BYTE> data = {0x01, 0x02, 0x03, 0x04, 0x05 };
    DWORD dwWritten(0);
    wcout << LR"(file.Write(data.data(), static_cast<DWORD>(data.size()), &dwWritten))" << endl;
    bResult = file.Write(data.data(), static_cast<DWORD>(data.size()), &dwWritten);
    if (!bResult) {
        wcout << L"> FAILED: " << ItsError::GetLastErrorDescription() << endl;
        return;
    }   
    wcout << L"> Success" << endl;
    
    wcout << LR"(file.Close())" << endl;
    bResult = file.Close();
    if (!bResult) {
        wcout << L"> FAILED: " << ItsError::GetLastErrorDescription() << endl;
        wcout << endl;
        return;
    }
    wcout << L"> Success" << endl;
    
    size_t size{ 0 };
    wcout << LR"(ItsFile::GetFileSize(g_filenameBinary, &size))" << endl;
    if (!ItsFile::GetFileSize(g_filenameBinary, &size)) {
        wcout << L"> FAILED: " << ItsError::GetLastErrorDescription() << endl;
        wcout << endl;
        return;
    }
    wcout << L"> Success. File " << g_filenameBinary << L" is " << size << L" bytes in size" << endl;

    wcout << LR"(ItsFile::Copy(g_filenameBinary, g_filenameBinaryCopyTo, false))" << endl;
    if (!ItsFile::Copy(g_filenameBinary, g_filenameBinaryCopyTo, false)) {
        wcout << L"> FAILED: " << ItsError::GetLastErrorDescription() << endl;
        wcout << endl;
        return;
    }
    wcout << L"> Success. File " << g_filenameBinary << " successfully copied to " << g_filenameBinaryCopyTo << endl;

    wcout << LR"(ItsFile::Exists(g_filenameBinaryCopyTo))" << endl;
    if (!ItsFile::Exists(g_filenameBinaryCopyTo)) {
        wcout << L"> File " << g_filenameBinaryCopyTo << " does not exist" << endl;
    }
    else {
        wcout << L"> File " << g_filenameBinaryCopyTo << " exists" << endl;
    }

    wcout << LR"(ItsFile::Delete(g_filenameBinaryCopyTo))" << endl;
    if (!ItsFile::Delete(g_filenameBinaryCopyTo)) {
        wcout << L"> FAILED: " << ItsError::GetLastErrorDescription() << endl;
        wcout << endl;
        return;
    }
    wcout << L"> Success. File " << g_filenameBinaryCopyTo << " successfully deleted" << endl;

    wcout << endl;
}

//
// Function: TestEventStart
//
// (i): Start testing event
//
void TestItsEventStart()
{
    g_event.Clear();

    PrintTestHeader(L"ItsEvent Start"); 
    wcout << L"Event is cleared" << endl;
    
    g_eventThread = thread([] {
        g_event.Wait(INFINITE);
        PrintTestHeader(L"ItsEvent Completed");
        wcout << L"Event works as expected" << endl;
        wcout << endl;
        });

    wcout << endl;
}

//
// Function: TestEventStop
//
// (i): Stop testing event.
//
void TestItsEventStop()
{
    g_event.Signal();

    PrintTestHeader(L"ItsEvent Stop");
    wcout << L"Event is Signaled" << endl;
    
    wcout << endl;
}

//
// Function: TestPath
//
// (i): Test ItsPath
//
void TestItsPath()
{
    PrintTestHeader(L"ItsPath");

    wstring path = ItsPath::Combine(g_path1, g_path2);
    if (ItsPath::Exists(path)) {
        wcout << L"> Path: " << path << L" exists" << endl;
    }
    else {
        wcout << L"> Path: " << path << L" does not exist" << endl;
    }

    wcout << LR"(ItsPath::GetVolume(path))" << endl;
    wcout << LR"(> ")" << ItsPath::GetVolume(path) << LR"(")" << endl;
    wcout << LR"(ItsPath::GetDirectory(path))" << endl; 
    wcout << LR"(> ")" << ItsPath::GetDirectory(path) << LR"(")" << endl;
    wcout << LR"(ItsPath::GetFilename(path))" << endl; 
    wcout << LR"(> ")" << ItsPath::GetFilename(path) << LR"(")" << endl;
    wcout << LR"(ItsPath::GetExtension(path))" << endl; 
    wcout << LR"(> ")" << ItsPath::GetExtension(path) << LR"(")" << endl;
    wcout << LR"(ItsPath::HasExtension(path, L".html"))" << endl; 
    wcout << LR"(> )" << ((ItsPath::HasExtension(path, L".html")) ? L"true" : L"false") << endl;
    wcout << LR"(ItsPath::HasExtension(path, L".js"))" << endl; 
    wcout << LR"(> )" << ((ItsPath::HasExtension(path, L".js")) ? L"true" : L"false") << endl;
    wcout << LR"(ItsPath::ChangeExtension(path,L".js"))" << endl; 
    wcout << LR"(> ")" << ItsPath::ChangeExtension(path, L".js") << LR"(")" << endl;
    wcout << LR"(ItsPath::IsPathValid(path))" << endl; 
    wcout << LR"(> )" << ((ItsPath::IsPathValid(path)) ? L"true" : L"false") << endl;
    wcout << LR"(ItsPath::IsPathValid(g_invalidPath))" << endl; 
    wcout << LR"(> )" << ((ItsPath::IsPathValid(g_invalidPath)) ? L"true" : L"false") << endl;

    wcout << endl;
}

//
// Function: TestDirectory
//
// (i): Test ItsDirectory
//
void TestItsDirectory()
{
    PrintTestHeader(L"ItsDirectory");

    wcout << LR"(ItsDirectory::GetDirectories(g_directoryRoot))" << endl;
    auto result = ItsDirectory::GetDirectories(g_directoryRoot);
    if (result.size() > 0) {
        wcout << L"> Success. Found " << result.size() << L" sub-directories under " << g_directoryRoot << endl;
        for (auto r : result) {
            wcout << L">> " << r << endl;
        }
    }
    else {
        wcout << L"> FAILED. No directories found under " << g_directoryRoot << endl;
    }

    wcout << LR"(ItsDirectory::GetFiles(g_directoryRoot))" << endl;
    auto result2 = ItsDirectory::GetFiles(g_directoryRoot);
    if (result2.size() > 0) {
        wcout << L"> Success. Found " << result2.size() << L" files under " << g_directoryRoot << endl;
        for (auto r : result2) {
            wcout << L">> " << r << endl;
        }
    }

    wcout << LR"(ItsDirectory::GetLogicalDrives())" << endl;
    auto result3 = ItsDirectory::GetLogicalDrives();
    if (result3.size() > 0) {
        wcout << L"> Success. Found " << result3.size() << L" drives" << endl;
        for (auto r : result3) {
            wcout << L">> " << r << endl;
        }
    }

    auto cdir = g_creatDir;
    wcout << LR"(ItsDirectory::CreateDirectory(cdir))" << endl;
    bool bResult = ItsDirectory::CreateDirectory(cdir);
    if (!bResult) {
        wcout << L"> FAILED. Error: " << ItsError::GetLastErrorDescription() << endl;
        wcout << endl;
        return;
    }
    wcout << L"> Success creating " << cdir << endl;

    wcout << LR"(ItsDirectory::RemoveDirectory(cdir))" << endl;
    bResult = ItsDirectory::RemoveDirectory(cdir);
    if (!bResult) {
        wcout << L"> FAILED: " << ItsError::GetLastErrorDescription() << endl;;
        wcout << endl;
        return;
    }
    wcout << L"> Success removing directory " << cdir << endl;

    wcout << endl;
}

//
// Function: TestGuid
//
// (i): Test ItsGuid
//
void TestItsGuid()
{
    PrintTestHeader(L"ItsGuid");

    wcout << L"ItsGuid::CreateGuid()" << endl;
    wcout << LR"(> ")" << ItsGuid::CreateGuid() << LR"(")" << endl;
    
    GUID guid{ 0 };
    wcout << L"ItsGuid::CreateGuid(&guid)" << endl;
    if (ItsGuid::CreateGuid(&guid))
    {
        wcout << L"> Success" << endl;

        wcout << L"ItsGuid::ToString(guid,ItsGuidFormat::RegistryFormat)" << endl;
        wcout << LR"(> ")" << ItsGuid::ToString(guid, ItsGuidFormat::RegistryFormat) << LR"(")" << endl;
        wcout << L"ItsGuid::ToString(guid, ItsGuidFormat::RegistryFormatStripped)" << endl;
        wcout << LR"(> ")" << ItsGuid::ToString(guid, ItsGuidFormat::RegistryFormatStripped) << LR"(")" << endl;
        wcout << L"ItsGuid::ToString(guid,ItsGuidFormat::ConstFormat)" << endl;
        wcout << LR"(> ")" << ItsGuid::ToString(guid, ItsGuidFormat::ConstFormat) << LR"(")" << endl;
        wcout << L"ItsGuid::ToString(guid,ItsGuidFormat::CompactFormat)" << endl;
        wcout << LR"(> ")" << ItsGuid::ToString(guid, ItsGuidFormat::CompactFormat) << LR"(")" << endl;
        wcout << L"ItsGuid::ToString(guid,ItsGuidFormat::PrefixedCompactFormat)" << endl;
        wcout << LR"(> ")" << ItsGuid::ToString(guid, ItsGuidFormat::PrefixedCompactFormat) << LR"(")" << endl;
    }
    else {
        wcout << L"> FAILED: " << ItsError::GetLastErrorDescription() << endl;
    }

    wcout << endl;
}

//
// Function: TestLog
//
// (i): Test ItsLog
//
void TestItsLog()
{
    PrintTestHeader(L"ItsLog");

    ItsLog log{L"ItsTestApp",true};
    log.LogInformation(L"This is an information log item");
    log.LogWarning(L"This is an warning log item");
    log.LogError(L"This is an error log item");
    log.LogOther(L"This is an other log item");
    log.LogDebug(L"This is an debug log item");
    
    PrintTestSubHeader(L"ToFriendlyString");
    wcout << log.ToFriendlyString() << endl;

    PrintTestSubHeader(L"ToString");
    wcout << log.ToString() << endl;

    wcout << endl;
}

//
// Function: TestDateTime
//
// (i): Test ItsDateTime
//
void TestItsDateTime()
{
    PrintTestHeader(L"ItsDateTime");

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
void TestItsID()
{
    PrintTestHeader(L"ItsID");

    wcout << L"ItsID::CreateID(12, ItsCreateIDOptions::LowerAndUpperCase, false)" << endl;
    wcout << LR"(> ")" << ItsID::CreateID(12, ItsCreateIDOptions::LowerAndUpperCase, false) << LR"(")" << endl;
    wcout << L"ItsID::CreateID(10, ItsCreateIDOptions::LowerAndUpperCase, true)" << endl;
    wcout << LR"(> ")" << ItsID::CreateID(10, ItsCreateIDOptions::LowerAndUpperCase, true) << LR"(")" << endl;
    wcout << L"ItsID::CreateID(8, ItsCreateIDOptions::UpperCase, false)" << endl;
    wcout << LR"(> ")" << ItsID::CreateID(8, ItsCreateIDOptions::UpperCase, false) << LR"(")" << endl;
    wcout << L"ItsID::CreateID(6, ItsCreateIDOptions::UpperCase, true)" << endl;
    wcout << LR"(> ")" << ItsID::CreateID(6, ItsCreateIDOptions::UpperCase, true) << LR"(")" << endl;
    wcout << L"ItsID::CreateID(14, ItsCreateIDOptions::LowerCase, false)" << endl;
    wcout << LR"(> ")" << ItsID::CreateID(14, ItsCreateIDOptions::LowerCase, false) << LR"(")" << endl;
    wcout << L"ItsID::CreateID(16, ItsCreateIDOptions::LowerCase, true)" << endl;
    wcout << LR"(> ")" << ItsID::CreateID(16, ItsCreateIDOptions::LowerCase, true) << LR"(")" << endl;
    wcout << L"ItsID::CreateID(64, ItsCreateIDOptions::LowerAndUpperCase, false)" << endl;
    wcout << LR"(> ")" << ItsID::CreateID(64, ItsCreateIDOptions::LowerAndUpperCase, false) << LR"(")" << endl;
    wcout << L"ItsID::CreateID(64, ItsCreateIDOptions::LowerAndUpperCase, true)" << endl;
    wcout << LR"(> ")" << ItsID::CreateID(64, ItsCreateIDOptions::LowerAndUpperCase, true) << LR"(")" << endl;

    wcout << endl;
}

//
// Function: TestItsFileMonitor
//
// (i): Tests ItsFileMonitor.
//
void TestItsFileMonitorStart()
{
    g_fm = make_unique<ItsFileMonitor>(g_fileMonDirectory, false, ItsFileMonitorMask::ChangeLastWrite, HandleFileEvent);

    PrintTestHeader(L"ItsFileMonitor Start");
    wcout << L"File monitor monitoring directory '" << g_fileMonDirectory << L"' with mask 'ItsFileMonitorMask::ChangeLastWrite'" << endl;

    wcout << endl;
}

//
// Function: TestItsFileMonitor
//
// (i): Tests ItsFileMonitor.
//
void TestItsFileMonitorStop()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    g_fm->Stop();

    PrintTestHeader(L"ItsFileMonitor Stop");
    wcout << L"File monitor monitoring directory '" << g_fileMonDirectory << L"' with mask 'ItsFileNonitorMask::ChangeLastWrite'" << endl;
    wcout << L"Items found:" << endl;
    for (auto i : g_fileMonNames) {
        wcout << L">> " << i << endl;
    }

    wcout << endl;
}

//
// Function: HandleFileEvent
//
// (i): handle file event.
//
void HandleFileEvent(ItsFileMonitorEvent& event)
{
    wstringstream ss;
    ss << L"Name: " << event.FileName << L", Action: ";
    switch (event.Action)
    {
    case FILE_ACTION_ADDED:
        ss << L"[FILE_ACTION_ADDED] ";
        break;
    case FILE_ACTION_REMOVED:
        ss << L"[FILE_ACTION_REMOVED] ";
        break;
    case FILE_ACTION_MODIFIED:
        ss << L"[FILE_ACTION_MODIFIED] ";
        break;
    case FILE_ACTION_RENAMED_OLD_NAME:
        ss << L"[FILE_ACTION_RENAMED_OLD_NAME] ";
        break;
    case FILE_ACTION_RENAMED_NEW_NAME:
        ss << L"[FILE_ACTION_RENAMED_NEW_NAME] ";
        break;
    default:
        ss << L"?";
        break;
    }
    g_fileMonNames.push_back(ss.str());
}