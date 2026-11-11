///////////////////////////////////////////////////////////////////
//: Title        : itsoftware.h
//: Product:     : Cpp.Include.Windows
//: Date         : 2020-05-01
//: Author       : "Kjetil Kristoffer Solberg" <post@ikjetil.no>
//: Version      : 1.0.0.0
//: Descriptions : Implementation of Cpp.Include.Windows.
#pragma once
//
// #include
//
#include <windows.h>
#include <wingdi.h>
#include <crtdbg.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <exception>
#include <algorithm>
#include <chrono>
#include <fstream>
#include "atlcomcli.h"		
#include <string>
#include <vector>
#include <codecvt>
#include <random>
#include <functional>
#include <generator>
#include <regex>
#include <stdlib.h>
#include <time.h>
#include <bit>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstring>
#include <cwctype>
#include <limits>
#include <mutex>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "itsoftware-exceptions.h"

//
// namespace
//
namespace ItSoftware
{
	//
	// using
	//
	using std::string;
	using std::wstring;
	using std::vector;
	using std::stringstream;
	using std::wstringstream;
	using std::endl;
	using ItSoftware::Exceptions::ItsException;

	// 
	// Macros
	//
#define DECIMALTOSTRING2(d) #d
#define DECIMALTOSTRING1(d) DECIMALTOSTRING2(d)
#define ERR_AT_LOCATION(e) __FILE__ "(" DECIMALTOSTRING1(__LINE__) "): " #e 
#define ERROR_HERE __FILE__ "(" DECIMALTOSTRING2(__LINE__) "): "  
#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif
#define BitsCount( val ) ( sizeof( val ) * CHAR_BIT )
#define Shift( val, steps ) ( (steps) % BitsCount( val ) )
#define ROL( val, steps ) ( ::ItSoftware::detail::Rotate<true>((val), (steps)) )
#define ROR( val, steps ) ( ::ItSoftware::detail::Rotate<false>((val), (steps)) )
#define AddFlag( val, flag ) ((val) | (flag))
#define RemoveFlag(val, flag) ((val) & ~(flag))

	namespace detail
	{
		template<bool Left, typename T, typename Steps>
		constexpr auto Rotate(T value, Steps steps)
		{
			using Unsigned = std::make_unsigned_t<T>;
			constexpr int bits = std::numeric_limits<Unsigned>::digits;
			const int count = static_cast<int>(steps % bits);
			const auto word = static_cast<Unsigned>(value);
			return static_cast<decltype(value | value)>(Left ? std::rotl(word, count) : std::rotr(word, count));
		}
	}

	//
	// Function: as_const
	//
	template<typename T>
	const T& as_const(const T& item) { return item; }

	//
	// Function: ItsAddFlag
	//
	template<typename T>
	T ItsAddFlag(T value, T flag)
	{
		return value | flag;
	}

	//
	// Function:: ItsRemoveFlag
	//
	template<typename T>
	T ItsRemoveFlag(T value, T flag)
	{
		return value & ~flag;
	}

	//
	// struct: DebugOnly
	//
	// (i): Wraps a value that is only present in Debug builds.
	//
	template<class T>
	struct DebugOnly {
#ifndef NDEBUG
		T value;
		template<class... A> explicit DebugOnly(A&&... a) : value(std::forward<A>(a)...) {}
		template<class F> void with(F&& f) { f(value); }  // runs only in Debug
#else
		template<class... A> explicit DebugOnly(A&&...) {}
		template<class F> void with(F&&) {}               // no-op in Release
#endif
	};

	//
	// Encoding
	//
	namespace Encoding
	{
		//
		// struct: UTF8
		//
		// (i): UTF8 wrappers routines.
		//
		struct UTF8
		{

			static std::wstring ToString(const std::string& str)
			{
				if (str.empty() || str.size() > static_cast<size_t>((std::numeric_limits<int>::max)())) return {};
				const int length = static_cast<int>(str.size());
				const int count = MultiByteToWideChar(CP_UTF8, 0, str.data(), length, nullptr, 0);
				if (count <= 0) return {};
				std::wstring result(count, 0);
				if (MultiByteToWideChar(CP_UTF8, 0, str.data(), length, result.data(), count) != count) return {};
				return result;
			}

			static std::string ToString(const std::wstring& wstr)
			{
				if (wstr.empty() || wstr.size() > static_cast<size_t>((std::numeric_limits<int>::max)())) return {};
				const int length = static_cast<int>(wstr.size());
				const int count = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), length, nullptr, 0, nullptr, nullptr);
				if (count <= 0) return {};
				std::string result(count, 0);
				if (WideCharToMultiByte(CP_UTF8, 0, wstr.data(), length, result.data(), count, nullptr, nullptr) != count) return {};
				return result;
			}

			static unsigned char* ToBytes(const std::wstring& wstr, long* cbLength)
			{
				if (cbLength == nullptr) return nullptr;
				*cbLength = 0;

				std::string s = UTF8::ToString(wstr);

				const size_t length = s.size();
				if (length == 0 && !wstr.empty()) return nullptr;
				auto* bytes = new (std::nothrow) unsigned char[length];
				if (bytes == nullptr) return nullptr;
				std::memcpy(bytes, s.data(), length);
				*cbLength = static_cast<long>(length);

				return bytes;
			}
		};
	}

	//
	// struct: ItsTime
	//
	// (i): Split milliseconds or render milliseconds to a friendly string.
	//
	struct ItsTime
	{
		static void SplitMsToHourMinuteSeconds(size_t time_ms, size_t* part_hours, size_t* part_minutes, size_t* part_seconds, size_t* part_ms)
		{
			const size_t seconds = time_ms / 1000;
			if (part_hours) *part_hours = seconds / 3600;
			if (part_minutes) *part_minutes = seconds / 60 % 60;
			if (part_seconds) *part_seconds = seconds % 60;
			if (part_ms) *part_ms = time_ms % 1000;
		}

		static void SplitHourToYearWeekDayHour(size_t houIn, size_t* houRest, size_t* day, size_t* week, size_t* year)
		{
			const size_t days = houIn / 24;
			if (houRest) *houRest = houIn % 24;
			if (day) *day = days % 7;
			if (week) *week = days / 7 % 52;
			if (year) *year = days / 7 / 52;
		}

		static wstring RenderMsToFullString(size_t milliseconds, bool bWithMilliseconds)
		{
			size_t hou = 0;
			size_t min = 0;
			size_t sec = 0;
			size_t ms = 0;
			ItsTime::SplitMsToHourMinuteSeconds(milliseconds, &hou, &min, &sec, &ms);

			size_t houRest = 0;
			size_t day = 0;
			size_t week = 0;
			size_t year = 0;
			ItsTime::SplitHourToYearWeekDayHour(hou, &houRest, &day, &week, &year);

			wstringstream ss;
			if (year > 0) {
				if (year == 1)
				{
					ss << year << L" year ";
				}
				else
				{
					ss << year << L" years ";
				}
			}
			if (week > 0 || year > 0) {
				if (week == 1 || week == 0) {
					ss << week << L" week ";
				}
				else
				{
					ss << week << L" weeks ";
				}
			}
			if (day > 0 || week > 0 || year > 0) {
				if (day == 1 || day == 0)
				{
					ss << day << L" day ";
				}
				else
				{
					ss << day << L" days ";
				}
			}
			if (houRest > 0 || day > 0 || week > 0 || year > 0)
			{
				if (houRest == 1 || houRest == 0)
				{
					ss << houRest << L" hour ";
				}
				else
				{
					ss << houRest << L" hours ";
				}
			}

			if (min < 10) {
				ss << L"0" << min << L":";
			}
			else
			{
				ss << min << L":";
			}
			if (sec < 10) {
				ss << L"0" << sec;
			}
			else
			{
				ss << sec;
			}

			if (bWithMilliseconds)
			{
				if (ms < 10) {
					ss << L".00" << ms;
				}
				else if (ms < 100) {
					ss << L".0" << ms;
				}
				else {
					ss << L"." << ms;
				}
			}

			wstring str = ss.str();

			return str;
		}
	};

	//
	// enum ItsExpandDirection
	//
	// (i): Where text in expanded string input text should be aligned.
	//
	enum class ItsExpandDirection
	{
		Left,
		Middle,
		Right
	};

	//
	// struct: ItsString
	//
	// (i): Misc. string routines in one place.
	//
	struct ItsString
	{
		static std::generator<string> StringFromTextByRegex(const string& text, const string& re) {
			return [](string owned, string pattern) -> std::generator<string> {
				const std::regex regex(pattern);
				auto it = std::sregex_iterator(owned.begin(), owned.end(), regex);
				for (; it != std::sregex_iterator{}; ++it) co_yield it->str();
				}(text, re);
		}

		static std::generator<wstring> StringFromTextByRegex(const wstring& text, const wstring& re) {
			return [](wstring owned, wstring pattern) -> std::generator<wstring> {
				const std::wregex regex(pattern);
				auto it = std::wsregex_iterator(owned.begin(), owned.end(), regex);
				for (; it != std::wsregex_iterator{}; ++it) co_yield it->str();
				}(text, re);
		}

		static std::generator<string> WordsFromText(const string& text) {
			return [](string owned) -> std::generator<string> {
				const std::regex regex(R"(\b\w+\b)");
				auto it = std::sregex_iterator(owned.begin(), owned.end(), regex);
				for (; it != std::sregex_iterator{}; ++it) co_yield it->str();
				}(text);
		}

		static std::generator<wstring> WordsFromText(const wstring& text) {
			return [](wstring owned) -> std::generator<wstring> {
				const std::wregex regex(LR"(\b\w+\b)");
				auto it = std::wsregex_iterator(owned.begin(), owned.end(), regex);
				for (; it != std::wsregex_iterator{}; ++it) co_yield it->str();
				}(text);
		}

		static std::generator<string> LinesFromText(const string& text) {
			return [](string owned) -> std::generator<string> {
				size_t start = 0;
				while (start < owned.size()) {
					const auto end = owned.find('\n', start);
					const auto limit = end != string::npos && end > start && owned[end - 1] == '\r' ? end - 1 : end;
					co_yield owned.substr(start, limit == string::npos ? string::npos : limit - start);
					if (end == string::npos) break;
					start = end + 1;
				}
				}(text);
		}

		static std::generator<wstring> LinesFromText(const wstring& text) {
			return [](wstring owned) -> std::generator<wstring> {
				size_t start = 0;
				while (start < owned.size()) {
					const auto end = owned.find('\n', start);
					const auto limit = end != wstring::npos && end > start && owned[end - 1] == '\r' ? end - 1 : end;
					co_yield owned.substr(start, limit == wstring::npos ? wstring::npos : limit - start);
					if (end == wstring::npos) break;
					start = end + 1;
				}
				}(text);
		}

		static wstring WidthExpand(const wstring& source, size_t width, wchar_t fill, ItsExpandDirection direction)
		{
			if (source.size() == 0) {
				return wstring(L"");
			}
			if (width == 0) {
				return wstring(L"");
			}

			if (source.size() >= width) {
				return source.substr(0, width);
			}

			wstringstream result;
			if (direction == ItsExpandDirection::Left)
			{
				for (size_t i = 0; i < (width - source.size()); i++)
				{
					result << fill;
				}
				result << source;
			}
			else if (direction == ItsExpandDirection::Middle)
			{
				for (size_t i = 0; i < ((width - source.size()) / 2); i++)
				{
					result << fill;
				}

				result << source;

				for (size_t i = result.str().size(); i < width; i++)
				{
					result << fill;
				}
			}
			else if (direction == ItsExpandDirection::Right)
			{
				result << source;

				for (size_t i = 0; i < (width - source.size()); i++)
				{
					result << fill;
				}
			}
			else {
				return wstring(L"");
			}

			wstring retVal = result.str();
			return retVal;
		}

		static vector<wstring> Split(const wstring& input, const wstring& delimiter)
		{
			vector<wstring> result;

			if (input.size() == 0 || delimiter.size() == 0) {
				return result;
			}

			size_t start = 0;
			size_t end = input.find(delimiter);

			while (end != std::string::npos) {
				result.push_back(input.substr(start, end - start));
				start = end + delimiter.length();
				end = input.find(delimiter, start);
			}

			// Add the last segment
			result.push_back(input.substr(start));

			return result;
		}

		static wstring ToLowerCase(wstring s)
		{
			std::transform(s.begin(), s.end(), s.begin(), [](wchar_t ch) { return static_cast<wchar_t>(std::towlower(ch)); });
			return s;
		}

		static wstring ToUpperCase(wstring s)
		{
			std::transform(s.begin(), s.end(), s.begin(), [](wchar_t ch) { return static_cast<wchar_t>(std::towupper(ch)); });
			return s;
		}

		static wstring TrimLeft(wstring s, const wstring& t = L" \t\n\r\f\v")
		{
			s.erase(0, s.find_first_not_of(t));
			return s;
		}

		static wstring TrimRight(wstring s, const wstring& t = L" \t\n\r\f\v")
		{
			s.erase(s.find_last_not_of(t) + 1);
			return s;
		}

		static wstring Trim(const wstring& s, const wstring& t = L" \t\n\r\f\v")
		{
			wstring right = TrimRight(s, t);
			return TrimLeft(right, t);
		}

		static wstring Left(const wstring& s, size_t count)
		{
			return s.substr(0, count);
		}

		static wstring Mid(const wstring& s, size_t index, size_t count)
		{
			return index >= s.size() ? wstring{} : s.substr(index, count);
		}

		static wstring Right(const wstring& s, size_t count)
		{
			return s.substr(s.size() - (std::min)(count, s.size()));
		}


		static wstring Replace(const wstring& s, const wstring& replace, const wstring& replace_with)
		{
			if (s.size() == 0 || replace.size() == 0 || replace.size() > s.size())
			{
				return s;
			}

			wstring retVal = s;
			size_t index = retVal.find(replace);
			if (index == wstring::npos)
			{
				return retVal;
			}

			auto result = ItsString::Split(s, replace);
			wstringstream ss;
			bool bHit = false;
			for (const auto& t : result) {
				if (bHit) {
					ss << replace_with;
				}

				ss << t;

				bHit = true;
			}

			return ss.str();
		}
	};

	//
	// enum: DataSizeStringType
	//
	// (i): Enum for string representation of ToDataSizeString ItsConvert function.
	//
	enum class ItsDataSizeStringType {
		Recommended,
		IEC
	};

	//
	// struct: ItsConvert
	//
	// (i): Misc. convertion routines in one place.
	//
	struct ItsConvert
	{
		static wstring ToDataSizeString(size_t size)
		{
			return ItsConvert::ToDataSizeString(size, 0);
		}
		static wstring ToDataSizeString(size_t size, int digits)
		{
			return ItsConvert::ToDataSizeString(size, digits, ItsDataSizeStringType::Recommended);
		}
		static wstring ToDataSizeString(size_t size, int digits, ItsDataSizeStringType type)
		{
			if (digits < 0)
			{
				digits = 0;
			}
			else if (digits > 3)
			{
				digits = 3;
			}

			wstringstream ss;
			double dSize = (double)size;

			int index = 0;
			while (dSize >= 1024)
			{
				dSize /= 1024;
				index++;
			}

			vector<wstring> szSize;
			if (type == ItsDataSizeStringType::IEC) {
				szSize = { L"Bi", L"KiB", L"MiB", L"GiB", L"TiB", L"PiB", L"EiB", L"ZiB", L"YiB", L"BB", L"GP" };
			}
			else {//(type == ItsDataSizeStringType::Recommended) { // defaults to this notation
				szSize = { L"B", L"KB", L"MB", L"GB", L"TB", L"PB", L"EB", L"ZB", L"YB", L"BB", L"GP" };
			}

			size_t tst = (size_t)dSize;
			ss << tst;
			if (digits > 0) {
				double t = dSize - tst;
				wstring ws = ItsConvert::ToString<double>(t);
				if (ws[0] == '0' && t != 0.0) {
					ws += L"000";
				}
				else {
					ws += L".000";
				}
				ss << ws.substr(1, static_cast<size_t>(digits) + 1);
			}
			ss << L" ";
			ss << ((index > (szSize.size() - 1) || index < 0) ? L"?" : szSize[index]);

			wstring retVal = ss.str();
			return retVal;
		}

#pragma warning(disable:4244)
		template<typename Numeric>
		static Numeric ToNumber(const wstring& str)
		{
			if constexpr (std::is_same_v<Numeric, float>) return std::stof(str);
			else if constexpr (std::is_same_v<Numeric, double>) return std::stod(str);
			else if constexpr (std::is_same_v<Numeric, long double>) return std::stold(str);
			else if constexpr (std::is_integral_v<Numeric> && std::is_signed_v<Numeric>) {
				const auto value = std::stoll(str);
				if (value < (std::numeric_limits<Numeric>::min)() || value >(std::numeric_limits<Numeric>::max)()) throw std::out_of_range("Integer does not fit destination type");
				return static_cast<Numeric>(value);
			}
			else if constexpr (std::is_integral_v<Numeric>) {
				const auto first = str.find_first_not_of(L" \t\r\n\f\v");
				if (first != wstring::npos && str[first] == L'-') throw std::out_of_range("Negative unsigned integer");
				const auto value = std::stoull(str);
				if (value > static_cast<unsigned long long>((std::numeric_limits<Numeric>::max)())) {
					throw std::out_of_range("Integer does not fit destination type");
				}
				return static_cast<Numeric>(value);
			}
			else {
				return std::stoull(str);
			}
		}
#pragma warning(error: 4244)
		static long ToLongFromHex(const wstring& str)
		{
			wchar_t* end = nullptr;
			errno = 0;
			const long n = std::wcstol(str.c_str(), &end, 16);
			if (errno == ERANGE || end == str.c_str() || end != str.c_str() + str.size()) {
				return 0;
			}
			return n;
		}

		static wstring ToString(const bool flag)
		{
			if (flag)
			{
				return wstring(L"True");
			}
			return wstring(L"False");
		}

		template<typename Numeric>
		static wstring ToString(Numeric number)
		{
			wstringstream ss;
			if (std::is_same_v<Numeric, float>)
			{
				ss << std::setprecision(std::numeric_limits<float>::digits10);
			}
			else if (std::is_same_v<Numeric, double>)
			{
				ss << std::setprecision(std::numeric_limits<double>::digits10);
			}
			else if (std::is_same_v<Numeric, long double>)
			{
				ss << std::setprecision(std::numeric_limits<long double>::digits10);
			}
			ss << number;

			wstring tos = ss.str();
			return tos;
		}

		static wstring ToStringFormatted(size_t number, wchar_t thousandSep = L',')
		{
			wstring txt = ItsConvert::ToString(number);

			wstringstream ss;

			int groupSize = 3;
			int i = groupSize;
			size_t iCount = 0;
			size_t iSize = txt.size();
			for (auto itr = txt.rbegin(); itr < txt.rend(); itr++)
			{
				if (i-- > 0)
				{
					ss << *itr;
					iCount++;
				}
				else
				{
					if (iCount < iSize)
					{
						ss << thousandSep;
						ss << *itr;
						iCount++;
						i = groupSize - 1;
					}
					else {
						i = groupSize;
					}
				}

			}

			txt = ss.str();
			wstringstream ss2;
			for (auto itr = txt.rbegin(); itr < txt.rend(); itr++)
			{
				ss2 << *itr;
			}

			wstring retVal = ss2.str();

			return retVal;
		}

		template<typename Numeric>
		static string ToHexString(Numeric number, bool uppercase = true, uint32_t width = 0, wchar_t fillChar = L'0', bool showBase = false)
		{
			wstringstream ss;
			ss << std::hex;
			if (showBase) ss << L"0x";
			if (uppercase) ss << std::uppercase;
			if (width > static_cast<uint32_t>((std::numeric_limits<int>::max)())) throw std::length_error("Hexadecimal width is too large");
			ss << std::setfill(fillChar) << std::setw(static_cast<int>(width));
			if constexpr (std::is_same_v<Numeric, char> || std::is_same_v<Numeric, signed char> || std::is_same_v<Numeric, unsigned char>) ss << +number;
			else ss << number;
			return Encoding::UTF8::ToString(ss.str());
		}

		static wstring ToString(tm dateTime) {
			wstringstream ss;
			ss << std::setfill(L'0') << std::setw(4) << (dateTime.tm_year + 1900);
			ss << L"-" << std::setfill(L'0') << std::setw(2) << (dateTime.tm_mon + 1);
			ss << L"-" << std::setfill(L'0') << std::setw(2) << dateTime.tm_mday;
			ss << L"T";
			ss << std::setfill(L'0') << std::setw(2) << dateTime.tm_hour;
			ss << L":" << std::setfill(L'0') << std::setw(2) << dateTime.tm_min;
			ss << L":" << std::setfill(L'0') << std::setw(2) << dateTime.tm_sec;

			wstring tos = ss.str();
			return tos;
		}

		static tm ToTM(const wstring& dateTime)
		{
			tm t = { 0 };

			t.tm_year = ItsConvert::ToNumber<int>(dateTime.substr(0, 4)) - 1900;
			t.tm_mon = ItsConvert::ToNumber<int>(dateTime.substr(5, 2)) - 1;
			t.tm_mday = ItsConvert::ToNumber<int>(dateTime.substr(8, 2));
			t.tm_hour = ItsConvert::ToNumber<int>(dateTime.substr(11, 2));
			t.tm_min = ItsConvert::ToNumber<int>(dateTime.substr(14, 2));
			t.tm_sec = ItsConvert::ToNumber<int>(dateTime.substr(17, 2));

			return t;
		}

		static bool ToBool(wstring flag) {

			flag = ItsString::ToLowerCase(flag);

			if (!wcscmp(flag.c_str(), L"true")) {
				return true;
			}
			else if (!wcscmp(flag.c_str(), L"1")) {
				return true;
			}
			else if (!wcscmp(flag.c_str(), L"yes")) {
				return true;
			}
			else if (!wcscmp(flag.c_str(), L"on")) {
				return true;
			}
			return false;
		}

		static COLORREF ToRGB(wstring color)
		{
			auto rgb = ItsString::Split(color, L",");
			if (rgb.size() != 3) throw std::invalid_argument("Expected three RGB components");
			int channels[3]{};
			for (size_t i = 0; i < rgb.size(); ++i) {
				const auto component = ItsString::Trim(rgb[i]);
				size_t end = 0;
				channels[i] = std::stoi(component, &end);
				if (end != component.size()) throw std::invalid_argument("Invalid RGB component");
				if (channels[i] < 0 || channels[i] > 255) throw std::out_of_range("RGB component is outside 0-255");
			}
			return RGB(channels[0], channels[1], channels[2]);
		}

		static wstring ToString(vector<long long> pks)
		{
			wstringstream ss;
			bool bAddSep = false;
			for (const auto& pk : pks) {
				if (bAddSep) {
					ss << L";";
				}
				ss << pk;
				bAddSep = true;
			}
			wstring tos = ss.str();
			return tos;
		}

		static vector<long long> ToPK(wstring pks)
		{
			//using convert_typeX = std::codecvt_utf8<wchar_t>;
			//std::wstring_convert<convert_typeX, wchar_t> converterX;

			stringstream ss;
			//string s = converterX.to_bytes(pks);
			std::string s = ItSoftware::Encoding::UTF8::ToString(pks);
			ss << s;

			string item;
			vector<long long> topk;
			while (getline(ss, item, ';'))
			{
				if (!item.empty())
				{
					topk.push_back(ItsConvert::ToNumber<long long>(wstring(item.begin(), item.end())));
				}
			}

			return topk;
		}

		static SYSTEMTIME ToSYSTEMTIME(tm dateTime)
		{
			SYSTEMTIME st = { 0 };

			st.wYear = static_cast<WORD>(dateTime.tm_year + 1900);
			st.wMonth = static_cast<WORD>(dateTime.tm_mon + 1);
			st.wDay = static_cast<WORD>(dateTime.tm_mday);
			st.wDayOfWeek = static_cast<WORD>(dateTime.tm_wday);
			st.wHour = static_cast<WORD>(dateTime.tm_hour);
			st.wMinute = static_cast<WORD>(dateTime.tm_min);
			st.wSecond = static_cast<WORD>(dateTime.tm_sec);

			return st;
		}

		static tm ToTM(SYSTEMTIME st)
		{
			tm t = { 0 };

			t.tm_year = st.wYear - 1900;
			t.tm_mon = st.wMonth - 1;
			t.tm_mday = st.wDay;
			t.tm_wday = st.wDayOfWeek;
			t.tm_isdst = -1;
			t.tm_hour = st.wHour;
			t.tm_min = st.wMinute;
			t.tm_sec = st.wSecond;

			return t;
		}
	};

	//
	// Function: ItsRandom
	//
	// (i): A random number wrapper.
	//
	template<typename Numeric, typename Generator = std::mt19937>
	Numeric ItsRandom(Numeric from, Numeric to)
	{
		if (!(from <= to)) throw std::invalid_argument("Invalid random range");
		thread_local static Generator gen(std::random_device{}());
		if constexpr (std::is_integral_v<Numeric>) {
			using Result = std::conditional_t<(sizeof(Numeric) < sizeof(int)), int, Numeric>;
			return static_cast<Numeric>(std::uniform_int_distribution<Result>(
				static_cast<Result>(from), static_cast<Result>(to))(gen));
		}
		else {
			if (!std::isfinite(from) || !std::isfinite(to) ||
				(from < 0 && to >(std::numeric_limits<Numeric>::max)() + from)) {
				throw std::invalid_argument("Invalid random range");
			}
			return std::uniform_real_distribution<Numeric>(from, to)(gen);
		}
	}

	//
	// struct: ItsDateTime
	//
	// (i): Represents a date and time.
	//
	struct ItsDateTime
	{
	private:
		tm m_tm;

		static void AdjustField(int& field, int amount, bool subtract)
		{
			const auto delta = static_cast<long long>(amount);
			const auto result = static_cast<long long>(field) + (subtract ? -delta : delta);
			if (result < (std::numeric_limits<int>::min)() || result >(std::numeric_limits<int>::max)()) {
				throw std::out_of_range("Date component overflow");
			}
			field = static_cast<int>(result);
		}

		void SetNormalized(tm candidate)
		{
			candidate.tm_isdst = -1;
			if (std::mktime(&candidate) == static_cast<time_t>(-1)) {
				throw std::out_of_range("Date is outside the supported time range");
			}
			m_tm = candidate;
		}

	public:
		explicit ItsDateTime(tm timeDate)
		{
			this->m_tm = timeDate;
		}

		explicit ItsDateTime(const ItsDateTime& dateTime)
		{
			this->m_tm = dateTime.m_tm;
		}

		static ItsDateTime Now()
		{
			time_t t;
			time(&t);
			tm tm2{ 0 };
			localtime_s(&tm2, &t);

			return ItsDateTime(tm2);
		}

		bool operator==(const ItsDateTime& other)
		{
			if (this->m_tm.tm_year == other.m_tm.tm_year &&
				this->m_tm.tm_mon == other.m_tm.tm_mon &&
				this->m_tm.tm_mday == other.m_tm.tm_mday &&
				this->m_tm.tm_hour == other.m_tm.tm_hour &&
				this->m_tm.tm_min == other.m_tm.tm_min &&
				this->m_tm.tm_sec == other.m_tm.tm_sec &&
				this->m_tm.tm_wday == other.m_tm.tm_wday &&
				this->m_tm.tm_isdst == other.m_tm.tm_isdst &&
				this->m_tm.tm_yday == other.m_tm.tm_yday) {
				return true;
			}
			return false;
		}

		bool operator!=(const ItsDateTime& other)
		{
			if (this->m_tm.tm_year == other.m_tm.tm_year &&
				this->m_tm.tm_mon == other.m_tm.tm_mon &&
				this->m_tm.tm_mday == other.m_tm.tm_mday &&
				this->m_tm.tm_hour == other.m_tm.tm_hour &&
				this->m_tm.tm_min == other.m_tm.tm_min &&
				this->m_tm.tm_sec == other.m_tm.tm_sec &&
				this->m_tm.tm_wday == other.m_tm.tm_wday &&
				this->m_tm.tm_isdst == other.m_tm.tm_isdst &&
				this->m_tm.tm_yday == other.m_tm.tm_yday) {
				return false;
			}
			return true;
		}

		const ItsDateTime& operator=(const ItsDateTime& other)
		{
			this->m_tm.tm_year = other.m_tm.tm_year;
			this->m_tm.tm_mon = other.m_tm.tm_mon;
			this->m_tm.tm_mday = other.m_tm.tm_mday;
			this->m_tm.tm_hour = other.m_tm.tm_hour;
			this->m_tm.tm_min = other.m_tm.tm_min;
			this->m_tm.tm_sec = other.m_tm.tm_sec;
			this->m_tm.tm_wday = other.m_tm.tm_wday;
			this->m_tm.tm_isdst = other.m_tm.tm_isdst;
			this->m_tm.tm_yday = other.m_tm.tm_yday;

			return *this;
		}

		explicit operator wstring() {
			return this->ToString(L"s");
		}

		wstring ToString()
		{
			wstringstream ss;
			ss << std::setfill(L'0') << std::setw(4) << (this->m_tm.tm_year + 1900);
			ss << L"-" << std::setfill(L'0') << std::setw(2) << (this->m_tm.tm_mon + 1);
			ss << L"-" << std::setfill(L'0') << std::setw(2) << this->m_tm.tm_mday;
			ss << L" ";
			ss << std::setfill(L'0') << std::setw(2) << this->m_tm.tm_hour;
			ss << L":" << std::setfill(L'0') << std::setw(2) << this->m_tm.tm_min;
			ss << L":" << std::setfill(L'0') << std::setw(2) << this->m_tm.tm_sec;

			wstring tos = ss.str();
			return tos;
		}

		wstring ToString(wstring option)
		{
			if (option == L"s" || option == L"S")
			{
				wstringstream ss;
				ss << std::setfill(L'0') << std::setw(4) << (this->m_tm.tm_year + 1900);
				ss << L"-" << std::setfill(L'0') << std::setw(2) << (this->m_tm.tm_mon + 1);
				ss << L"-" << std::setfill(L'0') << std::setw(2) << this->m_tm.tm_mday;
				ss << L"T";
				ss << std::setfill(L'0') << std::setw(2) << this->m_tm.tm_hour;
				ss << L":" << std::setfill(L'0') << std::setw(2) << this->m_tm.tm_min;
				ss << L":" << std::setfill(L'0') << std::setw(2) << this->m_tm.tm_sec;

				wstring tos = ss.str();
				return tos;
			}

			// day
			int d = this->GetDay();
			wstring str_d = ItsConvert::ToString(d);
			wstring str_dd;
			if (d < 10)
			{
				str_dd = L"0";
				str_dd += str_d;
			}
			else
			{
				str_dd = str_d;
			}

			// month
			int m = this->GetMonth();
			wstring str_m = ItsConvert::ToString(m);
			wstring str_mm;
			if (m < 10)
			{
				str_mm = L"0";
				str_mm += str_m;
			}
			else
			{
				str_mm = str_m;
			}

			// year
			int y = this->GetYear();
			wstring str_yyyy = ItsConvert::ToString(y);
			wstring str_yy = ItsString::Right(str_yyyy, 2);

			// hour			
			int h = this->GetHour();
			wstring str_h = ItsConvert::ToString(h);
			wstring str_hh;
			if (h < 10)
			{
				str_hh = L"0";
				str_hh += str_h;
			}
			else
			{
				str_hh = str_h;
			}

			// minute
			int min = this->GetMinute();
			wstring str_min = ItsConvert::ToString(min);
			wstring str_minmin;
			if (min < 10)
			{
				str_minmin = L"0";
				str_minmin += str_min;
			}
			else
			{
				str_minmin = str_min;
			}

			// second
			int s = this->GetSecond();
			wstring str_s = ItsConvert::ToString(s);
			wstring str_ss;
			if (s < 10)
			{
				str_ss = L"0";
				str_ss += str_s;
			}
			else
			{
				str_ss = str_s;
			}

			wstring wdd = wstring(L"dd");
			wstring wd = wstring(L"d");
			option = ItsString::Replace(option, wdd, str_dd);
			option = ItsString::Replace(option, wd, str_d);

			wstring wMM = wstring(L"MM");
			wstring wM = wstring(L"M");
			option = ItsString::Replace(option, wMM, str_mm);
			option = ItsString::Replace(option, wM, str_m);

			wstring wyyyy = wstring(L"yyyy");
			wstring wyy = wstring(L"yy");
			option = ItsString::Replace(option, wyyyy, str_yyyy);
			option = ItsString::Replace(option, wyy, str_yy);

			wstring wHH = wstring(L"HH");
			wstring wH = wstring(L"H");
			option = ItsString::Replace(option, wHH, str_hh);
			option = ItsString::Replace(option, wH, str_h);

			wstring wmm = wstring(L"mm");
			wstring wm = wstring(L"m");
			option = ItsString::Replace(option, wmm, str_minmin);
			option = ItsString::Replace(option, wm, str_min);

			wstring wss = wstring(L"ss");
			wstring ws = wstring(L"s");
			option = ItsString::Replace(option, wss, str_ss);
			option = ItsString::Replace(option, ws, str_s);

			return option;
		}

		const tm& TM()
		{
			return this->m_tm;
		}

		void AddYears(int year)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_year, year, false);
			SetNormalized(candidate);
		}

		void AddMonths(int month)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_mon, month, false);
			SetNormalized(candidate);
		}

		void AddDays(int day)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_mday, day, false);
			SetNormalized(candidate);
		}

		void AddHours(int hour)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_hour, hour, false);
			SetNormalized(candidate);
		}

		void AddMinutes(int min)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_min, min, false);
			SetNormalized(candidate);
		}

		void AddSeconds(int sec)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_sec, sec, false);
			SetNormalized(candidate);
		}

		void Add(int year, int month, int day, int hour, int min, int sec)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_year, year, false);
			AdjustField(candidate.tm_mon, month, false);
			AdjustField(candidate.tm_mday, day, false);
			AdjustField(candidate.tm_hour, hour, false);
			AdjustField(candidate.tm_min, min, false);
			AdjustField(candidate.tm_sec, sec, false);
			SetNormalized(candidate);
		}

		void SubtractYears(int year)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_year, year, true);
			SetNormalized(candidate);
		}

		void SubtractMonths(int month)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_mon, month, true);
			SetNormalized(candidate);
		}

		void SubtractDays(int day)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_mday, day, true);
			SetNormalized(candidate);
		}

		void SubtractHours(int hour)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_hour, hour, true);
			SetNormalized(candidate);
		}

		void SubtractMinutes(int min)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_min, min, true);
			SetNormalized(candidate);
		}

		void SubtractSeconds(int sec)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_sec, sec, true);
			SetNormalized(candidate);
		}

		void Subtract(int year, int month, int day, int hour, int min, int sec)
		{
			tm candidate = m_tm;
			AdjustField(candidate.tm_year, year, true);
			AdjustField(candidate.tm_mon, month, true);
			AdjustField(candidate.tm_mday, day, true);
			AdjustField(candidate.tm_hour, hour, true);
			AdjustField(candidate.tm_min, min, true);
			AdjustField(candidate.tm_sec, sec, true);
			SetNormalized(candidate);
		}

		int GetYear()
		{
			return this->m_tm.tm_year + 1900;
		}

		int GetMonth()
		{
			return this->m_tm.tm_mon + 1;
		}

		int GetDay()
		{
			return this->m_tm.tm_mday;
		}

		int GetHour()
		{
			return this->m_tm.tm_hour;
		}

		int GetMinute()
		{
			return this->m_tm.tm_min;
		}

		int GetSecond()
		{
			return this->m_tm.tm_sec;
		}

		ItsDateTime& operator=(const tm& timeDate)
		{
			this->m_tm = timeDate;
			return *this;
		}
	};

	//
	// ItsLogType
	//
	// (i): Log type
	//
	enum class ItsLogType
	{
		Information,
		Warning,
		Error,
		Other,
		Debug
	};

	//
	// ItsLogStatus
	//
	// (i): Log status
	//
	enum class ItsLogStatus
	{
		OK,
		Failure
	};

	//
	// ItsLogUtil
	//
	// (i): Convertion misc. methods.
	//
	struct ItsLogUtil
	{
		static constexpr const char* LogTypeToString(ItsLogType t)
		{
			switch (t)
			{
			case ItsLogType::Information:
				return "Information";
				break;
			case ItsLogType::Warning:
				return "Warning    ";
				break;
			case ItsLogType::Error:
				return "Error      ";
				break;
			case ItsLogType::Other:
				return "Other      ";
				break;
			case ItsLogType::Debug:
				return "Debug      ";
				break;
			default:
				break;
			}
			return "<UNKNOWN>  ";
		}
		static constexpr const char* LogStatusToString(ItsLogStatus s)
		{
			switch (s)
			{
			case ItsLogStatus::OK:
				return "[  OK  ]";
				break;
			case ItsLogStatus::Failure:
				return "[FAILED]";
				break;
			default:
				break;
			}
			return "[  ??  ]";
		}

		static string WideToUTF8(const std::wstring& wstr) {
			return Encoding::UTF8::ToString(wstr);
		}
	};


	//
	// struct: ItsLog
	//
	// (i): CUstom application event log.
	//
	struct ItsLog
	{
	private:
		wstring m_filename;
		bool m_doQuoteEsc = true;
	public:
		explicit ItsLog(const wstring& fileName, const bool doQuoteEsc = true)
			: m_filename(fileName), m_doQuoteEsc(doQuoteEsc)
		{}
		~ItsLog()
		{}

		static inline std::unique_ptr<ItsLog> ApplicationLog = nullptr;

		void Log(const ItsLogStatus& status, const ItsLogType type, const wstring& title, const wstring& description)
		{
			static std::mutex mutex;
			static std::set<wstring> startedFiles;
			static unsigned long long logCount = 1;

			std::wstring _title = title;
			_title.erase(std::remove(_title.begin(), _title.end(), L'\r'), _title.end());
			_title.erase(std::remove(_title.begin(), _title.end(), L'\n'), _title.end());
			std::string _title8 = ItsLogUtil::WideToUTF8(_title);

			std::wstring _description = description;
			_description.erase(std::remove(_description.begin(), _description.end(), L'\r'), _description.end());
			_description.erase(std::remove(_description.begin(), _description.end(), L'\n'), _description.end());
			std::string _description8 = ItsLogUtil::WideToUTF8(_description);

			auto fnEscQuot = [this](std::string s) {
				//std::string escape_quotes(std::string s)
				{
					if (!this->m_doQuoteEsc)
					{
						return s;
					}

					size_t pos = 0;

					while ((pos = s.find('"', pos)) != std::string::npos)
					{
						s.replace(pos, 1, "\\\"");
						pos += 2;
					}

					return s;
				}
				};

			const std::lock_guard lock(mutex);
			const auto mode = startedFiles.contains(m_filename) ? std::ios::app : std::ios::trunc;
			std::ofstream out(m_filename, mode);
			if (!out) return;
			out << logCount << "> ";
			out << ItsLogUtil::LogStatusToString(status) << " ";
			out << "| Type=" << ItsLogUtil::LogTypeToString(type) << " ";
			out << "| When=" << std::chrono::system_clock::now() << " ";
			out << "| Title=\"" << fnEscQuot(_title8) << "\" ";
			out << "| Description=\"" << fnEscQuot(_description8) << "\"\n";
			out.close();
			if (out) {
				startedFiles.insert(m_filename);
				++logCount;
			}
		}

		void LogInformation(const ItsLogStatus& status, const wstring& title, const wstring& description)
		{
			Log(status, ItsLogType::Information, title, description);
		}

		void LogWarning(const ItsLogStatus& status, const wstring& title, const wstring& description)
		{
			Log(status, ItsLogType::Warning, title, description);
		}

		void LogError(const ItsLogStatus& status, const wstring& title, const wstring& description)
		{
			Log(status, ItsLogType::Error, title, description);
		}

		void LogOther(const ItsLogStatus& status, const wstring& title, const wstring& description)
		{
			Log(status, ItsLogType::Other, title, description);
		}

		void LogDebug(const ItsLogStatus& status, const wstring& title, const wstring& description)
		{
			Log(status, ItsLogType::Debug, title, description);
		}
	};

	//
	// enum: ItsCreateIDOptions
	//
	// (i): ItsID::CreateID options.
	//
	enum class ItsCreateIDOptions
	{
		LowerCase,
		UpperCase,
		LowerAndUpperCase
	};

	//
	// struct: ItsID
	// 
	// (i): Generates unique ID's.
	//
	struct ItsID
	{
	public:
		static wstring CreateID(size_t count, ItsCreateIDOptions options, bool includeNumbers)
		{
			if (count == 0)
			{
				count = 16;
			}

			wstring alphabet;
			if (options != ItsCreateIDOptions::UpperCase) alphabet += L"abcdefghijklmnopqrstuvwxyz";
			if (options != ItsCreateIDOptions::LowerCase) alphabet += L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
			if (includeNumbers) alphabet += L"0123456789";

			wstring id;
			id.reserve(count);
			for (size_t i = 0; i < count; ++i) {
				id += alphabet[ItsRandom<size_t>(0, alphabet.size() - 1)];
			}
			return id;
		}
	};
}// namespace ItSoftware
