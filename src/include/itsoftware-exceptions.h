#pragma once

#include <windows.h>
#include <atlbase.h>
#include <atlcomcli.h>

#include <exception>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

namespace ItSoftware::Exceptions
{
    using std::wstring;
    using std::string;
    using std::endl;
    using std::wstringstream;
    using std::exception;
    using std::shared_ptr;
    using std::make_unique;

    class ItsException : public exception
    {
    private:
        static string ToUtf8(const wstring& text)
        {
            if (text.empty() ||
                text.size() > static_cast<size_t>((std::numeric_limits<int>::max)()))
            {
                return {};
            }

            const int length = static_cast<int>(text.size());
            const int count = ::WideCharToMultiByte(
                CP_UTF8, 0, text.data(), length, nullptr, 0, nullptr, nullptr);

            if (count == 0)
            {
                return {};
            }

            string result(static_cast<size_t>(count), '\0');
            if (::WideCharToMultiByte(
                CP_UTF8, 0, text.data(), length,
                result.data(), count, nullptr, nullptr) != count)
            {
                return {};
            }

            return result;
        }

    protected:
        unsigned int m_errorCode;
        wstring m_message;
        string m_messageA;
        shared_ptr<ItsException> m_pInnerException;

        wstring GetWin32ErrorMessage()
        {
            return GetWin32ErrorMessage(m_errorCode);
        }

        static wstring GetWin32ErrorMessage(unsigned int errorCode)
        {
            wchar_t* buffer = nullptr;
            const DWORD length = ::FormatMessageW(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                reinterpret_cast<LPWSTR>(&buffer),
                0,
                nullptr);

            const std::unique_ptr<wchar_t, decltype(&::LocalFree)>
                message(buffer, &::LocalFree);

            return length != 0 && message
                ? wstring(message.get(), length)
                : wstring{};
        }

        static wstring GetCoLastErrorMessage()
        {
            ATL::CComPtr<IErrorInfo> info;
            if (::GetErrorInfo(0, &info) != S_OK || !info)
            {
                return {};
            }

            ATL::CComBSTR description;
            if (FAILED(info->GetDescription(&description)) || !description.m_str)
            {
                return {};
            }

            return wstring(description.m_str, description.Length());
        }

    public:
        explicit ItsException()
            : m_errorCode(0)
        {}

        explicit ItsException(const unsigned int errorCode)
            : m_errorCode(errorCode)
        {}

        explicit ItsException(const wstring& message)
            : ItsException(0, message)
        {}

        explicit ItsException(const ItsException& other)
            : exception(other),
            m_errorCode(other.m_errorCode),
            m_message(other.m_message),
            m_messageA(ToUtf8(m_message)),
            m_pInnerException(other.m_pInnerException)
        {}

        ItsException(const unsigned int errorCode, const wstring& message)
            : m_errorCode(errorCode),
            m_message(message),
            m_messageA(ToUtf8(m_message))
        {}

        ItsException(const unsigned int errorCode, const ItsException& inner)
            : m_errorCode(errorCode),
            m_pInnerException(std::make_shared<ItsException>(inner))
        {}

        ItsException(
            const unsigned int errorCode,
            const wstring& message,
            const ItsException& inner)
            : m_errorCode(errorCode),
            m_message(message),
            m_messageA(ToUtf8(m_message)),
            m_pInnerException(std::make_shared<ItsException>(inner))
        {}

        virtual const char* what() const noexcept override
        {
            return m_messageA.c_str();
        }

        wstring Message() const
        {
            return m_message;
        }

        unsigned int ErrorCode() const
        {
            return m_errorCode;
        }

        wstring ToString()
        {
            wstringstream msg;
            msg << L"> (e): Exception\n"
                << L"> (e): Type          : ItsException\n"
                << L"> (e): Error Code    : 0x" << std::hex << m_errorCode << L'\n'
                << L"> (e): Message       : " << m_message << L'\n'
                << L"> (e): Win32 Message : " << GetWin32ErrorMessage() << L'\n';

            const wstring comMessage = GetCoLastErrorMessage();
            if (!comMessage.empty())
            {
                msg << L"> (e): COM Message   : " << comMessage << L'\n';
            }

            if (m_pInnerException)
            {
                msg << L'\n' << m_pInnerException->ToString() << L'\n';
            }

            return msg.str();
        }
    };
}