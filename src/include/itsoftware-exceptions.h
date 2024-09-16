///////////////////////////////////////////////////////////////////
//: Title        : itsoftware-exceptions.cpp
//: Product:     : Cpp.Include.Windows
//: Date         : 2020-05-01
//: Author       : "Kjetil Kristoffer Solberg" <post@ikjetil.no>
//: Version      : 1.0.0.0
//: Descriptions : Implementation of Cpp.Include.Windows.
#pragma once
//
// #include Windows Headers
//
#include <windows.h>
#include <atlbase.h>

//
// #include C++ Headers
//
#include <sstream>
#include <string>
#include <exception>
#include <stdexcept>
#include <memory>

//
// disable warning
//
#pragma warning(disable: 4244)

//
// namespace
//
namespace ItSoftware::Exceptions
{
    //
    // using
    //
	using std::wstring;
    using std::string;
	using std::endl;
    using std::wstringstream;
	using std::exception;
	using std::shared_ptr;
	using std::make_unique;

    //
    // Class: ItsException
    //
    // (i): Generic root exception
    //
    class ItsException : public exception
    {            
    private:
            
    protected:
		unsigned int				m_errorCode;
		wstring						m_message;
        string                      m_messageA;
		shared_ptr<ItsException>	m_pInnerException;

        //
        //
        //
        wstring GetWin32ErrorMessage()
        {
            return ItsException::GetWin32ErrorMessage( this->m_errorCode );
        }
        //
        //
        //
        static wstring GetWin32ErrorMessage(unsigned int errorCode)
        {
            LPVOID lpMsgBuf = nullptr;
            FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    errorCode,
                    MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // Default language
                    reinterpret_cast<wchar_t*>(&lpMsgBuf),
                    0,
                    NULL );

            if ( lpMsgBuf != nullptr )
            {
                // Copy buffer contet to bstr...
                wstring msg(reinterpret_cast<wchar_t *>(lpMsgBuf) );

                // Free the buffer...
                LocalFree( lpMsgBuf );

                // Return value...
                return msg;
            }

            return L"";
        }
        //
        //
        //
        static wstring GetCoLastErrorMessage()
        {
            IErrorInfo *pErrInfo = nullptr;
            HRESULT hr = GetErrorInfo( NULL, static_cast<IErrorInfo **>(&pErrInfo) );
            CComBSTR bstr;
            if ( hr == S_OK && pErrInfo != nullptr )
            {
                pErrInfo->GetDescription( &bstr );
                return wstring( bstr.operator LPWSTR() );
            }
            return L"";
        }

            
    public:
            
        explicit ItsException()
            : exception(),
            m_errorCode(0)
        { 
        }
            
        explicit ItsException(const unsigned int errorCode)
            : exception(),
            m_errorCode(errorCode)
        {

        }
            
        explicit ItsException(const wstring& message)
            : exception(),
            m_errorCode( 0 ),
            m_message(message)
        {
            m_messageA = string(this->m_message.begin(), this->m_message.end());
        }

        explicit ItsException(const ItsException& other)
        {            
            this->m_errorCode = other.m_errorCode;
            this->m_pInnerException = other.m_pInnerException;				
            this->m_message = other.m_message;            
            m_messageA = string(this->m_message.begin(), this->m_message.end());
        }

        ItsException( const unsigned int errorCode, const wstring& message )
            :	exception(),
            m_errorCode( errorCode ),
            m_message( message )
        {
            m_messageA = string(this->m_message.begin(), this->m_message.end());
        }

        ItsException( const unsigned int errorCode, const ItsException& inner )
            : exception(),
            m_errorCode( errorCode ),
            m_pInnerException(inner.m_pInnerException)
        {
			
        }        

        ItsException( const unsigned int errorCode, const wstring& message, const ItsException& inner )
            : exception(),
            m_errorCode( errorCode ),
			m_message( message ),
            m_pInnerException(inner.m_pInnerException)
        {
            m_messageA = string(this->m_message.begin(), this->m_message.end());
        }

		virtual const char* what() const override
		{            
            return m_messageA.c_str();
		}

        wstring Message() const {
            return this->m_message;
        }

        unsigned int ErrorCode() const {
            return this->m_errorCode;
        }

        wstring ToString() 
        {
            wstringstream msg;
            msg << L"> (e): Exception" << endl;
            msg << L"> (e): Type          : " << L"ItsException" << endl;
            msg << L"> (e): Error Code    : 0x" << L"0x" << std::hex << this->m_errorCode << endl;
            msg << L"> (e): Message       : " << this->m_message << endl;
            msg << L"> (e): Win32 Message : " << this->GetWin32ErrorMessage() << endl;
            wstring com_msg = ItsException::GetCoLastErrorMessage();
            if (com_msg.length() > 0) {
                msg << L"> (e): COM Message   : " << com_msg << endl;
            }

            if ( this->m_pInnerException != nullptr )
            {
                msg << endl;
                msg << this->m_pInnerException->ToString() << endl;
            }				
				
            wstring str = msg.str();
            return str;
        }	
    };// class ItsException
}// namespace ItSoftware::Exceptions