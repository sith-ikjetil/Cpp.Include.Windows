///////////////////////////////////////////////////////////////////
//: Title        : itsoftware-win.h
//: Product:     : Cpp.Include.Windows
//: Date         : 2020-05-01
//: Author       : "Kjetil Kristoffer Solberg" <post@ikjetil.no>
//: Version      : 1.0.0.0
//: Descriptions : Implementation of Cpp.Include.Windows. Uses Shlwapi.lib.
#pragma once
//
// #include
//
#include <windows.h>
#include <shlobj.h>
#include <WinUser.h>
#include <CommCtrl.h>
#include <memory>
#include <string>
#include <wincrypt.h>
#include <Shlwapi.h>
#include <vector>
#include <functional>
#include <thread>
#include "itsoftware.h"
#include "itsoftware-exceptions.h"
#include <time.h>
#include <map>
#include <chrono>
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <limits>
#include <sstream>
#include <string_view>
#include <cwchar>
#include <type_traits>
#include <utility>

//
// #pragma
//
#pragma warning(disable: 4244)

//
// namespace
//
namespace ItSoftware::Win::Core
{
	//
	// using
	//
	using std::unique_ptr;
	using std::vector;
	using std::wstring;
	using std::make_unique;
	using std::function;
	using std::thread;
	using std::map;
	using ItSoftware::ItsString;

	//
	// How to output time tracking results
	//
	enum class ItsTimeTrackerOutput {
		Console,
		MessageBox
	};

	//
	// class: ItsTimeTracker
	//
	// (i): Simple time tracking utility that prints elapsed time to console when going out of scope.
	//
	class ItsTimeTracker {
	private:
		std::chrono::time_point<std::chrono::steady_clock> start, end;
		std::string  name;
		std::wstring wname;
		ItsTimeTrackerOutput alertOutput{ ItsTimeTrackerOutput::Console };
		bool wide{ false };
		bool callback{ false };
		std::function<void(const std::string& name, std::chrono::steady_clock::duration)> fnComplete = nullptr;
		std::function<void(const std::wstring& name, std::chrono::steady_clock::duration)> wfnComplete = nullptr;
	public:
		explicit ItsTimeTracker(const std::string& fName)
			: name(fName),
			wide(false),
			start(std::chrono::steady_clock::now())
		{}

		ItsTimeTracker(const std::string& fName, ItsTimeTrackerOutput output)
			: name(fName),
			alertOutput(output),
			wide(false),
			start(std::chrono::steady_clock::now())
		{}

		explicit ItsTimeTracker(const std::wstring& fName)
			: wname(fName),
			wide(true),
			start(std::chrono::steady_clock::now())
		{}

		ItsTimeTracker(const std::wstring& fName, ItsTimeTrackerOutput output)
			: wname(fName),
			alertOutput(output),
			wide(true),
			start(std::chrono::steady_clock::now())
		{}

		ItsTimeTracker(const std::string& fName, std::function<void(std::string name, std::chrono::steady_clock::duration)> ff)
			: name(fName),
			fnComplete(ff),
			wide(false),
			start(std::chrono::steady_clock::now()),
			callback(true)
		{}

		ItsTimeTracker(const std::wstring& fName, std::function<void(std::wstring name, std::chrono::steady_clock::duration)> ff)
			: wname(fName),
			wfnComplete(ff),
			wide(true),
			start(std::chrono::steady_clock::now()),
			callback(true)
		{}

		~ItsTimeTracker() noexcept
			try
			{
				end = std::chrono::steady_clock::now();

				if (callback) {
					if (wide) {
						wfnComplete(this->wname, (end - start));
					}
					else {
						fnComplete(this->name, (end - start));
					}
				}
				else {
					if (alertOutput == ItsTimeTrackerOutput::MessageBox) {
						if (wide) {
							auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
							auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
							auto duration3 = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
							std::wstring msg = L"Time taken: " + std::to_wstring(duration1) + L" ms | " + std::to_wstring(duration2) + L" us | " + std::to_wstring(duration3) + L" ns";
							MessageBoxW(NULL, msg.c_str(), wname.c_str(), MB_OK | MB_ICONINFORMATION);
						}
						else {
							auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
							auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
							auto duration3 = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
							std::string msg = "Time taken: " + std::to_string(duration1) + " ms | " + std::to_string(duration2) + " us | " + std::to_string(duration3) + " ns";
							MessageBoxA(NULL, msg.c_str(), name.c_str(), MB_OK | MB_ICONINFORMATION);
						}
						return;
					}
					else if (alertOutput == ItsTimeTrackerOutput::Console) {
						// continue to print to console
						auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
						auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
						auto duration3 = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

						//std::println("{}: {} ms | {} us | {} ns", name, duration1, duration2, duration3);
						if (wide)
						{
							std::wcout << L">> " << wname << L": " << duration1 << L" ms | " << duration2 << L" us | " << duration3 << L" ns" << L'\n';
						}
						else
						{
							std::cout << ">> " << name << ": " << duration1 << " ms | " << duration2 << " us | " << duration3 << " ns" << '\n';
						}
					}
					else {
						// Unknown output type, default to console
					}
				}
			}
			catch (...) {
				// Reporting must not let an exception escape the destructor.
			}

			// No copying or moving
			ItsTimeTracker(const ItsTimeTracker&) = delete;
			ItsTimeTracker& operator=(const ItsTimeTracker&) = delete;
			ItsTimeTracker(ItsTimeTracker&&) = delete;
			ItsTimeTracker& operator=(ItsTimeTracker&&) = delete;
	};

	//
	// struct: ItsTimer
	//
	// (i): Timer wrapper.
	//
	struct ItsTimer
	{
	private:
		std::chrono::time_point<std::chrono::steady_clock> m_start;
		std::chrono::time_point<std::chrono::steady_clock> m_end;
		bool m_isRunning = false;

	public:
		void Start()
		{
			if (!this->IsRunning())
			{
				this->m_start = std::chrono::steady_clock::now();
				this->m_end = m_start;
				this->m_isRunning = true;
			}
		}

		void Stop()
		{
			if (this->m_isRunning) {
				this->m_end = std::chrono::steady_clock::now();
				this->m_isRunning = false;
			}
		}

		bool IsRunning()
		{
			return this->m_isRunning;
		}

		int64_t LapSeconds()
		{
			if (!this->IsRunning())
			{
				return 0;
			}

			return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - this->m_start).count();
		}

		int64_t LapMilliseconds()
		{
			if (!this->IsRunning())
			{
				return 0;
			}

			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->m_start).count();
		}

		int64_t LapMicroseconds()
		{
			if (!this->IsRunning())
			{
				return 0;
			}

			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - this->m_start).count();
		}

		int64_t LapNanoseconds()
		{
			if (!this->IsRunning())
			{
				return 0;
			}
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - this->m_start).count();
		}

		int64_t GetSeconds()
		{
			if (this->IsRunning())
			{
				return 0;
			}

			return std::chrono::duration_cast<std::chrono::seconds>(this->m_end - this->m_start).count();
		}

		int64_t GetMilliseconds()
		{
			if (this->IsRunning())
			{
				return 0;
			}

			return std::chrono::duration_cast<std::chrono::milliseconds>(this->m_end - this->m_start).count();
		}

		int64_t GetMicroseconds()
		{
			if (this->IsRunning())
			{
				return 0;
			}

			return std::chrono::duration_cast<std::chrono::microseconds>(this->m_end - this->m_start).count();
		}

		int64_t GetNanoseconds()
		{
			if (this->IsRunning())
			{
				return 0;
			}
			return std::chrono::duration_cast<std::chrono::nanoseconds>(this->m_end - this->m_start).count();
		}
	};

	//
	// struct: ItsGuidFormat
	//
	// (i): Container for premade Guid format strings.
	//
	struct ItsGuidFormat {
		inline static constexpr const wchar_t* RegistryFormat{ L"{%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}" };
		inline static constexpr const wchar_t* RegistryFormatStripped{ L"%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X" };
		inline static constexpr const wchar_t* ConstFormat{ L"{ 0x%lx, 0x%x, 0x%x, { 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x } }" };
		inline static constexpr const wchar_t* CompactFormat{ L"%08lX%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X" };
		inline static constexpr const wchar_t* PrefixedCompactFormat{ L"GUID%08lX%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X" };
	};

	//
	// struct: ItsGuid
	//
	// (i): Wrapper for Guid generation and rendering.
	//
	struct ItsGuid
	{
	public:
		static wstring CreateGuid()
		{
			GUID guid{ 0 };
			HRESULT hr = CoCreateGuid(&guid);
			if (FAILED(hr)) {
				return wstring(L"");
			}

			return ItsGuid::ToString(guid);
		}
		static bool CreateGuid(GUID* pGuid)
		{
			if (pGuid == nullptr) return false;
			HRESULT hr = CoCreateGuid(pGuid);
			if (FAILED(hr)) {
				return false;
			}
			return true;
		}
		static wstring ToString(GUID& guid) {
			return ItsGuid::ToString(guid, ItsGuidFormat::RegistryFormat);
		}
		static wstring ToString(GUID& guid, wstring format) {
			size_t argument = 0;
			bool firstIsLong = false;
			for (size_t i = 0; i < format.size(); ++i) {
				if (format[i] != L'%') continue;
				if (++i == format.size()) return {};
				if (format[i] == L'%') continue;
				i = format.find_first_not_of(L"-+# 0.123456789", i);
				if (i == wstring::npos || argument >= 11) return {};
				const bool wideInteger = format[i] == L'l';
				if (wideInteger && ++i == format.size()) return {};
				if ((wideInteger && argument != 0) ||
					std::wstring_view(L"xXuo").find(format[i]) == std::wstring_view::npos) return {};
				if (argument == 0) firstIsLong = wideInteger;
				++argument;
			}
			const auto render = [&](wchar_t* buffer, size_t size, auto first) {
				return std::swprintf(buffer, size, format.c_str(),
					first, static_cast<unsigned>(guid.Data2), static_cast<unsigned>(guid.Data3),
					static_cast<unsigned>(guid.Data4[0]), static_cast<unsigned>(guid.Data4[1]),
					static_cast<unsigned>(guid.Data4[2]), static_cast<unsigned>(guid.Data4[3]),
					static_cast<unsigned>(guid.Data4[4]), static_cast<unsigned>(guid.Data4[5]),
					static_cast<unsigned>(guid.Data4[6]), static_cast<unsigned>(guid.Data4[7]));
				};
			vector<wchar_t> buffer(128);
			while (buffer.size() <= 65536) {
				const int count = firstIsLong
					? render(buffer.data(), buffer.size(), guid.Data1)
					: render(buffer.data(), buffer.size(), static_cast<unsigned>(guid.Data1));
				if (count >= 0 && static_cast<size_t>(count) < buffer.size()) {
					return wstring(buffer.data(), count);
				}
				buffer.resize(buffer.size() * 2);
			}
			return {};
		}
	};


	//
	// struct: ItsPath
	// 
	// (i): Path routines.
	//
	struct ItsPath
	{
	public:
		inline static const wchar_t VolumeSeparator = ':';
		inline static const wchar_t PathSeparator = '\\';
		inline static const wchar_t AlternatePathSeparator = '/';
		inline static const wchar_t ExtensionSeparator = '.';
		static wstring NormalizePath(wstring path)
		{
			if (path.size() == 0) {
				return wstring(L"");
			}

			wstring aps;
			aps += ItsPath::AlternatePathSeparator;

			wstring ps;
			ps += ItsPath::PathSeparator;

			path = ItsString::Replace(path, aps, ps);

			return path;

		}
		static const vector<wchar_t> GetInvalidPathCharacters()
		{
			vector<wchar_t> chars;
			chars.push_back(static_cast<wchar_t>(0x0022));
			chars.push_back(static_cast<wchar_t>(0x003c));
			chars.push_back(static_cast<wchar_t>(0x003e));
			chars.push_back(static_cast<wchar_t>(0x007c));
			chars.push_back(static_cast<wchar_t>(0x0000));
			chars.push_back(static_cast<wchar_t>(0x0001));
			chars.push_back(static_cast<wchar_t>(0x0002));
			chars.push_back(static_cast<wchar_t>(0x0003));
			chars.push_back(static_cast<wchar_t>(0x0004));
			chars.push_back(static_cast<wchar_t>(0x0005));
			chars.push_back(static_cast<wchar_t>(0x0006));
			chars.push_back(static_cast<wchar_t>(0x0007));
			chars.push_back(static_cast<wchar_t>(0x0008));
			chars.push_back(static_cast<wchar_t>(0x0009));
			chars.push_back(static_cast<wchar_t>(0x000a));
			chars.push_back(static_cast<wchar_t>(0x000b));
			chars.push_back(static_cast<wchar_t>(0x000c));
			chars.push_back(static_cast<wchar_t>(0x000d));
			chars.push_back(static_cast<wchar_t>(0x000e));
			chars.push_back(static_cast<wchar_t>(0x000f));
			chars.push_back(static_cast<wchar_t>(0x0010));
			chars.push_back(static_cast<wchar_t>(0x0011));
			chars.push_back(static_cast<wchar_t>(0x0012));
			chars.push_back(static_cast<wchar_t>(0x0013));
			chars.push_back(static_cast<wchar_t>(0x0014));
			chars.push_back(static_cast<wchar_t>(0x0015));
			chars.push_back(static_cast<wchar_t>(0x0016));
			chars.push_back(static_cast<wchar_t>(0x0017));
			chars.push_back(static_cast<wchar_t>(0x0018));
			chars.push_back(static_cast<wchar_t>(0x0019));
			chars.push_back(static_cast<wchar_t>(0x001a));
			chars.push_back(static_cast<wchar_t>(0x001b));
			chars.push_back(static_cast<wchar_t>(0x001c));
			chars.push_back(static_cast<wchar_t>(0x001d));
			chars.push_back(static_cast<wchar_t>(0x001e));
			chars.push_back(static_cast<wchar_t>(0x001f));
			return chars;
		}
		static const vector<wchar_t> GetInvalidFilenameCharacters()
		{
			vector<wchar_t> chars = ItsPath::GetInvalidPathCharacters();
			chars.push_back(static_cast<wchar_t>(0x003a));
			chars.push_back(static_cast<wchar_t>(0x002a));
			chars.push_back(static_cast<wchar_t>(0x003f));
			chars.push_back(static_cast<wchar_t>(0x005c));
			chars.push_back(static_cast<wchar_t>(0x002f));
			return chars;
		}
		static wstring Combine(wstring path1, wstring path2)
		{
			if (path1.size() == 0 && path2.size() == 0) {
				return wstring(L"");
			}

			if (path1.size() == 0) {
				return path2;
			}

			if (path2.size() == 0) {
				return path1;
			}

			//wstring aps;
			//aps += ItsPath::AlternatePathSeparator;

			//wstring ps;
			//ps += ItsPath::PathSeparator;

			path1 = ItsPath::NormalizePath(path1);
			path2 = ItsPath::NormalizePath(path2);

			wstringstream path;
			path << path1;
			if (path1.back() == ItsPath::PathSeparator && path2.front() == ItsPath::PathSeparator) {
				path2.erase(0, 1);
			}
			else if (path1.back() != ItsPath::PathSeparator && path2.front() != ItsPath::PathSeparator) {
				path << ItsPath::PathSeparator;
			}
			path << path2;

			wstring ret = path.str();
			return ret;
		}
		static bool Exists(const wstring& path)
		{
			return static_cast<bool>(PathFileExistsW(path.c_str()));
		}
		static bool IsFile(const wstring& path)
		{
			DWORD fileAttributes = GetFileAttributesW(path.c_str());
			return (fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY));
		}
		static bool IsDirectory(const wstring& path)
		{
			DWORD fileAttributes = GetFileAttributesW(path.c_str());
			return (fileAttributes != INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY));
		}
		static wstring GetVolume(const wstring& path)
		{
			if (path.size() >= 2) {
				if (path[1] == ItsPath::VolumeSeparator) {
					wstring ret;
					ret += path[0];
					return ret;
				}
			}
			return wstring(L"");
		}
		static wstring GetDirectory(wstring path)
		{
			if (path.size() == 0) {
				return wstring(L"");
			}

			path = ItsPath::NormalizePath(path);

			auto i = path.find_last_of(ItsPath::PathSeparator);
			if (i == std::wstring::npos) {
				return wstring(L"");
			}
			return path.substr(0, i + 1);
		}
		static wstring GetFilename(wstring path)
		{
			if (path.size() == 0) {
				return wstring(L"");
			}

			path = ItsPath::NormalizePath(path);
			const auto separator = path.rfind(ItsPath::PathSeparator);
			if (separator != wstring::npos) return path.substr(separator + 1);
			return ItsPath::GetVolume(path).empty() ? path : path.substr(2);
		}
		static wstring GetExtension(wstring path)
		{
			if (path.size() == 0) {
				return wstring(L"");
			}

			const auto filename = ItsPath::GetFilename(path);
			const auto dot = filename.rfind(ItsPath::ExtensionSeparator);
			return dot == wstring::npos ? wstring{} : filename.substr(dot);
		}
		static bool IsPathValid(const wstring& path)
		{
			if (path.size() == 0) {
				return false;
			}

			auto normalized = ItsPath::NormalizePath(path);
			if (normalized.rfind(L"\\\\?\\", 0) == 0) normalized.erase(0, 4);
			if (normalized.empty()) return false;
			if (normalized.size() >= 2 && normalized[1] == L':') {
				const wchar_t drive = normalized[0];
				if (!((drive >= L'A' && drive <= L'Z') || (drive >= L'a' && drive <= L'z'))) {
					return false;
				}
				normalized.erase(0, 2);
			}
			const auto invalid = ItsPath::GetInvalidFilenameCharacters();
			return std::all_of(normalized.begin(), normalized.end(), [&](wchar_t ch) {
				return ch == ItsPath::PathSeparator ||
					std::find(invalid.begin(), invalid.end(), ch) == invalid.end();
				});
		}
		static bool HasExtension(const wstring& path, const wstring& extension)
		{
			if (path.size() == 0) {
				return false;
			}

			wstring ext = ItsPath::GetExtension(path);
			if (ext.size() == 0) {
				return false;
			}

			return (ext == extension);
		}
		static wstring ChangeExtension(wstring path, const wstring& newExtension)
		{
			if (path.size() == 0) {
				return wstring(L"");
			}

			if (newExtension.size() == 0) {
				return path;
			}

			path = ItsPath::NormalizePath(path);

			if (path[path.size() - 1] == ItsPath::PathSeparator) {
				return path;
			}

			wstring ext = ItsPath::GetExtension(path);
			if (ext.size() == 0) {
				if (newExtension[0] == ItsPath::ExtensionSeparator) {
					return path + newExtension;
				}
				else {
					return path + ItsPath::ExtensionSeparator + newExtension;
				}
			}

			auto pe = path.rfind(ext);
			if (pe == wstring::npos) {
				return path;
			}

			wstring npath = path;
			const auto extension = newExtension.front() == ItsPath::ExtensionSeparator
				? newExtension : ItsPath::ExtensionSeparator + newExtension;
			return npath.replace(pe, path.size() - pe, extension);
		}

		static wstring GetParentDirectory(wstring path)
		{
			if (path.size() == 0) {
				return wstring(L"");
			}

			path = ItsPath::NormalizePath(path);
			path = ItsPath::GetDirectory(path);

			size_t pos1 = path.rfind(ItsPath::PathSeparator);
			if (pos1 == wstring::npos) {
				return wstring(L"");
			}

			size_t pos2 = pos1;
			if (pos1 != 0 && pos1 == (path.size() - 1)) {
				pos2 = path.rfind(ItsPath::PathSeparator, pos1 - 1);
				if (pos2 == wstring::npos) {
					pos2 = pos1;
				}
			}

			return path.substr(0, pos2 + 1);
		}
		static wstring GetUserDocumentsPath()
		{
			PWSTR path = nullptr;
			std::wstring result;

			HRESULT hr = SHGetKnownFolderPath(
				FOLDERID_Documents,
				0,
				nullptr,
				&path
			);

			if (SUCCEEDED(hr))
			{
				result = path;
			}

			CoTaskMemFree(path);

			return result;
		}
		static wstring GetProgramFilesPath()
		{
			PWSTR path = nullptr;
			std::wstring result;

			HRESULT hr = SHGetKnownFolderPath(
				FOLDERID_ProgramFiles,
				0,
				nullptr,
				&path
			);

			if (SUCCEEDED(hr))
			{
				result = path;
			}

			CoTaskMemFree(path);

			return result;
		}
	};

	//
	// struct: ItsError
	//
	// (i): System error description generation.
	//
	struct ItsError
	{
		//
		// Get GetLastError() Error Description
		//
		static wstring GetLastErrorDescription()
		{
			return GetErrorDescription(GetLastError());
			return std::wstring();
		}// inline CComBSTR GetLastErrorDesc( )

			//
			// Get Error Description of Given Error Code
			//
		static wstring GetErrorDescription(DWORD dwError)
		{
			wchar_t* message = nullptr;
			const DWORD count = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr, dwError, 0, reinterpret_cast<wchar_t*>(&message), 0, nullptr);
			const std::unique_ptr<wchar_t, decltype(&::LocalFree)> buffer(message, ::LocalFree);
			return count == 0 ? wstring{} : wstring(buffer.get(), count);
		}// inline CComBSTR GetLastErrorDesc( )

		static wstring GetHResultErrorInfo(HRESULT hr)
		{
			return GetErrorDescription(static_cast<DWORD>(hr));
		}

		//
		// Get COM IErrorInfo Error Description
		//
		static wstring GetCoLastErrorInfoDescription()
		{
			IErrorInfo* pErrInfo = nullptr;
			HRESULT hr = GetErrorInfo(NULL, static_cast<IErrorInfo**>(&pErrInfo));
			if (FAILED(hr)) {
				return std::wstring();
			}

			if (pErrInfo == nullptr) {
				return std::wstring();
			}

			const auto release = [](IErrorInfo* info) { info->Release(); };
			const std::unique_ptr<IErrorInfo, decltype(release)> info(pErrInfo, release);
			BSTR description = nullptr;
			hr = info->GetDescription(&description);
			const std::unique_ptr<wchar_t, decltype(&::SysFreeString)> text(description, ::SysFreeString);
			return FAILED(hr) || !text ? wstring{} : wstring(text.get(), SysStringLen(text.get()));
		}

		/// <summary>
		/// Gets the error description for a standard exception.
		/// </summary>
		/// <param name="e"></param>
		/// <returns></returns>
		static wstring GetStdExceptionErrorInfo(const std::exception& e)
		{

			auto Utf8ToWide = [](std::string_view text) -> wstring
				{
					if (text.empty())
						return {};

					int size = MultiByteToWideChar(
						CP_UTF8,
						0,
						text.data(),
						static_cast<int>(text.size()),
						nullptr,
						0
					);

					if (size == 0)
						return {};

					std::wstring result(size, L'\0');

					MultiByteToWideChar(
						CP_UTF8,
						0,
						text.data(),
						static_cast<int>(text.size()),
						result.data(),
						size
					);

					return result;
				};

			string msg = e.what();
			return Utf8ToWide(msg);
		}// inline CComBSTR GetLastErrorDesc( )}
	};

	//
	// class: unique_handle
	//
	// (i): Wrapper for many types of Windows handles.
	//
	template<typename Traits>
	class unique_handle
	{
	private:
		typedef typename Traits::pointer pointer;

		pointer m_value;

		bool HasValue() const noexcept
		{
			if constexpr (std::is_pointer_v<pointer>) {
				if (m_value == nullptr) return false;
			}
			return m_value != Traits::invalid();
		}

	protected:
	public:

		explicit unique_handle(pointer value = Traits::invalid()) noexcept
			: m_value{ value }
		{

		}

		~unique_handle() noexcept
		{
			close();
		}

		auto close() noexcept -> void
		{
			if (*this)
			{
				Traits::close(m_value);
				m_value = Traits::invalid();
			}
		}

		pointer p()
		{
			return m_value;
		}

		pointer* GetAddressOf()
		{
			return &m_value;
		}

		//
		// bool
		//
		explicit operator bool() const noexcept
		{
			return HasValue();
		}

		//
		// HANDLE
		//
		operator HANDLE() const noexcept
		{
			return static_cast<HANDLE>(m_value);
		}

		void operator=(HANDLE handle)
		{
			const auto next = static_cast<typename Traits::pointer>(handle);
			if (next != m_value) {
				close();
				m_value = next;
			}
		}

		//
		// HWND
		//
		operator HWND() const noexcept
		{
			return static_cast<HWND>(m_value);
		}

		void operator=(HWND handle)
		{
			const auto next = static_cast<typename Traits::pointer>(handle);
			if (next != m_value) {
				close();
				m_value = next;
			}
		}

		//
		// HBITMAP
		//
		operator HBITMAP() const noexcept
		{
			return static_cast<HBITMAP>(m_value);
		}

		void operator=(HBITMAP handle)
		{
			const auto next = static_cast<typename Traits::pointer>(handle);
			if (next != m_value) {
				close();
				m_value = next;
			}
		}

		//
		// HGDIOBJ
		//
		//operator HGDIOBJ() const noexcept
		//{
		//	return static_cast<HGDIOBJ>(m_value);
		//}
		//
		//void operator=(HGDIOBJ handle)
		//{
		//	m_value = static_cast<Traits::pointer>(handle);
		//}

		//
		// HMENU
		//
		operator HMENU() const noexcept
		{
			return static_cast<HMENU>(m_value);
		}

		void operator=(HMENU handle)
		{
			const auto next = static_cast<typename Traits::pointer>(handle);
			if (next != m_value) {
				close();
				m_value = next;
			}
		}

		//
		// HIMAGELIST
		//
		operator HIMAGELIST() const noexcept
		{
			return static_cast<HIMAGELIST>(m_value);
		}

		void operator=(HIMAGELIST handle)
		{
			const auto next = static_cast<typename Traits::pointer>(handle);
			if (next != m_value) {
				close();
				m_value = next;
			}
		}

		//
		// HRGN
		//
		operator HRGN() const noexcept
		{
			return static_cast<HRGN>(m_value);
		}

		void operator=(HRGN handle)
		{
			const auto next = static_cast<typename Traits::pointer>(handle);
			if (next != m_value) {
				close();
				m_value = next;
			}
		}

		//
		// HBRUSH
		//
		operator HBRUSH() const noexcept
		{
			return static_cast<HBRUSH>(m_value);
		}

		void operator=(HBRUSH handle)
		{
			const auto next = static_cast<typename Traits::pointer>(handle);
			if (next != m_value) {
				close();
				m_value = next;
			}
		}

		//
		// HFONT
		//
		operator HFONT() const noexcept
		{
			return static_cast<HFONT>(m_value);
		}

		void operator=(HFONT handle)
		{
			const auto next = static_cast<typename Traits::pointer>(handle);
			if (next != m_value) {
				close();
				m_value = next;
			}
		}

		//
		// HICON
		//
		operator HICON() const noexcept
		{
			return static_cast<HICON>(m_value);
		}

		void operator=(HICON handle)
		{
			const auto next = static_cast<typename Traits::pointer>(handle);
			if (next != m_value) {
				close();
				m_value = next;
			}
		}

		//
		// IsInvalid
		//
		bool IsInvalid()
		{
			return !HasValue();
		}

		//
		// IsValid
		//
		bool IsValid()
		{
			return HasValue();
		}
	};

	//
	// HANDLE traits
	//
	struct handle_handle_traits
	{
		typedef HANDLE pointer;
		static auto invalid() noexcept -> pointer
		{
			return INVALID_HANDLE_VALUE;
		}

		static auto close(pointer value) noexcept -> void
		{
			CloseHandle(value);
		}
	};


	//
	// FINDHANDLE traits
	//
	struct handle_findhandle_traits
	{
		typedef HANDLE pointer;
		static auto invalid() noexcept -> pointer
		{
			return nullptr;
		}

		static auto close(pointer value) noexcept -> void
		{
			::FindClose(value);
		}
	};

	//
	// FindChangeNotification HANDLE traits
	//
	struct handle_findchangehandle_traits
	{
		typedef HANDLE pointer;
		static auto invalid() noexcept -> pointer
		{
			return INVALID_HANDLE_VALUE;
		}

		static auto close(pointer value) noexcept -> void
		{
			::FindCloseChangeNotification(value);
		}
	};

	//
	// HWND traits
	//
	struct hwnd_handle_traits
	{
		typedef HWND pointer;
		static auto invalid() noexcept -> pointer
		{
			return nullptr;
		}

		static auto close(pointer value) noexcept -> void
		{
			if (value != invalid())
			{
				DestroyWindow(value);
			}
		}
	};

	//
	// HBITMAP traits
	//
	struct hbitmap_handle_traits
	{
		typedef HBITMAP pointer;
		static auto invalid() noexcept -> pointer
		{
			return nullptr;
		}

		static auto close(pointer value) noexcept -> void
		{
			if (value != invalid())
			{
				DeleteObject(value);
			}
		}
	};

	//
	// HMENU traits
	//
	struct hmenu_handle_traits
	{
		typedef HMENU pointer;
		static auto invalid() noexcept -> pointer
		{
			return nullptr;
		}

		static auto close(pointer value) noexcept -> void
		{
			if (value != invalid())
			{
				::DestroyMenu(value);
			}
		}
	};

	//
	// HIMAGELIST traits
	//
	struct himagelist_handle_traits
	{
		typedef HIMAGELIST pointer;
		static auto invalid() noexcept -> pointer
		{
			return nullptr;
		}

		static auto close(pointer value) noexcept -> void
		{
			if (value != invalid())
			{
				::ImageList_Destroy(value);
			}
		}
	};

	//
	// HRGN traits
	//
	struct hrgn_handle_traits
	{
		typedef HRGN pointer;
		static auto invalid() noexcept -> pointer
		{
			return nullptr;
		}

		static auto close(pointer value) noexcept -> void
		{
			if (value != invalid())
			{
				::DeleteObject(value);
			}
		}
	};

	//
	// HBRUSH traits
	//
	struct hbrush_handle_traits
	{
		typedef HBRUSH pointer;
		static auto invalid() noexcept -> pointer
		{
			return nullptr;
		}

		static auto close(pointer value) noexcept -> void
		{
			if (value != invalid())
			{
				::DeleteObject(value);
			}
		}
	};

	//
	// HFONT traits
	//
	struct hfont_handle_traits
	{
		typedef HFONT pointer;
		static auto invalid() noexcept -> pointer
		{
			return nullptr;
		}

		static auto close(pointer value) noexcept -> void
		{
			if (value != invalid())
			{
				::DeleteObject(value);
			}
		}
	};

	//
	// HICON traits
	//
	struct hicon_handle_traits
	{
		typedef HICON pointer;
		static auto invalid() noexcept -> pointer
		{
			return nullptr;
		}

		static auto close(pointer value) noexcept -> void
		{
			if (value != invalid())
			{
				::DestroyIcon(value);
			}
		}
	};

	//
	// unique_xxx_handle typedefs
	//
	typedef unique_handle<handle_handle_traits> unique_handle_handle;
	typedef unique_handle<handle_findhandle_traits> unique_findhandle_handle;
	typedef unique_handle<handle_findchangehandle_traits> unique_findchangehandle_handle;
	typedef unique_handle<hwnd_handle_traits> unique_hwnd_handle;
	typedef unique_handle<hbitmap_handle_traits> unique_hbitmap_handle;
	typedef unique_handle<hmenu_handle_traits> unique_hmenu_handle;
	typedef unique_handle<himagelist_handle_traits> unique_himagelist_handle;
	typedef unique_handle<hrgn_handle_traits> unique_hrgn_handle;
	typedef unique_handle<hbrush_handle_traits> unique_hbrush_handle;
	typedef unique_handle<hfont_handle_traits> unique_hfont_handle;
	typedef unique_handle<hicon_handle_traits> unique_hicon_handle;

	//
	// struct: ItsDirectory
	// 
	// (i): Windows directory routines.
	//
	struct ItsDirectory
	{
	public:
		static bool Exists(wstring directory)
		{
			DWORD fileAttributes = GetFileAttributesW(directory.c_str());
			return (fileAttributes != INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY));
		}
		static bool CreateDirectory(wstring path)
		{
			return ::CreateDirectoryW(path.c_str(), nullptr);
		}
		static bool RemoveDirectory(wstring path)
		{
			return ::RemoveDirectoryW(path.c_str());
		}
		static bool SetCurrentDirectory(wstring path)
		{
			return ::SetCurrentDirectoryW(path.c_str());
		}
		static vector<wstring> GetDirectories(wstring path) {
			if (path.size() == 0) {
				return vector<wstring>();
			}

			path = ItsPath::NormalizePath(path);
			if (path[path.size() - 1] != ItsPath::PathSeparator)
			{
				if (path[path.size() - 1] != L'*') {
					path += ItsPath::PathSeparator;
					path += L"*";
				}
			}
			else {
				path += L'*';
			}

			WIN32_FIND_DATAW wfd{ 0 };
			vector<wstring> dirs;
			unique_findhandle_handle h(::FindFirstFileW(path.c_str(), &wfd));
			if (h.IsValid()) {
				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if (wcscmp(wfd.cFileName, L".") != 0 &&
						wcscmp(wfd.cFileName, L"..") != 0)
					{
						dirs.push_back(wfd.cFileName);
					}
				}
				while (::FindNextFileW(h, &wfd)) {
					if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						if (wcscmp(wfd.cFileName, L".") != 0 &&
							wcscmp(wfd.cFileName, L"..") != 0)
						{
							dirs.push_back(wfd.cFileName);
						}
					}
				}
			}

			return dirs;
		}
		static vector<wstring> GetFiles(wstring path) {
			if (path.size() == 0) {
				return vector<wstring>();
			}

			path = ItsPath::NormalizePath(path);
			if (path[path.size() - 1] != ItsPath::PathSeparator)
			{
				if (path[path.size() - 1] != L'*') {
					path += ItsPath::PathSeparator;
					path += L"*";
				}
			}
			else {
				path += L'*';
			}

			WIN32_FIND_DATAW wfd{ 0 };
			vector<wstring> dirs;
			unique_findhandle_handle h(::FindFirstFileW(path.c_str(), &wfd));
			if (h.IsValid()) {
				if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					dirs.push_back(wfd.cFileName);
				}
				while (::FindNextFileW(h, &wfd)) {
					if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						dirs.push_back(wfd.cFileName);
					}
				}
			}

			return dirs;
		}
		static vector<wchar_t> GetLogicalDrives()
		{
			const wchar_t wszDrives[27] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
			vector<wchar_t> d;

			DWORD dw = ::GetLogicalDrives();

			for (DWORD i = 0; i < 26; i++) {
				DWORD dwBit = DWORD{ 1 } << i;
				DWORD dwResult = dw & dwBit;
				if (dwResult) {
					d.push_back(wszDrives[i]);
				}
			}

			return d;
		}
	};

	//
	// class: ItsEvent
	//
	// (i): Windows event wrapper.
	//
	class ItsEvent
	{
	private:
		unique_handle_handle m_handle;
	protected:
	public:
		ItsEvent()
		{
			this->m_handle = CreateEventW(NULL, TRUE, FALSE, NULL);
		}

		explicit ItsEvent(bool signaled)
		{
			this->m_handle = CreateEventW(NULL, TRUE, (signaled) ? TRUE : FALSE, NULL);
		}

		ItsEvent(ItsEvent const&) = delete;
		auto operator=(ItsEvent const&)->ItsEvent & = delete;

		operator HANDLE() noexcept
		{
			return this->m_handle.p();
		}

		void Clear()
		{
			ResetEvent(this->m_handle.p());
		}

		void Signal()
		{
			SetEvent(this->m_handle.p());
		}

		bool Wait(DWORD milliseconds = INFINITE)
		{
			auto result = WaitForSingleObject(this->m_handle.p(), milliseconds);
			return result == WAIT_OBJECT_0;
		}
	};

	//
	// enum: ItsFileOpen
	//
	enum class ItsFileOpenCreation
	{
		CreateAlways,
		CreateNew,
		OpenAlways,
		OpenExisting,
		TruncateExisting
	};

	//
	// enum: ItsFilePosition
	//
	enum class ItsFilePosition
	{
		FileBegin,
		FileCurrent,
		FileEnd
	};

	//
	// enum: ItsFileTextType
	//
	enum class ItsFileTextType
	{
		Ansi,
		UTF8NoBOM,
		UTF8WithBOM,
		Unicode
	};

	//
	// class: ItsFileBase
	//
	// (i): ItsFile and ItsTextFile base class.
	//
	class ItsFileBase
	{
	private:
	protected:
		unique_handle_handle m_handle;

		//
		// Method: GetFileSize
		//
		bool GetFileSize(size_t* size)
		{
			LARGE_INTEGER sizeValue{};
			if (!size || !GetFileSizeEx(this->m_handle.p(), &sizeValue) || sizeValue.QuadPart < 0 ||
				static_cast<ULONGLONG>(sizeValue.QuadPart) >(std::numeric_limits<size_t>::max)()) return false;
			*size = static_cast<size_t>(sizeValue.QuadPart);
			return true;
		}
	public:

		//
		// Method: Read
		//
		bool Read(void* pBuffer, DWORD dwBytesToRead, DWORD* dwBytesRead)
		{
			if (this->m_handle.IsInvalid())
			{
				return false;
			}

			return (bool)ReadFile(this->m_handle.p(), pBuffer, dwBytesToRead, dwBytesRead, NULL);
		}

		//
		// Method: Write
		//
		bool Write(const void* pBuffer, DWORD dwBytesToWrite, DWORD* dwBytesWritten)
		{
			if (this->m_handle.IsInvalid())
			{
				return false;
			}

			return (bool)WriteFile(this->m_handle.p(), pBuffer, dwBytesToWrite, dwBytesWritten, NULL);
		}

		//
		// Method: Close
		//
		bool Close()
		{
			if (this->m_handle.IsInvalid())
			{
				return false;
			}

			this->m_handle.close();

			return true;
		}

		//
		// Method: GetFilePosition
		//
		bool GetFilePosition(size_t* position)
		{
			if (this->m_handle.IsInvalid())
			{
				return false;
			}

			LARGE_INTEGER zero{ 0 };
			LARGE_INTEGER pos{ 0 };
			if (!position || !SetFilePointerEx(this->m_handle.p(), zero, &pos, FILE_CURRENT) ||
				pos.QuadPart < 0 || static_cast<ULONGLONG>(pos.QuadPart) >(std::numeric_limits<size_t>::max)()) return false;
			*position = static_cast<size_t>(pos.QuadPart);
			return true;
		}

		//
		// Method: SetFilePosition
		//
		bool SetFilePosition(size_t distanceToMove, ItsFilePosition position)
		{
			if (this->m_handle.IsInvalid())
			{
				return false;
			}

			DWORD dwPosition = 0;
			if (position == ItsFilePosition::FileBegin)
			{
				dwPosition = FILE_BEGIN;
			}
			else if (position == ItsFilePosition::FileCurrent)
			{
				dwPosition = FILE_CURRENT;
			}
			else if (position == ItsFilePosition::FileEnd)
			{
				dwPosition = FILE_END;
			}

			else {
				return false;
			}
			if (distanceToMove > static_cast<ULONGLONG>((std::numeric_limits<LONGLONG>::max)())) return false;
			LARGE_INTEGER distance{};
			distance.QuadPart = static_cast<LONGLONG>(distanceToMove);
			return (bool)SetFilePointerEx(this->m_handle.p(), distance, NULL, dwPosition);
		}

		operator HANDLE() const
		{
			return this->m_handle.operator HANDLE();
		}

		bool IsValid()
		{
			return this->m_handle.IsValid();
		}

		bool IsInvalid()
		{
			return this->m_handle.IsInvalid();
		}

		bool SetFileSize(size_t pos)
		{
			if (!this->IsValid())
			{
				return false;
			}
			if (!this->SetFilePosition(pos, ItsFilePosition::FileBegin)) return false;
			return (bool)SetEndOfFile(this->m_handle.p());
		}
	};

	//
	// class: ItsFile
	//
	// (i): Wrapper for a file on Windows.
	//
	class ItsFile : public ItsFileBase
	{
	private:
	protected:
	public:

		//
		// Constructor
		//
		ItsFile()
		{}

		//
		// Method: OpenOrCreate
		//
		bool OpenOrCreate(const wstring& filename, const wstring& accessReadWrite, const wstring& shareReadWrite, ItsFileOpenCreation flagCreation)
		{
			if (this->m_handle.IsValid())
			{
				return false;
			}

			DWORD dwAccess = 0;
			if (accessReadWrite.size() != 0)
			{
				if (accessReadWrite.find(L"r") != wstring::npos) {
					dwAccess |= GENERIC_READ;
				}
				if (accessReadWrite.find(L"w") != wstring::npos) {
					dwAccess |= GENERIC_WRITE;
				}
			}

			DWORD dwShare = 0;
			if (shareReadWrite.size() != 0)
			{
				if (shareReadWrite.find(L"r") != wstring::npos) {
					dwShare |= FILE_SHARE_READ;
				}
				if (shareReadWrite.find(L"w") != wstring::npos) {
					dwShare |= FILE_SHARE_WRITE;
				}
				if (shareReadWrite.find(L"d") != wstring::npos) {
					dwShare |= FILE_SHARE_DELETE;
				}
			}

			DWORD dwFlags = 0;
			if (flagCreation == ItsFileOpenCreation::CreateAlways) {
				dwFlags = CREATE_ALWAYS;
			}
			else if (flagCreation == ItsFileOpenCreation::CreateNew) {
				dwFlags = CREATE_NEW;
			}
			else if (flagCreation == ItsFileOpenCreation::OpenAlways) {
				dwFlags = OPEN_ALWAYS;
			}
			else if (flagCreation == ItsFileOpenCreation::OpenExisting) {
				dwFlags = OPEN_EXISTING;
			}
			else if (flagCreation == ItsFileOpenCreation::TruncateExisting) {
				dwFlags = TRUNCATE_EXISTING;
			}

			this->m_handle = CreateFileW(filename.c_str(), dwAccess, dwShare, NULL, dwFlags, FILE_ATTRIBUTE_NORMAL, NULL);

			return this->m_handle.IsValid();
		}

		//
		// Method: GetFileSize
		//
		static bool GetFileSize(const wstring& filename, size_t* size)
		{
			ItsFile file;
			if (!file.OpenOrCreate(filename, L"r", L"r", ItsFileOpenCreation::OpenExisting))
			{
				return false;
			}

			return file.ItsFileBase::GetFileSize(size);
		}

		//
		// Method: Delete
		//
		static bool Delete(const wstring& filename)
		{
			return (bool)DeleteFileW(filename.c_str());
		}

		//
		// Method: Copy
		//
		static bool Copy(const wstring& existingFilename, const wstring& newFilename, bool failIfExists)
		{
			return (bool)CopyFileW(existingFilename.c_str(), newFilename.c_str(), (failIfExists) ? TRUE : FALSE);
		}

		//
		// Method: Move
		//
		static bool Move(const wstring& existingFilename, const wstring& newFilename, bool failIfExists)
		{
			return (bool)MoveFileExW(existingFilename.c_str(), newFilename.c_str(), MOVEFILE_COPY_ALLOWED | (failIfExists ? 0 : MOVEFILE_REPLACE_EXISTING));
		}

		//
		// Method: Exists
		//
		static bool Exists(const wstring& filename)
		{
			DWORD fileAttributes = GetFileAttributesW(filename.c_str());
			return (fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY));
		}

		//
		// Method: SetFileSize
		//
		static bool SetSize(const wstring& filename, size_t pos)
		{
			ItsFile file;
			if (!file.OpenOrCreate(filename, L"rw", L"", ItsFileOpenCreation::OpenExisting)) {
				return false;
			}
			return file.SetFileSize(pos);
		}

		static bool Shred(const wstring& filename, bool alsoDelete)
		{
			ItsFile f;
			if (!f.OpenOrCreate(filename, L"rw", L"", ItsFileOpenCreation::OpenExisting)) return false;
			size_t fileSize = 0;
			if (!f.ItsFileBase::GetFileSize(&fileSize)) return false;

			const uint32_t bufferSize = 2048;
			DWORD bytesWritten = 0;
			size_t totalWritten = 0;
			const unique_ptr<uint8_t[]> pdata = make_unique<uint8_t[]>(bufferSize);
			for (uint32_t i = 0; i < bufferSize; i++) {
				pdata[i] = 0xFF;
			}
			while (totalWritten < fileSize) {
				const DWORD count = static_cast<DWORD>((std::min)(fileSize - totalWritten, size_t{ bufferSize }));
				if (!f.Write(pdata.get(), count, &bytesWritten) || bytesWritten == 0) return false;
				totalWritten += bytesWritten;
			}
			if (!FlushFileBuffers(f.m_handle.p())) return false;
			f.Close();

			if (alsoDelete)
			{
				return ItsFile::Delete(filename);
			}

			return true;
		}

		static bool ShredAndDelete(const wstring& filename)
		{
			return ItsFile::Shred(filename, true);
		}
	};

	//
	// class: ItsTextFile
	//
	// (i): Wrapper for text file on Windows.
	//
	class ItsTextFile : public ItsFileBase
	{
	private:
		ItsFileTextType m_textType = ItsFileTextType::Ansi;
		size_t m_bomSize = 0;

		bool WriteBytes(const void* data, size_t size)
		{
			auto bytes = static_cast<const BYTE*>(data);
			while (size != 0) {
				const DWORD count = static_cast<DWORD>((std::min)(size, size_t{ 65536 }));
				DWORD written = 0;
				if (!this->Write(bytes, count, &written) || written == 0) return false;
				bytes += written;
				size -= written;
			}
			return true;
		}

		//
		// Method: OpenOrCreate
		//
		bool OpenOrCreate(const wstring& filename, const wstring& accessReadWrite, const wstring& shareReadWrite, ItsFileOpenCreation flagCreation)
		{
			if (this->m_handle.IsValid())
			{
				return false;
			}

			DWORD dwAccess = 0;
			if (accessReadWrite.size() != 0)
			{
				if (accessReadWrite.find(L"r") != wstring::npos) {
					dwAccess |= GENERIC_READ;
				}
				if (accessReadWrite.find(L"w") != wstring::npos) {
					dwAccess |= GENERIC_WRITE;
				}
			}

			DWORD dwShare = 0;
			if (shareReadWrite.size() != 0)
			{
				if (shareReadWrite.find(L"r") != wstring::npos) {
					dwShare |= FILE_SHARE_READ;
				}
				if (shareReadWrite.find(L"w") != wstring::npos) {
					dwShare |= FILE_SHARE_WRITE;
				}
				if (shareReadWrite.find(L"d") != wstring::npos) {
					dwShare |= FILE_SHARE_DELETE;
				}
			}

			DWORD dwFlags = 0;
			if (flagCreation == ItsFileOpenCreation::CreateAlways) {
				dwFlags = CREATE_ALWAYS;
			}
			else if (flagCreation == ItsFileOpenCreation::CreateNew) {
				dwFlags = CREATE_NEW;
			}
			else if (flagCreation == ItsFileOpenCreation::OpenAlways) {
				dwFlags = OPEN_ALWAYS;
			}
			else if (flagCreation == ItsFileOpenCreation::OpenExisting) {
				dwFlags = OPEN_EXISTING;
			}
			else if (flagCreation == ItsFileOpenCreation::TruncateExisting) {
				dwFlags = TRUNCATE_EXISTING;
			}

			this->m_handle = CreateFileW(filename.c_str(), dwAccess, dwShare, NULL, dwFlags, FILE_ATTRIBUTE_NORMAL, NULL);

			return this->m_handle.IsValid();
		}

	protected:
	public:
		inline static constexpr const wchar_t* LineDelimiterWindows = L"\r\n";
		inline static constexpr const wchar_t* LineDelimiterUnix = L"\n";
		inline static const vector<BYTE> ByteOrderMarkUnicode = { 0xFF, 0xFE };
		inline static const vector<BYTE> ByteOrderMarkUTF8 = { 0xEF, 0xBB, 0xBF };

		//
		// Constructor
		//
		ItsTextFile()
		{}

		//
		// Method: OpenOrCreateText
		//
		bool OpenOrCreateText(const wstring& filename, const wstring& accessReadWrite, const wstring& shareReadWrite, ItsFileOpenCreation flagCreation, ItsFileTextType type)
		{
			if (this->m_handle.IsValid())
			{
				return false;
			}

			if (type != ItsFileTextType::Ansi && type != ItsFileTextType::Unicode &&
				type != ItsFileTextType::UTF8WithBOM && type != ItsFileTextType::UTF8NoBOM) return false;
			if (!this->OpenOrCreate(filename, accessReadWrite, shareReadWrite, flagCreation)) return false;

			const auto fail = [&]() { this->Close(); return false; };
			size_t size = 0;
			if (!this->GetFileSize(&size)) return fail();

			m_textType = type;
			m_bomSize = 0;
			const bool canRead = accessReadWrite.find(L'r') != wstring::npos;
			const bool canWrite = accessReadWrite.find(L'w') != wstring::npos;
			if (size != 0 && canRead) {
				BYTE bom[3]{};
				DWORD count = 0;
				if (!this->Read(bom, sizeof(bom), &count)) return fail();
				if (count >= 2 && bom[0] == 0xFF && bom[1] == 0xFE) {
					m_textType = ItsFileTextType::Unicode;
					m_bomSize = 2;
				}
				else if (count == 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
					m_textType = ItsFileTextType::UTF8WithBOM;
					m_bomSize = 3;
				}
				if (!this->SetFilePosition(m_bomSize, ItsFilePosition::FileBegin)) return fail();
			}
			else if (size == 0 && canWrite) {
				const vector<BYTE>* bom = nullptr;
				if (type == ItsFileTextType::Unicode) bom = &ByteOrderMarkUnicode;
				if (type == ItsFileTextType::UTF8WithBOM) bom = &ByteOrderMarkUTF8;
				if (bom != nullptr) {
					if (!WriteBytes(bom->data(), bom->size())) return fail();
					m_bomSize = bom->size();
				}
			}
			return true;
		}

		//
		// Function: CreateTextFile
		// 
		// (i): Creates a new text file (with BOM) but otherwise empty and ready to
		// append/write text to.
		//
		static bool CreateTextFile(const wstring& filename, ItsFileTextType type)
		{
			ItsTextFile file{};
			return file.OpenOrCreateText(filename, L"rw", L"rw", ItsFileOpenCreation::CreateNew, type);
		}

		//
		// Method: ReadTextAllLines
		//
		bool ReadTextAllLines(vector<wstring>& lines, const wstring& lineDelimiter = L"\r\n")
		{
			if (lineDelimiter.empty()) return false;
			wstring text;
			if (!ReadTextAll(text)) return false;
			if (text.empty()) lines.clear();
			else lines = ItsString::Split(text, lineDelimiter);
			return true;
		}

		//
		// Function: ReadTextAllLines
		//
		// (i): Reads all text from a text file.
		//
		static bool ReadTextAllLines(const wstring& filename, ItsFileTextType textType, vector<wstring>& out, const wstring& lineDelimiter = ItsTextFile::LineDelimiterWindows)
		{
			if (!ItsFile::Exists(filename)) {
				return false;
			}

			ItsTextFile file{};
			if (!file.OpenOrCreateText(filename, L"r", L"rw", ItsFileOpenCreation::OpenExisting, textType)) {
				return false;
			}

			if (!file.ReadTextAllLines(out, lineDelimiter)) {
				file.Close();
				return false;
			}

			file.Close();
			return true;
		}

		//
		// Method: ReadTextAll
		//
		bool ReadTextAll(wstring& out)
		{
			size_t size = 0;
			if (!this->GetFileSize(&size) || size < m_bomSize ||
				!this->SetFilePosition(m_bomSize, ItsFilePosition::FileBegin)) return false;

			vector<char> bytes(size - m_bomSize);
			size_t offset = 0;
			while (offset < bytes.size()) {
				const DWORD requested = static_cast<DWORD>((std::min)(bytes.size() - offset, size_t{ 65536 }));
				DWORD count = 0;
				if (!this->Read(bytes.data() + offset, requested, &count) || count == 0) return false;
				offset += count;
			}

			wstring text;
			if (m_textType == ItsFileTextType::Unicode) {
				if (bytes.size() % sizeof(wchar_t) != 0) return false;
				text.resize(bytes.size() / sizeof(wchar_t));
				if (!bytes.empty()) std::memcpy(text.data(), bytes.data(), bytes.size());
			}
			else if (m_textType == ItsFileTextType::Ansi) {
				// Preserve the existing one-byte text representation.
				text.assign(bytes.begin(), bytes.end());
			}
			else if (!bytes.empty()) {
				if (bytes.size() > static_cast<size_t>((std::numeric_limits<int>::max)())) return false;
				const int sizeBytes = static_cast<int>(bytes.size());
				const int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes.data(), sizeBytes, nullptr, 0);
				if (count == 0) return false;
				text.resize(count);
				if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes.data(), sizeBytes, text.data(), count) != count) return false;
			}
			out.swap(text);
			return true;
		}

		//
		// Function: ReadTextAll
		//
		// (i): Reads all text from a text file.
		//
		static bool ReadTextAll(const wstring& filename, ItsFileTextType textType, wstring& out)
		{
			if (!ItsFile::Exists(filename)) {
				return false;
			}

			ItsTextFile file{};
			if (!file.OpenOrCreateText(filename, L"r", L"rw", ItsFileOpenCreation::OpenExisting, textType)) {
				return false;
			}

			if (!file.ReadTextAll(out)) {
				file.Close();
				return false;
			}

			file.Close();
			return true;
		}

		//
		// Method: WriteText
		//
		bool WriteText(const string& text)
		{
			wstring txt(text.begin(), text.end());
			return this->WriteText(txt);
		}

		//
		// Method: WriteText
		//
		bool WriteText(const wstring& text)
		{
			if (this->m_handle.IsInvalid())
			{
				return false;
			}

			if (text.empty()) return true;
			if (m_textType == ItsFileTextType::Ansi) {
				const string bytes(text.begin(), text.end());
				return WriteBytes(bytes.data(), bytes.size());
			}
			if (m_textType == ItsFileTextType::Unicode) {
				return WriteBytes(text.data(), text.size() * sizeof(wchar_t));
			}
			if (text.size() > static_cast<size_t>((std::numeric_limits<int>::max)())) return false;
			const int length = static_cast<int>(text.size());
			const int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text.data(), length,
				nullptr, 0, nullptr, nullptr);
			if (size == 0) return false;
			vector<char> bytes(size);
			return WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text.data(), length,
				bytes.data(), size, nullptr, nullptr) == size && WriteBytes(bytes.data(), bytes.size());
		}

		//
		// Function: ReadTextAllLines
		//
		// (i): Reads all text from a text file.
		//
		static bool AppendText(const wstring& filename, ItsFileTextType textType, const wstring& textToWrite)
		{
			if (!ItsFile::Exists(filename)) {
				return false;
			}

			ItsTextFile file{};
			if (!file.OpenOrCreateText(filename, L"rw", L"rw", ItsFileOpenCreation::OpenExisting, textType)) {
				return false;
			}

			if (!file.SetFilePosition(0, ItsFilePosition::FileEnd)) {
				file.Close();
				return false;
			}

			if (!file.WriteText(textToWrite)) {
				file.Close();
				return false;
			}

			file.Close();
			return true;
		}

		//
		// Method: WriteTextLine
		//
		bool WriteTextLine(const string& text, const string& lineDelimiter = "\r\n")
		{
			wstring txt(text.begin(), text.end());
			wstring dl(lineDelimiter.begin(), lineDelimiter.end());
			return this->WriteTextLine(txt, dl);
		}

		//
		// Method: WriteTextLine
		//
		bool WriteTextLine(const wstring& text, const wstring& lineDelimiter = L"\r\n")
		{
			if (this->m_handle.IsInvalid())
			{
				return false;
			}

			if (!this->WriteText(text))
			{
				return false;
			}

			if (!this->WriteText(lineDelimiter))
			{
				return false;
			}

			return true;
		}
	};

#pragma warning(disable: 26812)
	enum EREGCLASS
	{
		EHKEY_CLASSES_ROOT = 0,
		EHKEY_CURRENT_USER = 1,
		EHKEY_LOCAL_MACHINE = 2,
		EHKEY_USERS = 3,
		EHKEY_PERFORMANCE_DATA = 4,
		EHKEY_CURRENT_CONFIG = 5,
		EHKEY_DYN_DATA = 6
	};
#pragma warning(error: 26812)
	//
	// class: ItsRegistry
	//
	// (i): Windows registry routines.
	//
	class ItsRegistry
	{
	private:
		//
		// Function: CheckHKey
		//
		// (i)	Checks to see wheather or not a valid REGCLASS value has been
		//		passed. If so the function returnes true, else it returnes false.
		//
		static bool CheckHKey(EREGCLASS a_regclass, HKEY& a_hKey)
		{
			switch (a_regclass)
			{
			case EREGCLASS(0):
				a_hKey = HKEY_CLASSES_ROOT;
				break;
			case EREGCLASS(1):
				a_hKey = HKEY_CURRENT_USER;
				break;
			case EREGCLASS(2):
				a_hKey = HKEY_LOCAL_MACHINE;
				break;
			case EREGCLASS(3):
				a_hKey = HKEY_USERS;
				break;
			case EREGCLASS(4):
				a_hKey = HKEY_PERFORMANCE_DATA;
				break;
			case EREGCLASS(5):
				a_hKey = HKEY_CURRENT_CONFIG;
				break;
			case EREGCLASS(6):
				a_hKey = HKEY_DYN_DATA;
				break;
			default:
				return false;
				break;
			};

			return true;
		}// bool CSettings::CheckHKey( long a_lhKey, HKEY &a_hKey )
	public:
		//
		// Function: EnumerateKeys
		//
		static bool EnumerateKeys(EREGCLASS eregclass, wstring path, vector<wstring>* result)
		{
			HKEY hKey;
			if (!CheckHKey(eregclass, hKey) || path.size() == 0 || result == nullptr)
			{
				return false;
			}

			HKEY hOpenKey;
			LONG lResult(0);
			lResult = RegOpenKeyExW(hKey, path.c_str(), 0, KEY_ENUMERATE_SUB_KEYS, &hOpenKey);
			if (lResult != ERROR_SUCCESS)
			{
				return false;
			}

			result->clear();

			long lIndex(0);
			ULONG cbAvailable(0);
			const auto close = std::unique_ptr<std::remove_pointer_t<HKEY>, decltype(&RegCloseKey)>(hOpenKey, RegCloseKey);
			wchar_t name[256]{};

			FILETIME ft;
			do
			{
				cbAvailable = static_cast<ULONG>(std::size(name));
				lResult = RegEnumKeyExW(hOpenKey, lIndex++, name, &cbAvailable, nullptr, nullptr, nullptr, &ft);

				if (lResult == ERROR_SUCCESS)
				{
					result->emplace_back(name, cbAvailable);
				}// if ( lResult == ERROR_SUCCESS ) {			

			} while (lResult == ERROR_SUCCESS);

			if (lResult != ERROR_NO_MORE_ITEMS) return false;

			return true;
		}

		//
		// Function: EnumerateValues
		//
		static bool EnumerateValues(EREGCLASS eregclass, wstring path, const wstring& key, vector<wstring>* result)
		{
			HKEY hKey;
			if (!CheckHKey(eregclass, hKey) || (path.size() == 0 && key.size() == 0) || result == nullptr)
			{
				return false;
			}

			if (path.size() > 0 && path[path.size() - 1] != L'\\')
			{
				path += L'\\';
			}
			path += key;

			HKEY hOpenKey;
			LONG lResult(0);
			lResult = RegOpenKeyExW(hKey, path.c_str(), 0, KEY_QUERY_VALUE, &hOpenKey);
			if (lResult != ERROR_SUCCESS)
			{
				return false;
			}

			result->clear();

			const auto close = std::unique_ptr<std::remove_pointer_t<HKEY>, decltype(&RegCloseKey)>(hOpenKey, RegCloseKey);
			DWORD lIndex = 0;
			ULONG cbAvailable = 0;
			vector<wchar_t> name(16384);
			do
			{
				cbAvailable = static_cast<ULONG>(name.size());
				lResult = RegEnumValueW(hOpenKey, lIndex++, name.data(), &cbAvailable, nullptr, nullptr, nullptr, nullptr);
				if (lResult == ERROR_SUCCESS)
				{
					result->emplace_back(name.data(), cbAvailable);
				}// if ( lResult == ERROR_SUCCESS ) {			
			} while (lResult == ERROR_SUCCESS);

			if (lResult != ERROR_NO_MORE_ITEMS) return false;

			return true;
		}

		//
		// Function: ReadValue
		//
		static bool ReadValue(EREGCLASS eregclass, wstring path, const wstring& key, wstring value, wstring* data)
		{
			HKEY hKey;
			if (!CheckHKey(eregclass, hKey) || (path.size() == 0 && key.size() == 0) || data == nullptr)
			{
				return false;
			}

			if (path.size() > 0 && path[path.size() - 1] != L'\\')
			{
				path += L'\\';
			}
			path += key;

			HKEY hOpenKey;
			LONG lResult(0);
			lResult = RegOpenKeyExW(hKey, path.c_str(), 0, KEY_READ, &hOpenKey);
			if (lResult != ERROR_SUCCESS)
			{
				return false;
			}

			const auto close = std::unique_ptr<std::remove_pointer_t<HKEY>, decltype(&RegCloseKey)>(hOpenKey, RegCloseKey);
			DWORD size = 0;
			LSTATUS status = RegGetValueW(hOpenKey, nullptr, value.c_str(), RRF_RT_REG_SZ, nullptr, nullptr, &size);
			while (status == ERROR_SUCCESS || status == ERROR_MORE_DATA) {
				if (size % sizeof(wchar_t) != 0) return false;
				vector<wchar_t> buffer(size / sizeof(wchar_t) + 1);
				DWORD actual = size;
				status = RegGetValueW(hOpenKey, nullptr, value.c_str(), RRF_RT_REG_SZ, nullptr, buffer.data(), &actual);
				if (status == ERROR_SUCCESS) {
					*data = buffer.data();
					return true;
				}
				if (status != ERROR_MORE_DATA || actual <= size) return false;
				size = actual;
			}
			return false;
		}

		//
		// Function: DeleteKey
		//
		static bool DeleteKey(EREGCLASS eregclass, wstring path, const wstring& key)
		{
			HKEY hKey;
			if (!CheckHKey(eregclass, hKey) || (path.size() == 0 && key.size() == 0))
			{
				return false;
			}

			if (path.size() > 0 && path[path.size() - 1] != L'\\')
			{
				path += L'\\';
			}
			path += key;

			DWORD dwResult(0);
			dwResult = ::SHDeleteKeyW(hKey, path.c_str());
			if (dwResult != ERROR_SUCCESS) {
				if (GetLastError() != 0) {
					return false;
				}
				else {
					return false;
				}
			}

			return true;
		}

		//
		// Function: CreateKey
		//
		static bool CreateKey(EREGCLASS eregclass, wstring path, const wstring& key, wstring default_data)
		{
			HKEY hKey;
			if (!CheckHKey(eregclass, hKey) || (path.size() == 0 && key.size() == 0))
			{
				return false;
			}

			path.erase(0, path.find_first_not_of(L'\\'));

			if (path.length() > 0) {
				if (path.at(path.length() - 1) != L'\\') {
					path += L'\\';
				}
			}

			path += key;

			// Create new key
			HKEY hNewKey;
			LONG lResult = RegCreateKeyExW(hKey, path.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hNewKey, NULL);
			if (lResult != ERROR_SUCCESS) {
				return false;
			}

			// Set default value.	
			if (default_data.size() > 0) {
				lResult = RegSetValueExW(hNewKey, NULL, 0, REG_SZ, reinterpret_cast<CONST BYTE*>(const_cast<const wchar_t*>(default_data.data())), (static_cast<DWORD>(default_data.size()) * sizeof(wchar_t)) + 2);
				if (lResult != ERROR_SUCCESS) {
					RegCloseKey(hNewKey);	// Close New Key				
					return false;
				}
			}

			RegCloseKey(hNewKey);	// Close New Key

			return true;
		}

		//
		// Function: AddValue
		//
		static bool AddValue(EREGCLASS eregclass, wstring path, const wstring& key, wstring value, wstring data)
		{
			HKEY hKey;
			if (!CheckHKey(eregclass, hKey) || (path.size() == 0 && key.size() == 0) || value.size() == 0)
			{
				return false;
			}

			if (path.size() > 0 && path[path.size() - 1] != L'\\')
			{
				path += L'\\';
			}
			path += key;

			HKEY hOpenKey;
			LONG lResult(0);
			lResult = RegOpenKeyExW(hKey, path.c_str(), 0, KEY_ALL_ACCESS, &hOpenKey);
			if (lResult != ERROR_SUCCESS) {
				return false;
			}

			lResult = RegSetValueExW(hOpenKey, value.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(data.data()), (static_cast<DWORD>(data.size()) * sizeof(wchar_t)) + 2);
			if (lResult != ERROR_SUCCESS) {
				RegCloseKey(hOpenKey);
				return false;
			}

			RegCloseKey(hOpenKey);

			return true;
		}

		//
		// Function: DeleteValue
		//
		static bool DeleteValue(EREGCLASS eregclass, wstring path, const wstring& key, wstring value)
		{
			HKEY hKey;
			if (!CheckHKey(eregclass, hKey) || (path.size() == 0 && key.size() == 0) || value.size() == 0)
			{
				return false;
			}

			if (path.size() > 0 && path[path.size() - 1] != L'\\')
			{
				path += L'\\';
			}
			path += key;

			HKEY hOpenKey;
			LONG lResult(0);
			lResult = RegOpenKeyExW(hKey, path.c_str(), 0, KEY_ALL_ACCESS, &hOpenKey);
			if (lResult != ERROR_SUCCESS) {
				return false;
			}
			lResult = RegDeleteValueW(hOpenKey, value.c_str());
			if (lResult != ERROR_SUCCESS) {
				RegCloseKey(hOpenKey);
				return false;
			}

			RegCloseKey(hOpenKey);

			return true;
		}
	};// ItsRegistry

	//
	// enum: ItsFileMonitorMask
	//
	// (i): mask for ItsFileMonitor
	//
	enum ItsFileMonitorMask : uint32_t {
		ChangeFileName = FILE_NOTIFY_CHANGE_FILE_NAME,
		ChangeDirName = FILE_NOTIFY_CHANGE_DIR_NAME,
		ChangeAttributes = FILE_NOTIFY_CHANGE_ATTRIBUTES,
		ChangeSize = FILE_NOTIFY_CHANGE_SIZE,
		ChangeLastWrite = FILE_NOTIFY_CHANGE_LAST_WRITE,
		ChangeSecurity = FILE_NOTIFY_CHANGE_SECURITY,
		ChangeCreation = FILE_NOTIFY_CHANGE_CREATION
	};

	struct ItsFileMonitorEvent {
		DWORD         Action;
		LARGE_INTEGER CreationTime;
		LARGE_INTEGER LastModificationTime;
		LARGE_INTEGER LastChangeTime;
		LARGE_INTEGER LastAccessTime;
		LARGE_INTEGER FileSize;
		DWORD         FileAttributes;
		DWORD         ReparsePointTag;
		LARGE_INTEGER FileId;
		LARGE_INTEGER ParentFileId;
		wstring		  FileName;
	};

	//
	// class: ItsFileMonitor
	//
	// (i): Monitors a given file folder
	//
	class ItsFileMonitor
	{
	private:

		static constexpr DWORD NotificationBufferSize = 64 * 1024;
		uint32_t m_mask;
		thread m_thread;
		unique_handle_handle m_dirHandle;
		unique_handle_handle m_stopEvent;
		wstring m_pathname;
		unique_ptr<BYTE[]> m_pbuffer;
		bool m_bWatchSubTree;
		std::atomic<bool> m_bPaused{ false };
		std::atomic<bool> m_bStopped{ false };
	protected:
		void ExecuteDispatchThread(function<void(ItsFileMonitorEvent&)> func)
		{
			unique_handle_handle completed(CreateEventW(nullptr, TRUE, FALSE, nullptr));
			if (completed.IsInvalid()) {
				Stop();
				return;
			}
			const HANDLE events[] = { m_stopEvent.p(), completed.p() };
			while (!IsStopped()) {
				ResetEvent(completed.p());
				OVERLAPPED request{};
				request.hEvent = completed.p();
				if (!ReadDirectoryChangesExW(m_dirHandle.p(), m_pbuffer.get(), NotificationBufferSize,
					m_bWatchSubTree, m_mask, nullptr, &request, nullptr, ReadDirectoryNotifyExtendedInformation)) {
					if (GetLastError() == ERROR_NOTIFY_ENUM_DIR) continue;
					break;
				}

				DWORD bytes = 0;
				if (WaitForMultipleObjects(2, events, FALSE, INFINITE) != WAIT_OBJECT_0 + 1) {
					CancelIoEx(m_dirHandle.p(), &request);
					GetOverlappedResult(m_dirHandle.p(), &request, &bytes, TRUE);
					break;
				}
				if (!GetOverlappedResult(m_dirHandle.p(), &request, &bytes, TRUE)) {
					if (GetLastError() == ERROR_NOTIFY_ENUM_DIR) continue;
					break;
				}
				if (bytes > NotificationBufferSize) break;

				constexpr size_t headerSize = offsetof(FILE_NOTIFY_EXTENDED_INFORMATION, FileName);
				size_t offset = 0;
				while (!IsStopped() && bytes - offset >= headerSize) {
					FILE_NOTIFY_EXTENDED_INFORMATION record{};
					std::memcpy(&record, m_pbuffer.get() + offset, headerSize);
					const size_t remaining = bytes - offset;
					const size_t recordSize = record.NextEntryOffset ? record.NextEntryOffset : remaining;
					if (recordSize > remaining || recordSize < headerSize ||
						record.FileNameLength > recordSize - headerSize ||
						record.FileNameLength % sizeof(wchar_t) != 0) break;

					if (!IsPaused() && func) {
						try {
							ItsFileMonitorEvent event{};
							event.Action = record.Action;
							event.CreationTime = record.CreationTime;
							event.LastModificationTime = record.LastModificationTime;
							event.LastChangeTime = record.LastChangeTime;
							event.LastAccessTime = record.LastAccessTime;
							event.FileSize = record.FileSize;
							event.FileAttributes = record.FileAttributes;
							event.ReparsePointTag = record.ReparsePointTag;
							event.FileId = record.FileId;
							event.ParentFileId = record.ParentFileId;
							if (record.FileNameLength == 0) event.FileName = L".";
							else {
								event.FileName.resize(record.FileNameLength / sizeof(wchar_t));
								std::memcpy(event.FileName.data(), m_pbuffer.get() + offset + headerSize,
									record.FileNameLength);
							}
							func(event);
						}
						catch (...) {
							Stop();
							break;
						}
					}
					if (!record.NextEntryOffset) break;
					offset += record.NextEntryOffset;
				}
			}
			Stop();
		}

	public:
		ItsFileMonitor(const wstring& pathname, bool watchSubTree, function<void(ItsFileMonitorEvent&)> func)
			: ItsFileMonitor(pathname, watchSubTree, ItsFileMonitorMask::ChangeLastWrite, func)
		{

		}
		ItsFileMonitor(const wstring& pathname, bool watchSubTree, uint32_t mask, function<void(ItsFileMonitorEvent&)> func)
			: m_mask(mask), m_pathname(pathname), m_bWatchSubTree(watchSubTree)
		{
			m_dirHandle = CreateFileW(m_pathname.c_str(), FILE_LIST_DIRECTORY,
				FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
				FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
			m_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
			if (m_dirHandle.IsInvalid() || m_stopEvent.IsInvalid() || m_mask == 0 || !func) {
				Stop();
				return;
			}
			m_pbuffer = make_unique<BYTE[]>(NotificationBufferSize);
			m_thread = thread(&ItsFileMonitor::ExecuteDispatchThread, this, std::move(func));
		}
		void Pause() {
			this->m_bPaused = true;
		}
		void Resume() {
			this->m_bPaused = false;
		}
		bool IsPaused() {
			return this->m_bPaused;
		}
		void Stop() {
			this->m_bStopped = true;
			if (m_stopEvent.IsValid()) SetEvent(m_stopEvent.p());
		}
		bool IsStopped()
		{
			return this->m_bStopped;
		}
		~ItsFileMonitor()
		{
			this->Stop();

			if (this->m_thread.joinable()) {
				this->m_thread.join();
			}
		}
	};
	//
	// class: ItsRegistry
	//
	// (i): Windows registry routines.
	//
	class ItsIniFile {
	private:
	protected:
	public:
		//
		// Creates a key in ini file.
		//
		static bool CreateKey(wstring filename, wstring sectionname, wstring keyname, wstring value, bool overwriteifexist)
		{
			if (filename.size() == 0 || sectionname.size() == 0 || keyname.size() == 0) {
				return false;
			}

			bool bDoCreate = false;
			if (overwriteifexist == true) {
				bDoCreate = true;
			}
			else {
				wstring readValue;
				if (ItsIniFile::ReadValue(filename, sectionname, keyname, &readValue) == false) {
					bDoCreate = true;
				}
			}

			if (bDoCreate == true) {
				if (!::WritePrivateProfileStringW(sectionname.c_str(), keyname.c_str(), value.c_str(), filename.c_str())) {
					return false;
				}
			}

			return true;
		}

		static bool DeleteKey(wstring filename, wstring sectionname, wstring keyname)
		{
			if (filename.size() == 0 || sectionname.size() == 0 || keyname.size() == 0) {
				return false;
			}

			wstring readValue;
			if (ItsIniFile::ReadValue(filename, sectionname, keyname, &readValue) == true) {
				if (!WritePrivateProfileStringW(sectionname.c_str(), keyname.c_str(), NULL, filename.c_str())) {
					return false;
				}
			}

			return true;
		}

		static bool ReadValue(wstring filename, wstring sectionname, wstring keyname, wstring* value)
		{
			if (filename.empty() || sectionname.empty() || keyname.empty() || value == nullptr) return false;

			const wchar_t missing[] = L"24AA9E3A-5086-409c-A6CF-E1C48B8FFA62";
			vector<wchar_t> buffer(256);
			for (;;) {
				const DWORD count = GetPrivateProfileStringW(sectionname.c_str(), keyname.c_str(),
					missing, buffer.data(), static_cast<DWORD>(buffer.size()), filename.c_str());
				if (count < buffer.size() - 1) {
					if (std::wstring_view(buffer.data(), count) == missing) {
						const DWORD probe = GetPrivateProfileStringW(sectionname.c_str(), keyname.c_str(),
							L"?", buffer.data(), static_cast<DWORD>(buffer.size()), filename.c_str());
						if (probe == 1 && buffer[0] == L'?') return false;
						*value = missing;
					}
					else value->assign(buffer.data(), count);
					return true;
				}
				if (buffer.size() > (std::numeric_limits<DWORD>::max)() / 2) return false;
				buffer.resize(buffer.size() * 2);
			}
		}


		static bool SetValue(wstring filename, wstring sectionname, wstring keyname, wstring value)
		{
			if (filename.size() == 0 || sectionname.size() == 0 || keyname.size() == 0) {
				return false;
			}

			wstring str;
			if (!ItsIniFile::ReadValue(filename, sectionname, keyname, &str)) {
				return false;
			}

			if (value.size() == 0) {
				if (!WritePrivateProfileStringW(sectionname.c_str(), keyname.c_str(), L"", filename.c_str())) {
					return false;
				}
			}
			else {
				if (!WritePrivateProfileStringW(sectionname.c_str(), keyname.c_str(), value.c_str(), filename.c_str())) {
					return false;
				}
			}

			return true;
		}

		static bool CreateSection(const wstring& filename, const wstring& sectionname)
		{
			if (filename.size() == 0 || sectionname.size() == 0) {
				return false;
			}

			vector<wstring> sections;
			if (!EnumerateSections(filename, sections)) return false;
			for (const auto& section : sections) {
				if (CompareStringOrdinal(section.c_str(), -1, sectionname.c_str(), -1, TRUE) == CSTR_EQUAL) return true;
			}
			const wchar_t emptySection[] = { L'\0', L'\0' };
			return WritePrivateProfileSectionW(sectionname.c_str(), emptySection, filename.c_str()) != FALSE;
		}

		static bool DeleteSection(const wstring& filename, const wstring& sectionname)
		{
			if (filename.size() == 0 || sectionname.size() == 0) {
				return false;
			}

			if (!WritePrivateProfileStringW(sectionname.c_str(), NULL, NULL, filename.c_str())) {
				return false;
			}

			return true;
		}

		static bool StoreCollection(const wstring& filename, const wstring& sectionname, map<wstring, wstring>& map, bool overwriteifexist)
		{
			if (filename.size() == 0 || sectionname.size() == 0 || map.size() == 0) {
				return false;
			}

			for (auto itr = map.begin(); itr != map.end(); itr++) {
				const wstring key = itr->first;
				const wstring value = itr->second;
				if (!CreateKey(filename, sectionname, key, value, overwriteifexist)) return false;
			}

			return true;
		}

		static bool EnumerateKeys(const wstring& filename, const wstring& sectionname, map<wstring, wstring>& map)
		{
			if (filename.size() == 0 || sectionname.size() == 0) {
				return false;
			}

			vector<wchar_t> buffer(256);
			for (;;) {
				const DWORD count = GetPrivateProfileStringW(sectionname.c_str(), nullptr, nullptr,
					buffer.data(), static_cast<DWORD>(buffer.size()), filename.c_str());
				if (count < buffer.size() - 2) break;
				if (buffer.size() > (std::numeric_limits<DWORD>::max)() / 2) return false;
				buffer.resize(buffer.size() * 2);
			}
			for (const wchar_t* key = buffer.data(); *key != L'\0'; key += std::wcslen(key) + 1) {
				wstring value;
				if (!ReadValue(filename, sectionname, key, &value)) return false;
				map.emplace(key, std::move(value));
			}
			return true;
		}

		static bool EnumerateSections(const wstring& filename, vector<wstring>& list)
		{
			if (filename.size() == 0) {
				return false;
			}

			vector<wchar_t> buffer(256);
			for (;;) {
				const DWORD count = GetPrivateProfileStringW(nullptr, nullptr, nullptr,
					buffer.data(), static_cast<DWORD>(buffer.size()), filename.c_str());
				if (count < buffer.size() - 2) break;
				if (buffer.size() > (std::numeric_limits<DWORD>::max)() / 2) return false;
				buffer.resize(buffer.size() * 2);
			}
			for (const wchar_t* section = buffer.data(); *section != L'\0'; section += std::wcslen(section) + 1) {
				list.emplace_back(section);
			}
			return true;
		}
	};// ItsIniFile
}// namespace ItSoftware::Win::Core
