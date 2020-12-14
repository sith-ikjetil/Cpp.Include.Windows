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
#include "atlcomcli.h"		
#include <string>
#include <vector>
#include <codecvt>
#include <random>
#include <functional>
#include <stdlib.h>
#include <time.h>
#include "itsoftware-exceptions.h"

//
// using namespace
//
using namespace ItSoftware::Exceptions;

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

	// 
	// Macros
	//
	#define DECIMALTOSTRING2(d) #d
	#define DECIMALTOSTRING1(d) DECIMALTOSTRING2(d)
	#define ERR_AT_LOCATION(e) __FILE__ "(" DECIMALTOSTRING1(__LINE__) "): " #e 
	#define ERROR_HERE __FILE__ "(" DECIMALTOSTRING2(__LINE__) "): "  
	#define CHAR_BIT 8
	#define BitsCount( val ) ( sizeof( val ) * CHAR_BIT )
	#define Shift( val, steps ) ( steps % BitsCount( val ) )
	#define ROL( val, steps ) ( ( val << Shift( val, steps ) ) | ( val >> ( BitsCount( val ) - Shift( val, steps ) ) ) )
	#define ROR( val, steps ) ( ( val >> Shift( val, steps ) ) | ( val << ( BitsCount( val ) - Shift( val, steps ) ) ) )	
	#define AddFlag( val, flag ) (val | flag)
	#define RemoveFlag(val, flag) (val & ~flag)

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
	// Encoding
	//
	namespace Encoding
	{
		//
		// struct: UTF8
		//
		struct UTF8
		{

			static std::wstring ToString( const std::string& str )
			{
				typedef std::codecvt_utf8<wchar_t> convert_typeX;
				std::wstring_convert<convert_typeX, wchar_t> converterX;

				return converterX.from_bytes( str );
			}

			static std::string ToString( const std::wstring& wstr )
			{
				typedef std::codecvt_utf8<wchar_t> convert_typeX;
				std::wstring_convert<convert_typeX, wchar_t> converterX;

				return converterX.to_bytes( wstr );
			}

			static unsigned char* ToBytes( const std::wstring& wstr, long* cbLength )
			{
				typedef std::codecvt_utf8<wchar_t> convert_typeX;
				std::wstring_convert<convert_typeX, wchar_t> converterX;

				std::string s = converterX.to_bytes( wstr );

				size_t length = s.length( ) * sizeof( char );
				unsigned char* pBytes = new ( std::nothrow ) unsigned char[length];
				if ( pBytes == nullptr )
				{
					*cbLength = 0;
					return nullptr;
				}

				memcpy( (void*) pBytes, s.data( ), length );

				*cbLength = (long) length;

				return pBytes;
			}
		};
	}
	
	//
	// struct: ItsTime
	//
	struct ItsTime
	{
		//
		// Split ms to its parts
		//
		static void SplitMsToHourMinuteSeconds( size_t time_ms, size_t* part_hours, size_t* part_minutes, size_t* part_seconds, size_t* part_ms )
		{
			size_t seconds = time_ms / 1000;

			*part_hours = 0;
			*part_minutes = 0;;
			*part_seconds = 0;
			*part_ms = 0;

			*part_hours = seconds / 3600;
			*part_minutes = ( seconds - ( *part_hours * 3600 ) ) / 60;
			*part_seconds = seconds - ( *part_hours * 3600 ) - ( *part_minutes * 60 );
			*part_ms = time_ms - ( *part_seconds * 1000 ) - ( *part_minutes * 60 * 1000 ) - ( *part_hours * 3600 * 1000 );
		}		

		static void SplitHourToYearWeekDayHour( size_t houIn, size_t* houRest, size_t* day, size_t* week, size_t* year )
		{
			*houRest = houIn;

			*day = houIn / 24;
			*week = *day / 7;
			*year = *week / 52;

			*day -= ( *week * 7 );

			*week -= ( *year * 52 );

			*houRest -= *week * 7 * 24;
			*houRest -= *day * 24;
			*houRest -= *year * 52 * 7 * 24;
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
	// struct: ItsString
	//
	struct ItsString
	{
		static vector<wstring> Split( wstring data, wstring token )
		{
			vector<wstring> output;
			size_t pos = wstring::npos; // size_t to avoid improbable overflow
			do
			{
				pos = data.find( token );
				output.push_back( data.substr( 0, pos ) );
				if ( wstring::npos != pos )
					data = data.substr( pos + token.size( ) );
			} while ( wstring::npos != pos );

			return output;
		}
		// to lower case
		static wstring ToLowerCase( wstring& s )
		{
			std::transform( s.begin( ), s.end( ), s.begin( ), tolower );
			return s;
		}

		// to upper case
		static wstring ToUpperCase( wstring& s )
		{
			std::transform( s.begin( ), s.end( ), s.begin( ), toupper );
			return s;
		}

		// trim from left
		static wstring TrimLeft( wstring& s, const wchar_t* t = L" \t\n\r\f\v" )
		{
			s.erase( 0, s.find_first_not_of( t ) );
			return s;
		}

		// trim from right
		static wstring TrimRight( wstring& s, const wchar_t* t = L" \t\n\r\f\v" )
		{
			s.erase( s.find_last_not_of( t ) + 1 );
			return s;
		}

		// trim from left & right
		static wstring Trim( wstring& s, const wchar_t* t = L" \t\n\r\f\v" )
		{
			wstring right = TrimRight(s, t);
			return TrimLeft( right, t );
		}

		// left count chars
		static wstring Left( wstring &s, unsigned int count )
		{
			if ( s.size( ) == 0 || count <= 0 )
			{
				return wstring( L"" );
			}

			if ( count >= s.size( ) )
			{
				return s;
			}

			wstringstream ss;
			for ( unsigned int i = 0; i < count; i++ )
			{
				ss << s[i];
			}

			wstring str = ss.str( );
			return str;
		}
		
		// mid index, count chars
		static wstring Mid( wstring &s, size_t index, size_t count )
		{
			if ( s.size( ) == 0 || count <= 0 || index < 0 || index >= s.size( ) )
			{
				return wstring( L"" );
			}

			if ( index + count >= s.size( ) )
			{
				wstringstream ss;
				for ( size_t i = index; i < s.size( ); i++ )
				{
					ss << s[i];
				}
				wstring str = ss.str( );
				return str;
			}

			wstringstream ss;
			for ( size_t i = index; i < index + count; i++ )
			{
				ss << s[i];
			}

			wstring str = ss.str( );
			return str;
		}
		
		// right count chars
		static wstring Right( wstring &s, unsigned int count )
		{
			if ( s.size( ) == 0 || count <= 0 )
			{
				return wstring( L"" );
			}

			if ( count >= s.size( ) )
			{
				return s;
			}

			wstringstream ss;
			for ( size_t i = s.size( ) - count; i < s.size( ); i++ )
			{
				ss << s[i];
			}

			wstring str = ss.str( );
			return str;
		}

		//
		//
		//
		static wstring Replace( wstring& s, wstring& replace, wstring& replace_with )
		{
			if ( s.size( ) == 0 || replace.size( ) == 0 || replace.size( ) > s.size( ) )
			{
				return wstring( L"" );
			}

			wstringstream ss;
			size_t index = s.find( replace );
			if ( index == wstring::npos )
			{
				return s;
			}
			while ( index != wstring::npos )
			{
				ss << ItsString::Left( s, (int)index );
				ss << replace_with;
				ss << ItsString::Right( s, (int)(s.size( ) - index - replace.size( )) );

				index = s.find( replace, index + replace.size( ) );
			}

			wstring str = ss.str( );
			return str;
		}
	};

	//
	// struct: ItsConvert
	//
	struct ItsConvert
	{
#pragma warning(disable:4244)
		template<typename Numeric>
		static Numeric ToNumber(const wstring& str)
		{
			if (std::is_same_v<Numeric, short>)
			{
				return (short)std::stoi(str);
			}
			else if (std::is_same_v<Numeric, float>)
			{
				return std::stof(str);
			}
			else if (std::is_same_v<Numeric, double>)
			{
				return std::stod(str);
			}
			else if (std::is_same_v<Numeric, int>)
			{
				return std::stoi(str);
			}
			else if (std::is_same_v<Numeric, long>)
			{
				return std::stol(str);
			}
			else if (std::is_same_v<Numeric, long long>)
			{
				return std::stoll(str);
			}

			Numeric x = std::stoull(str);
			return x;
		}
#pragma warning(error: 4244)
		static long ToLongFromHex(const wstring& str)
		{
			wchar_t* p;
			long n = wcstol(str.c_str(), &p, 16);
			if (*p != 0) {
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
			ss << number;

			wstring tos = ss.str();
			return tos;
		}

		static wstring ToStringFormatted( size_t number )
		{
			wstring txt = ItsConvert::ToString( number );
			
			wstringstream ss;

			int groupSize = 3;			
			int i = groupSize;
			size_t iCount = 0;
			size_t iSize = txt.size( );
			for ( auto itr = txt.rbegin( ); itr < txt.rend( ); itr++ )
			{
				if ( i-- > 0 )
				{
					ss << *itr;
					iCount++;
				}
				else
				{
					if ( iCount < iSize )
					{
						ss << L" ";
						ss << *itr;
						iCount++;
						i = groupSize - 1;
					}					
					else {
						i = groupSize;
					}
				}
				
			}

			txt = ss.str( );
			wstringstream ss2;
			for ( auto itr = txt.rbegin( ); itr < txt.rend( ); itr++ )
			{
				ss2 << *itr;
			}

			wstring retVal = ss2.str( );

			return retVal;
		}

		template<typename Numeric>
		static string ToHexString(Numeric number, bool uppercase = true, uint32_t width = 0, wchar_t fillChar = L'0', bool showBase = false)
		{
			wstringstream ss;
			ss << std::hex;
			if (showBase)
			{
				ss << "0x";
			}
			if (uppercase)
			{
				ss << std::uppercase;
			}
			if (width > 0)
			{
				ss << std::setfill(fillChar) << std::setw(width);
			}
			ss << number;

			wstring tos = ss.str();
			return tos;
		}
		
		static wstring ToString( tm dateTime ) {
			wstringstream ss;
			ss << std::setfill( L'0' ) << std::setw( 4 ) << ( dateTime.tm_year + 1900 );
			ss << L"-" << std::setfill( L'0' ) << std::setw( 2 ) << ( dateTime.tm_mon + 1 );
			ss << L"-" << std::setfill( L'0' ) << std::setw(2) << dateTime.tm_mday;						
			ss << L"T";
			ss << std::setfill( L'0' ) << std::setw( 2 ) << dateTime.tm_hour;
			ss << L":" << std::setfill( L'0' ) << std::setw( 2 ) << dateTime.tm_min;
			ss << L":" << std::setfill( L'0' ) << std::setw( 2 ) << dateTime.tm_sec;			

			wstring tos = ss.str( );
			return tos;
		}

		static tm ToTM( wstring dateTime ) 
		{
			tm t = { 0 };

			t.tm_year = ItsConvert::ToNumber<int>( dateTime.substr( 0, 4 ) ) - 1900;
			t.tm_mon = ItsConvert::ToNumber<int>( dateTime.substr( 5, 2 ) ) - 1;
			t.tm_mday = ItsConvert::ToNumber<int>(dateTime.substr( 8, 2 ));
			t.tm_hour = ItsConvert::ToNumber<int>( dateTime.substr( 11, 2 ) );
			t.tm_min = ItsConvert::ToNumber<int>( dateTime.substr( 14, 2 ) );
			t.tm_sec = ItsConvert::ToNumber<int>( dateTime.substr( 17, 2 ) );

			return t;
		}

		static bool ToBool( wstring flag ) {
						
			flag = ItsString::ToLowerCase( flag );

			if ( !wcscmp( flag.c_str( ), L"true" ) ) {
				return true;
			}
			else if ( !wcscmp( flag.c_str( ), L"1" ) ) {
				return true;
			}
			else if ( !wcscmp( flag.c_str( ), L"yes" ) ) {
				return true;
			}
			else if (!wcscmp(flag.c_str(), L"on")) {
				return true;
			}
			return false;
		}

		static COLORREF ToRGB( wstring color )
		{
			auto rgb = ItsString::Split( color, L"," );
			return (COLORREF)(RGB( ItsConvert::ToNumber<int>(rgb[0]), ItsConvert::ToNumber<int>(rgb[1]), ItsConvert::ToNumber<int>(rgb[2]) ));
		}

		static wstring ToString( vector<long long> pks ) 
		{
			wstringstream ss;
			bool bAddSep = false;
			for ( auto&pk : pks ) {
				if ( bAddSep ) {
					ss << L";";
				}
				ss << pk;
				bAddSep = true;
			}
			wstring tos = ss.str( );
			return tos;
		}

		static vector<long long> ToPK( wstring pks ) 
		{
			using convert_typeX = std::codecvt_utf8<wchar_t>;
			std::wstring_convert<convert_typeX, wchar_t> converterX;

			stringstream ss;
			string s = converterX.to_bytes(pks);
			ss << s;

			string item;
			vector<long long> topk;
			while ( getline( ss, item, ';' ) ) 
			{
				if ( !item.empty( ) ) 
				{
					topk.push_back( ItsConvert::ToNumber<long long>( wstring(item.begin(), item.end())) );
				}
			}

			return topk;
		}		

		static SYSTEMTIME ToSYSTEMTIME( tm dateTime ) 
		{
			SYSTEMTIME st = { 0 };
			
			st.wYear = static_cast<WORD>(dateTime.tm_year + 1900);
			st.wMonth = static_cast<WORD>(dateTime.tm_mon + 1);
			st.wDay = static_cast<WORD>(dateTime.tm_mday);
			st.wHour = static_cast<WORD>(dateTime.tm_hour);
			st.wMinute = static_cast<WORD>(dateTime.tm_min);
			st.wSecond = static_cast<WORD>(dateTime.tm_sec);

			return st;
		}

		static tm ToTM( SYSTEMTIME st )
		{
			tm t = { 0 };
			
			t.tm_year = st.wYear - 1900;
			t.tm_mon = st.wMonth - 1;
			t.tm_mday = st.wDay;
			t.tm_hour = st.wHour;
			t.tm_min = st.wMinute;
			t.tm_sec = st.wSecond;

			return t;
		}
	};	
	
	template<typename Numeric, typename Generator = std::mt19937>
	Numeric ItsRandom(Numeric from, Numeric to)
	{
		thread_local static Generator gen(std::random_device{}());

		using dist_type = typename std::conditional
			<
			std::is_integral<Numeric>::value
			, std::uniform_int_distribution<Numeric>
			, std::uniform_real_distribution<Numeric>
			>::type;

		thread_local static dist_type dist{};

		return dist(gen, typename dist_type::param_type{ from, to });
	}

	//
	// class: ItsDateTime
	//
	class ItsDateTime
	{		
	private:
		tm m_tm;

	public:
		ItsDateTime( tm timeDate ) 
		{
			this->m_tm = timeDate;
		}

		ItsDateTime(ItsDateTime& dateTime)
		{
			this->m_tm = dateTime.m_tm;
		}

		ItsDateTime(ItsDateTime&& dateTime) noexcept
		{
			this->m_tm = dateTime.m_tm;
		}

		static ItsDateTime Now( ) 
		{
			time_t t;
			time( &t );
			tm tm2;
			localtime_s( &tm2, &t );

			return ItsDateTime( tm2 );
		}

		wstring ToString( )
		{
			wstringstream ss;
			ss << std::setfill( L'0' ) << std::setw( 4 ) << ( this->m_tm.tm_year + 1900 );
			ss << L"-" << std::setfill( L'0' ) << std::setw( 2 ) << ( this->m_tm.tm_mon + 1 );
			ss << L"-" << std::setfill( L'0' ) << std::setw( 2 ) << this->m_tm.tm_mday;
			ss << L" ";
			ss << std::setfill( L'0' ) << std::setw( 2 ) << this->m_tm.tm_hour;
			ss << L":" << std::setfill( L'0' ) << std::setw( 2 ) << this->m_tm.tm_min;
			ss << L":" << std::setfill( L'0' ) << std::setw( 2 ) << this->m_tm.tm_sec;

			wstring tos = ss.str( );
			return tos;
		}

		wstring ToString( wstring option )
		{
			if ( option == L"s" || option == L"S" )
			{
				wstringstream ss;
				ss << std::setfill( L'0' ) << std::setw( 4 ) << ( this->m_tm.tm_year + 1900 );
				ss << L"-" << std::setfill( L'0' ) << std::setw( 2 ) << ( this->m_tm.tm_mon + 1 );
				ss << L"-" << std::setfill( L'0' ) << std::setw( 2 ) << this->m_tm.tm_mday;
				ss << L"T";
				ss << std::setfill( L'0' ) << std::setw( 2 ) << this->m_tm.tm_hour;
				ss << L":" << std::setfill( L'0' ) << std::setw( 2 ) << this->m_tm.tm_min;
				ss << L":" << std::setfill( L'0' ) << std::setw( 2 ) << this->m_tm.tm_sec;

				wstring tos = ss.str( );
				return tos;
			}
			
			// day
			int d = this->GetDay( );
			wstring str_d = ItsConvert::ToString( d );
			wstring str_dd;
			if ( d < 10 )
			{
				str_dd = L"0";
				str_dd += str_d;
			}
			else
			{
				str_dd = str_d;
			}

			// month
			int m = this->GetMonth( );
			wstring str_m = ItsConvert::ToString( m );
			wstring str_mm;
			if ( m < 10 )
			{
				str_mm = L"0";
				str_mm += str_m;
			}
			else
			{
				str_mm = str_m;
			}

			// year
			int y = this->GetYear( );
			wstring str_yyyy = ItsConvert::ToString( y );
			wstring str_yy = ItsString::Right( str_yyyy, 2 );

			// hour			
			int h = this->GetHour( );
			wstring str_h = ItsConvert::ToString( h );
			wstring str_hh;
			if ( h < 10 )
			{
				str_hh = L"0";
				str_hh += str_m;
			}
			else
			{
				str_hh = str_h;
			}

			// minute
			int min = this->GetMinute( );
			wstring str_min = ItsConvert::ToString( min );
			wstring str_minmin;
			if ( min < 10 )
			{
				str_minmin = L"0";
				str_minmin += str_min;
			}
			else
			{
				str_minmin = str_min;
			}

			// second
			int s = this->GetSecond( );
			wstring str_s = ItsConvert::ToString( s );
			wstring str_ss;
			if ( s < 10 )
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
			option = ItsString::Replace( option, wdd, str_dd );
			option = ItsString::Replace( option, wd, str_d );

			wstring wMM = wstring(L"MM");
			wstring wM = wstring(L"M");
			option = ItsString::Replace( option, wMM, str_mm );
			option = ItsString::Replace( option, wM, str_m );

			wstring wyyyy = wstring(L"yyyy");
			wstring wyy = wstring(L"yy");
			option = ItsString::Replace( option, wyyyy, str_yyyy );
			option = ItsString::Replace( option, wyy, str_yy );

			wstring wHH = wstring(L"HH");
			wstring wH = wstring(L"H");
			option = ItsString::Replace( option, wHH, str_hh );
			option = ItsString::Replace( option, wH, str_h );

			wstring wmm = wstring(L"mm");
			wstring wm = wstring(L"m");
			option = ItsString::Replace( option, wmm, str_minmin );
			option = ItsString::Replace( option, wm, str_min );

			wstring wss = wstring(L"ss");
			wstring ws = wstring(L"s");
			option = ItsString::Replace( option, wss, str_ss );
			option = ItsString::Replace( option, ws, str_s );

			return option;			
		}

		tm TM( )
		{
			return this->m_tm;
		}

		void AddYears( int year )
		{
			this->m_tm.tm_year += year;			

			mktime( &this->m_tm );
		}

		void AddMonths( int month )
		{			
			this->m_tm.tm_mon += month;

			mktime( &this->m_tm );
		}

		void AddDays( int day )
		{
			this->m_tm.tm_mday += day;

			mktime( &this->m_tm );
		}

		void AddHours( int hour )
		{
			this->m_tm.tm_hour += hour;

			mktime( &this->m_tm );
		}

		void AddMinutes( int min )
		{
			this->m_tm.tm_min += min;

			mktime( &this->m_tm );
		}

		void AddSeconds( int sec )
		{
			this->m_tm.tm_sec += sec;

			mktime( &this->m_tm );
		}

		void Add( int year, int month, int day, int hour, int min, int sec ) 
		{
			this->m_tm.tm_year += year;
			this->m_tm.tm_mon += month;
			this->m_tm.tm_mday += day;
			this->m_tm.tm_hour += hour;
			this->m_tm.tm_min += min;
			this->m_tm.tm_sec += sec;

			mktime( &this->m_tm );
		}

		void SubtractYears( int year )
		{
			this->m_tm.tm_year -= year;			

			mktime( &this->m_tm );
		}

		void SubtractMonths( int month )
		{
			this->m_tm.tm_mon -= month;
		
			mktime( &this->m_tm );
		}

		void SubtractDays( int day )
		{
			this->m_tm.tm_mday -= day;

			mktime( &this->m_tm );
		}

		void SubtractHours( int hour )
		{
			this->m_tm.tm_hour -= hour;

			mktime( &this->m_tm );
		}

		void SubtractMinutes( int min )
		{
			this->m_tm.tm_min -= min;

			mktime( &this->m_tm );		
		}

		void SubtractSeconds( int sec )
		{
			this->m_tm.tm_sec -= sec;

			mktime( &this->m_tm );
		}

		void Subtract( int year, int month, int day, int hour, int min, int sec )
		{
			this->m_tm.tm_year -= year;
			this->m_tm.tm_mon -= month;
			this->m_tm.tm_mday -= day;
			this->m_tm.tm_hour -= hour;
			this->m_tm.tm_min -= min;
			this->m_tm.tm_sec -= sec;

			mktime( &this->m_tm );
		}

		int GetYear( )
		{
			return this->m_tm.tm_year + 1900;
		}

		int GetMonth( )
		{
			return this->m_tm.tm_mon + 1;
		}

		int GetDay( )
		{
			return this->m_tm.tm_mday;
		}

		int GetHour( )
		{
			return this->m_tm.tm_hour;
		}

		int GetMinute( )
		{
			return this->m_tm.tm_min;
		}

		int GetSecond( ) 
		{
			return this->m_tm.tm_sec;
		}

		ItsDateTime& operator=( const tm& timeDate )
		{
			this->m_tm = timeDate;
			return *this;
		}
	};

}// namespace ItSoftware
