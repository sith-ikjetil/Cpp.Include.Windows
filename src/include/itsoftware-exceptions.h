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
// namespace
//
namespace ItSoftware::Exceptions
{
    //
    // using
    //
	using std::wstring;
	using std::endl;
    using std::wstringstream;
	using std::exception;
	using std::unique_ptr;
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
		unique_ptr<ItsException>	m_pInnerException;

        //
        //
        //
        wstring GetWin32ErrorMessage()
        {
            return this->GetWin32ErrorMessage( this->m_errorCode );
        }
        //
        //
        //
        wstring GetWin32ErrorMessage(unsigned int errorCode)
        {
            LPVOID lpMsgBuf = nullptr;
            FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    errorCode,
                    MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // Default language
                    (wchar_t *)&lpMsgBuf,
                    0,
                    NULL );

            if ( lpMsgBuf != nullptr )
            {
                // Copy buffer contet to bstr...
                wstring msg( (wchar_t *)lpMsgBuf );

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
        wstring GetCoLastErrorMessage()
        {
            IErrorInfo *pErrInfo = nullptr;
            HRESULT hr = GetErrorInfo( NULL, (IErrorInfo **)&pErrInfo );
            CComBSTR bstr;
            if ( hr == S_OK && pErrInfo != nullptr )
            {
                pErrInfo->GetDescription( &bstr );
                return wstring( bstr.operator LPWSTR() );
            }
            return L"";
        }

            
    public:
            
        ItsException()
            : exception(),
            m_errorCode(0)
        { 
        }
            
        ItsException(unsigned int errorCode)
            : exception(),
            m_errorCode(errorCode)
        {

        }
            
        ItsException( wstring message)
            : exception(),
            m_errorCode( 0 ),
            m_message(message)
        {

        }
             
        ItsException( ItsException&& other ) noexcept
            : exception()
        {
            //
            // Take over ownership
            //
            this->m_errorCode = other.m_errorCode;
            this->m_pInnerException = std::move(other.m_pInnerException);
            this->m_message = other.m_message;
                
            //
            // Clean up other
            //
            other.m_errorCode = 0;                
            other.m_message = L"";
        }

        ItsException( unsigned int errorCode, wstring message )
            :	exception(),
            m_errorCode( errorCode ),
            m_message( message )
        {

        }

        ItsException( unsigned int errorCode, ItsException&& inner )
            : exception(),
            m_errorCode( errorCode )				  
        {
			m_pInnerException = make_unique<ItsException>(std::move(inner));
        }

        ItsException( wstring message, ItsException&& inner )
            : exception(),
            m_errorCode( 0 ),
            m_message( message )
        {
			m_pInnerException = make_unique<ItsException>( std::move(inner) );
        }

        ItsException( unsigned int errorCode, wstring message, ItsException&& inner )
            : exception(),
            m_errorCode( errorCode ),
			m_message( message )
        {
			m_pInnerException = make_unique<ItsException>( std::move(inner) );
        }

        virtual wstring ToString()
        {
            wstringstream msg;
            msg << L"## Exception ##" << endl;
            msg << L"Type:" << endl << L"ItsException" << endl << endl;
            msg << L"Error Code:" << endl << L"0x" << std::hex << this->m_errorCode << endl << endl;
            msg << L"Message:" << endl << this->m_message << endl << endl;
            msg << L"Win32 Error Code Message:" << endl << this->GetWin32ErrorMessage() << endl << endl;
            msg << L"COM Last Error Message:" << endl << this->GetCoLastErrorMessage() << endl << endl;

            if ( this->m_pInnerException != nullptr )
            {
                msg << this->m_pInnerException->ToString() << endl;
            }				
				
            return msg.str();
        }	
    };// class ItsException

	//
	// Class: ItsNullReferenceException
	//
	// (i): Generic root exception
	//
	class ItsNullReferenceException : public ItsException
	{
	private:
	protected:
		wstring m_argumentName;

	public:
		ItsNullReferenceException( wstring name )
			: m_argumentName( name )
		{
		}

		ItsNullReferenceException( wstring name, wstring message )
			: m_argumentName( name ),
				ItsException(message)
		{
		}
			
		ItsNullReferenceException( wstring name, ItsException&& inner )
			: m_argumentName( name )
		{
			this->m_pInnerException = make_unique<ItsException>( std::move( inner ) );
		}

		ItsNullReferenceException( wstring name, wstring message, ItsException&& inner )
		{
			this->m_message = message;
			this->m_pInnerException = make_unique<ItsException>( std::move( inner ) );
			this->m_argumentName = name;					
		}

		virtual wstring ToString()
		{
			wstringstream msg;
			msg << L"## Exception ##" << endl;
			msg << L"Type:" << endl << L"ItsNullReferenceException" << endl << endl;
			msg << L"Parameter Name:" << endl << this->m_argumentName << endl << endl;
			msg << L"Message: " << endl << this->m_message << endl << endl;

			if ( this->m_pInnerException != nullptr )
			{
				msg << this->m_pInnerException->ToString() << endl;
			}

			return msg.str();
		}
	};// class ItsNullReferenceException

	//
	// Class: ItsArgumentNullException
	//
	// (i): Generic root exception
	//
	class ItsArgumentNullException : public ItsException
	{			
	private:
	protected:
		wstring m_argumentName;

	public:
		ItsArgumentNullException( wstring name )
			: m_argumentName(name)
		{
		}

		ItsArgumentNullException( wstring name, ItsException&& inner )
			: m_argumentName( name )
		{
			this->m_pInnerException = make_unique<ItsException>( std::move( inner ) );
		}

		virtual wstring ToString()
		{
			wstringstream msg;
			msg << L"## Exception ##" << endl;
			msg << L"Type:" << endl << L"ItsArgumentNullException" << endl << endl;
			msg << L"Parameter Name:" << endl << this->m_argumentName << endl << endl;				

			if ( this->m_pInnerException != nullptr )
			{
				msg << this->m_pInnerException->ToString() << endl;
			}

			return msg.str();
		}
	};// class ItsArgumentNullException

	//
	// Class: ItsArgumentException
	//
	// (i): Generic root exception
	//
	class ItsArgumentException : public ItsException
	{
	private:
	protected:
		wstring m_argumentName;

	public:
		ItsArgumentException( wstring name, wstring message )
			: ItsException(message),
				m_argumentName( name )
		{
		}

		ItsArgumentException( wstring name, wstring message, ItsException&& inner )
			: ItsException(message),
				m_argumentName( name )
		{
			this->m_pInnerException = make_unique<ItsException>( std::move( inner ) );
		}

		virtual wstring ToString()
		{
			wstringstream msg;
			msg << L"## Exception ##" << endl;
			msg << L"Type:" << endl << L"ItsArgumentException" << endl << endl;
			msg << L"Parameter Name:" << endl << this->m_argumentName << endl << endl;
			msg << L"Message:" << endl << this->m_message << endl << endl;

			if ( this->m_pInnerException != nullptr )
			{
				msg << this->m_pInnerException->ToString() << endl;
			}
				
			return msg.str();
		}
	};// class ItsArgumentException
}// namespace ItSoftware::Exceptions