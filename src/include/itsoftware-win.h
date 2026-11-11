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
#include <Windows.h>
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
#include <algorithm>
#include <cstring>
#include <cwchar>
#include <limits>
#include <utility>
#include <time.h>

//
// #pragma
//
#pragma warning(disable: 4244)

//
// namespace
//
namespace ItSoftware::Win
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
	using ItSoftware::ItsString;

	//
	// Variable: hCBTHook for ItsWin
	//
	inline HHOOK g_hCBTHook = nullptr;

	//
	// struct: ItsWin
	//
	// (i): Misc Windows routines.
	//
	struct ItsWin
	{
	private:
		//
		// Function: CBTMsgBox
		//
		struct MessageBoxContext
		{
			HWND owner;
			bool centered = false;
		};

		inline static thread_local MessageBoxContext* activeMessageBox = nullptr;

		static LRESULT CALLBACK CBTMsgBox(int code, WPARAM wParam, LPARAM lParam)
		{
			const auto context = activeMessageBox;
			if (code == HCBT_ACTIVATE && context && !context->centered && context->owner) {
				const auto dialog = reinterpret_cast<HWND>(wParam);
				wchar_t className[16]{};
				if (GetWindow(dialog, GW_OWNER) == context->owner &&
					GetClassNameW(dialog, className, 16) &&
					std::wcscmp(className, L"#32770") == 0) {
					context->centered = true;
					CenterToItsParentWindow(dialog);
				}
			}
			return CallNextHookEx(nullptr, code, wParam, lParam);
		}

		static int ShowCenteredMessage(HWND owner, const wstring& title, const wstring& message, UINT flags)
		{
			MessageBoxContext context{ owner };
			const auto previous = activeMessageBox;
			if (!previous && !owner) return MessageBoxW(owner, message.c_str(), title.c_str(), flags);
			const HHOOK hook = !previous
				? SetWindowsHookExW(WH_CBT, CBTMsgBox, nullptr, GetCurrentThreadId())
				: nullptr;

			if (!previous && !hook) return MessageBoxW(owner, message.c_str(), title.c_str(), flags);

			// A reentrant message box uses the enclosing thread hook and its own context.
			activeMessageBox = &context;
			const int result = MessageBoxW(owner, message.c_str(), title.c_str(), flags);
			activeMessageBox = previous;
			if (hook) UnhookWindowsHookEx(hook);
			return result;
		}

		static bool ReportEventLogEntry(const wstring& source, WORD type, short category,
			long eventId, short stringCount, const wchar_t* description)
		{
			// The public description parameter carries one string.
			if (source.empty() || stringCount < 0 || stringCount > 1 ||
				(stringCount == 1 && (!description || *description == L'\0'))) return false;

			const HANDLE log = RegisterEventSourceW(nullptr, source.c_str());
			if (!log) return false;

			LPCWSTR strings[] = { description };
			const BOOL reported = ReportEventW(log, type, static_cast<WORD>(category),
				static_cast<DWORD>(eventId), nullptr, static_cast<WORD>(stringCount), 0,
				stringCount == 1 ? strings : nullptr, nullptr);
			DeregisterEventSource(log);
			return reported != FALSE;
		}

	public:

		//
		//
		//
		static wstring ToRGBString(COLORREF color)
		{
			return std::to_wstring(GetRValue(color)) + L"," +
				std::to_wstring(GetGValue(color)) + L"," +
				std::to_wstring(GetBValue(color));
		}

		//
		// Function: CenterToParentWindow
		//
		static void CenterToItsParentWindow(HWND hDlg)
		{
			const HWND owner = GetWindow(hDlg, GW_OWNER);
			RECT dialog{};
			RECT anchor{};
			if (!owner || !GetWindowRect(hDlg, &dialog) || !GetWindowRect(owner, &anchor)) return;

			const LONG width = dialog.right - dialog.left;
			const LONG height = dialog.bottom - dialog.top;
			LONG left = anchor.left + (anchor.right - anchor.left - width) / 2;
			LONG top = anchor.top + (anchor.bottom - anchor.top - height) / 2;

			MONITORINFO monitor{ sizeof(MONITORINFO) };
			if (GetMonitorInfoW(MonitorFromWindow(owner, MONITOR_DEFAULTTONEAREST), &monitor)) {
				const RECT& area = monitor.rcWork;
				left = std::clamp(left, area.left, (std::max)(area.left, area.right - width));
				top = std::clamp(top, area.top, (std::max)(area.top, area.bottom - height));
			}
			SetWindowPos(hDlg, nullptr, left, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}

		//
		// ## ShowInformationMessageXXX #################################
		//

		//
		// Function: ShowInformationMessage
		//
		static void ShowInformationMessage(const wstring& message)
		{
			ShowInformationMessage(NULL, L"Information!", message);
		}

		//
		// Function: ShowInformationMessage
		//
		static void ShowInformationMessage(const wstring& title, const wstring& message)
		{
			ShowInformationMessage(NULL, title, message);
		}

		//
		// Function: ShowInformationMessage
		//
		static void ShowInformationMessage(HWND hWnd, const wstring& message)
		{
			ShowInformationMessage(hWnd, L"Information!", message);
		}

		//
		// Function: ShowInformationMessage
		//
		static void ShowInformationMessage(HWND hWnd, const wstring& title, const wstring& message)
		{
			ShowCenteredMessage(hWnd, title, message, MB_OK | MB_ICONINFORMATION);
		}

		//
		// Function: ShowInformationMessageYesNo
		//
		static int ShowInformationMessageYesNo(const wstring& message)
		{
			return ShowInformationMessageYesNo(NULL, L"Information!", message);
		}

		//
		// Function: ShowInformationMessageYesNo
		//
		static int ShowInformationMessageYesNo(const wstring& title, const wstring& message)
		{
			return ShowInformationMessageYesNo(NULL, title, message);
		}

		//
		// Function: ShowInformationMessageYesNo
		//
		static int ShowInformationMessageYesNo(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONINFORMATION | MB_YESNO);
		}

		//
		// Function: ShowInformationMessageYesNoCancel
		//
		static int ShowInformationMessageYesNoCancel(const wstring& message)
		{
			return ShowInformationMessageYesNoCancel(NULL, L"Information!", message);
		}

		//
		// Function: ShowInformationMessageYesNoCancel
		//
		static int ShowInformationMessageYesNoCancel(const wstring& title, const wstring& message)
		{
			return ShowInformationMessageYesNoCancel(NULL, title, message);
		}

		//
		// Function: ShowInformationMessageYesNoCancel
		//
		static int ShowInformationMessageYesNoCancel(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONINFORMATION | MB_YESNOCANCEL);
		}

		//
		// Function: ShowInformationMessageAbortRetryIgnore
		//
		static int ShowInformationMessageAbortRetryIgnore(const wstring& message)
		{
			return ShowInformationMessageAbortRetryIgnore(NULL, L"Information!", message);
		}

		//
		// Function: ShowInformationMessageAbortRetryIgnore
		//
		static int ShowInformationMessageAbortRetryIgnore(const wstring& title, const wstring& message)
		{
			return ShowInformationMessageAbortRetryIgnore(NULL, title, message);
		}

		//
		// Function: ShowWarningMessageAbortRetryIgnore
		//
		static int ShowInformationMessageAbortRetryIgnore(HWND hWnd, const wstring& message)
		{
			return ShowInformationMessageAbortRetryIgnore(hWnd, L"Information!", message);
		}

		//
		// Function: ShowInformationMessageAbortRetryIgnore
		//
		static int ShowInformationMessageAbortRetryIgnore(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONINFORMATION | MB_ABORTRETRYIGNORE);
		}

		//
		// ## ShowWarningMessageXXX ##################################
		//

		//
		// ShowWarningMessage
		//
		static void ShowWarningMessage(const wstring& message)
		{
			ShowWarningMessage(NULL, L"Warning!", message);
		}

		//
		// ShowWarningMessage
		//
		static void ShowWarningMessage(const wstring& title, const wstring& message)
		{
			ShowWarningMessage(NULL, title, message);
		}

		//
		// ShowWarningMessage
		//
		static void ShowWarningMessage(HWND hWnd, const wstring& message)
		{
			ShowWarningMessage(hWnd, L"Warning!", message);
		}

		//
		// Function: ShowWarningMessage
		//
		static void ShowWarningMessage(HWND hWnd, const wstring& title, const wstring& message)
		{
			ShowCenteredMessage(hWnd, title, message, MB_OK | MB_ICONWARNING);
		}

		//
		// Function: ShowWarningMessageYesNo
		//
		static int ShowWarningMessageYesNo(const wstring& message)
		{
			return ShowWarningMessageYesNo(NULL, L"Warning!", message);
		}

		//
		// Function: ShowWarningMessageYesNo
		//
		static int ShowWarningMessageYesNo(const wstring& title, const wstring& message)
		{
			return ShowWarningMessageYesNo(NULL, title, message);
		}

		//
		// Function: ShowWarningMessageYesNo
		//
		static int ShowWarningMessageYesNo(HWND hWnd, const wstring& message)
		{
			return ShowWarningMessageYesNo(hWnd, L"Warning!", message);
		}

		//
		// Function: ShowWarningMessageYesNo
		//
		static int ShowWarningMessageYesNo(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONWARNING | MB_YESNO);
		}

		//
		// Function: ShowWarningMessageYesNoCancel
		//
		static int ShowWarningMessageYesNoCancel(const wstring& message)
		{
			return ShowWarningMessageYesNoCancel(NULL, L"Warning!", message);
		}

		//
		// Function: ShowWarningMessageYesNoCancel
		//
		static int ShowWarningMessageYesNoCancel(const wstring& title, const wstring& message)
		{
			return ShowWarningMessageYesNoCancel(NULL, title, message);
		}

		//
		// Function: ShowWarningMessageYesNoCancel
		//
		static int ShowWarningMessageYesNoCancel(HWND hWnd, const wstring& message)
		{
			return ShowWarningMessageYesNoCancel(hWnd, L"Warning!", message);
		}

		//
		// Function: ShowWarningMessageYesNoCancel
		//
		static int ShowWarningMessageYesNoCancel(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONWARNING | MB_YESNOCANCEL);
		}

		//
		// Function: ShowWarningMessageAbortRetryIgnore
		//
		static int ShowWarningMessageAbortRetryIgnore(const wstring& message)
		{
			return ShowWarningMessageAbortRetryIgnore(NULL, L"Warning!", message);
		}

		//
		// Function: ShowWarningMessageAbortRetryIgnore
		//
		static int ShowWarningMessageAbortRetryIgnore(const wstring& title, const wstring& message)
		{
			return ShowWarningMessageAbortRetryIgnore(NULL, title, message);
		}

		//
		// Function: ShowWarningMessageAbortRetryIgnore
		//
		static int ShowWarningMessageAbortRetryIgnore(HWND hWnd, const wstring& message)
		{
			return ShowWarningMessageAbortRetryIgnore(hWnd, L"Warning!", message);
		}

		//
		// Function: ShowWarningMessageAbortRetryIgnore
		//
		static int ShowWarningMessageAbortRetryIgnore(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONWARNING | MB_ABORTRETRYIGNORE);
		}

		//
		// ## ShowHandMessageXXX #################################
		//

		//
		// Function: ShowHandMessage
		//
		static void ShowHandMessage(const wstring& message)
		{
			ShowHandMessage(NULL, L"Hand!", message);
		}

		//
		// Function: ShowHandMessage
		//
		static void ShowHandMessage(const wstring& title, const wstring& message)
		{
			ShowHandMessage(NULL, title, message);
		}

		//
		// Function: ShowHandMessage
		//
		static void ShowHandMessage(HWND hWnd, const wstring& message)
		{
			ShowHandMessage(hWnd, L"Hand!", message);
		}

		//
		// Function: ShowHandMessage
		//
		static void ShowHandMessage(HWND hWnd, const wstring& title, const wstring& message)
		{
			ShowCenteredMessage(hWnd, title, message, MB_OK | MB_ICONHAND);
		}

		//
		// Function: ShowHandMessageYesNo
		//
		static int ShowHandMessageYesNo(const wstring& message)
		{
			return ShowHandMessageYesNo(NULL, L"Hand!", message);
		}

		//
		// Function: ShowHandMessageYesNo
		//
		static int ShowHandMessageYesNo(const wstring& title, const wstring& message)
		{
			return ShowHandMessageYesNo(NULL, title, message);
		}

		//
		// Function: ShowHandMessageYesNo
		//
		static int ShowHandMessageYesNo(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONHAND | MB_YESNO);
		}

		//
		// Function: ShowHandMessageYesNoCancel
		//
		static int ShowHandMessageYesNoCancel(const wstring& message)
		{
			return ShowHandMessageYesNoCancel(NULL, L"Hand!", message);
		}

		//
		// Function: ShowHandMessageYesNoCancel
		//
		static int ShowHandMessageYesNoCancel(const wstring& title, const wstring& message)
		{
			return ShowHandMessageYesNoCancel(NULL, title, message);
		}

		//
		// Function: ShowHandMessageYesNoCancel
		//
		static int ShowHandMessageYesNoCancel(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONHAND | MB_YESNOCANCEL);
		}

		//
		// Function: ShowHandMessageAbortRetryIgnore
		//
		static int ShowHandMessageAbortRetryIgnore(const wstring& message)
		{
			return ShowHandMessageAbortRetryIgnore(NULL, L"Hand!", message);
		}

		//
		// Function: ShowHandMessageAbortRetryIgnore
		//
		static int ShowHandMessageAbortRetryIgnore(const wstring& title, const wstring& message)
		{
			return ShowHandMessageAbortRetryIgnore(NULL, title, message);
		}

		//
		// Function: ShowHandMessageAbortRetryIgnore
		//
		static int ShowHandMessageAbortRetryIgnore(HWND hWnd, const wstring& message)
		{
			return ShowHandMessageAbortRetryIgnore(hWnd, L"Hand!", message);
		}

		//
		// Function: ShowHandMessageAbortRetryIgnore
		//
		static int ShowHandMessageAbortRetryIgnore(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONHAND | MB_ABORTRETRYIGNORE);
		}

		//
		// ## ShowErrorMessageXXX ################################
		//

		//
		// Function: ShowErrorMessage
		//
		static void ShowErrorMessage(const wstring& message)
		{
			ShowErrorMessage(NULL, L"Error!", message);
		}

		//
		// Function: ShowErrorMessage
		//
		static void ShowErrorMessage(const wstring& title, const wstring& message)
		{
			ShowErrorMessage(NULL, title, message);
		}

		//
		// Function: ShowErrorMessage
		//
		static void ShowErrorMessage(HWND hWnd, const wstring& message)
		{
			ShowErrorMessage(hWnd, L"Error!", message);
		}

		//
		// Function: ShowErrorMessage
		//
		static void ShowErrorMessage(HWND hWnd, const wstring& title, const wstring& message)
		{
			ShowCenteredMessage(hWnd, title, message, MB_OK | MB_ICONERROR);
		}

		//
		// Function: ShowErrorMessageYesNo
		//
		static int ShowErrorMessageYesNo(const wstring& message)
		{
			return ShowErrorMessageYesNo(NULL, L"Error!", message);
		}

		//
		// Function: ShowErrorMessageYesNo
		//
		static int ShowErrorMessageYesNo(const wstring& title, const wstring& message)
		{
			return ShowErrorMessageYesNo(NULL, title, message);
		}

		//
		// Function: ShowErrorMessageYesNo
		//
		static int ShowErrorMessageYesNo(HWND hWnd, const wstring& message)
		{
			return ShowErrorMessageYesNo(hWnd, L"Error!", message);
		}

		//
		// Function: ShowErrorMessageYesNo
		//
		static int ShowErrorMessageYesNo(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONERROR | MB_YESNO);
		}

		//
		// Function: ShowErrorMessageYesNoCancel
		//
		static int ShowErrorMessageYesNoCancel(const wstring& message)
		{
			return ShowErrorMessageYesNoCancel(NULL, L"Error!", message);
		}

		//
		// Function: ShowErrorMessageYesNoCancel
		//
		static int ShowErrorMessageYesNoCancel(const wstring& title, const wstring& message)
		{
			return ShowErrorMessageYesNoCancel(NULL, title, message);
		}

		//
		// Function: ShowErrorMessageYesNoCancel
		//
		static int ShowErrorMessageYesNoCancel(HWND hWnd, const wstring& message)
		{
			return ShowErrorMessageYesNoCancel(hWnd, L"Error!", message);
		}

		//
		// Function: ShowErrorMessageYesNoCancel
		//
		static int ShowErrorMessageYesNoCancel(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONERROR | MB_YESNOCANCEL);
		}

		//
		// Function: ShowErrorMessageAbortRetryIgnore
		//
		static int ShowErrorMessageAbortRetryIgnore(const wstring& message)
		{
			return ShowErrorMessageAbortRetryIgnore(NULL, L"Error!", message);
		}

		//
		// Function: ShowErrorMessageAbortRetryIgnore
		//
		static int ShowErrorMessageAbortRetryIgnore(const wstring& title, const wstring& message)
		{
			return ShowErrorMessageAbortRetryIgnore(NULL, title, message);
		}

		//
		// Function: ShowErrorMessageAbortRetryIgnore
		//
		static int ShowErrorMessageAbortRetryIgnore(HWND hWnd, const wstring& message)
		{
			return ShowErrorMessageAbortRetryIgnore(hWnd, L"Error!", message);
		}

		//
		// Function: ShowErrorMessageAbortRetryIgnore
		//
		static int ShowErrorMessageAbortRetryIgnore(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONERROR | MB_ABORTRETRYIGNORE);
		}

		//
		// ## ShowQuestionMessageXXX
		//

		//
		// Function: ShowQuestionMessageYesNo
		//
		static int ShowQuestionMessageYesNo(const wstring& message)
		{
			return ShowQuestionMessageYesNo(NULL, L"Question!", message);
		}

		//
		// Function: ShowQuestionMessageYesNo
		//
		static int ShowQuestionMessageYesNo(const wstring& title, const wstring& message)
		{
			return ShowQuestionMessageYesNo(NULL, title, message);
		}

		//
		// Function: ShowQuestionMessageYesNo
		//
		static int ShowQuestionMessageYesNo(HWND hWnd, const wstring& message)
		{
			return ShowQuestionMessageYesNo(hWnd, L"Question!", message);
		}

		//
		// Function: ShowQuestionMessageYesNo
		//
		static int ShowQuestionMessageYesNo(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONQUESTION | MB_YESNO);
		}

		//
		// Function: ShowQuestionMessageYesNoCancel
		//
		static int ShowQuestionMessageYesNoCancel(const wstring& message)
		{
			return ShowQuestionMessageYesNoCancel(NULL, L"Question!", message);
		}

		//
		// Function: ShowQuestionMessageYesNoCancel
		//
		static int ShowQuestionMessageYesNoCancel(const wstring& title, const wstring& message)
		{
			return ShowQuestionMessageYesNoCancel(NULL, title, message);
		}

		//
		// Function: ShowQuestionMessageYesNoCancel
		//
		static int ShowQuestionMessageYesNoCancel(HWND hWnd, const wstring& message)
		{
			return ShowQuestionMessageYesNoCancel(hWnd, L"Question!", message);
		}

		//
		// Function: ShowQuestionMessageYesNoCancel
		//
		static int ShowQuestionMessageYesNoCancel(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONQUESTION | MB_YESNOCANCEL);
		}

		//
		// Function: ShowQuestionMessageAbortRetryIgnore
		//
		static int ShowQuestionMessageAbortRetryIgnore(const wstring& message)
		{
			return ShowQuestionMessageAbortRetryIgnore(NULL, L"Question!", message);
		}

		//
		// Function: ShowQuestionMessageAbortRetryIgnore
		//
		static int ShowQuestionMessageAbortRetryIgnore(const wstring& title, const wstring& message)
		{
			return ShowQuestionMessageAbortRetryIgnore(NULL, title, message);
		}

		//
		// Function: ShowQuestionMessageAbortRetryIgnore
		//
		static int ShowQuestionMessageAbortRetryIgnore(HWND hWnd, const wstring& message)
		{
			return ShowQuestionMessageAbortRetryIgnore(hWnd, L"Question!", message);
		}

		//
		// Function: ShowQuestionMessageAbortRetryIgnore
		//
		static int ShowQuestionMessageAbortRetryIgnore(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONQUESTION | MB_ABORTRETRYIGNORE);
		}

		//
		// ## ShowStopMessageXXX
		//

		//
		// Function: ShowStopMessageYesNo
		//
		static int ShowStopMessageYesNo(const wstring& message)
		{
			return ShowStopMessageYesNo(NULL, L"Stop!", message);
		}

		//
		// Function: ShowStopMessageYesNo
		//
		static int ShowStopMessageYesNo(const wstring& title, const wstring& message)
		{
			return ShowStopMessageYesNo(NULL, title, message);
		}

		//
		// Function: ShowStopMessageYesNo
		//
		static int ShowStopMessageYesNo(HWND hWnd, const wstring& message)
		{
			return ShowStopMessageYesNo(hWnd, L"Stop!", message);
		}

		//
		// Function: ShowStopMessageYesNo
		//
		static int ShowStopMessageYesNo(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONSTOP | MB_YESNO);
		}

		//
		// Function: ShowStopMessageYesNoCancel
		//
		static int ShowStopMessageYesNoCancel(const wstring& message)
		{
			return ShowStopMessageYesNoCancel(NULL, L"Stop!", message);
		}

		//
		// Function: ShowStopMessageYesNoCancel
		//
		static int ShowStopMessageYesNoCancel(const wstring& title, const wstring& message)
		{
			return ShowStopMessageYesNoCancel(NULL, title, message);
		}

		//
		// Function: ShowStopMessageYesNoCancel
		//
		static int ShowStopMessageYesNoCancel(HWND hWnd, const wstring& message)
		{
			return ShowStopMessageYesNoCancel(hWnd, L"Stop!", message);
		}

		//
		// Function: ShowStopMessageYesNoCancel
		//
		static int ShowStopMessageYesNoCancel(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONSTOP | MB_YESNOCANCEL);
		}

		//
		// Function: ShowStopMessageAbortRetryIgnore
		//
		static int ShowStopMessageAbortRetryIgnore(const wstring& message)
		{
			return ShowStopMessageAbortRetryIgnore(NULL, L"Stop!", message);
		}

		//
		// Function: ShowStopMessageAbortRetryIgnore
		//
		static int ShowStopMessageAbortRetryIgnore(const wstring& title, const wstring& message)
		{
			return ShowStopMessageAbortRetryIgnore(NULL, title, message);
		}

		//
		// Function: ShowStopMessageAbortRetryIgnore
		//
		static int ShowStopMessageAbortRetryIgnore(HWND hWnd, const wstring& message)
		{
			return ShowStopMessageAbortRetryIgnore(hWnd, L"Stop!", message);
		}

		//
		// Function: ShowStopMessageYesNoCancel
		//
		static int ShowStopMessageAbortRetryIgnore(HWND hWnd, const wstring& title, const wstring& message)
		{
			return ShowCenteredMessage(hWnd, title, message, MB_ICONSTOP | MB_ABORTRETRYIGNORE);
		}

		//
		// ReportSuccessEventLogEntry ###########################################
		//

		//
		// Function: ReportSuccessEventLogEntry
		//
		static bool ReportSuccessEventLogEntry(const wstring& sourcename, const wstring& text)
		{
			if (sourcename.size() == 0)
			{
				return false;
			}

			return ReportSuccessEventLogEntry(sourcename, 0, 0, 1, text.c_str());
		}

		//
		// Function: ReportSuccessEventLogEntry
		//
		static bool ReportSuccessEventLogEntry(const wstring& sourcename, short category, long eventid, short numberofstrings, const wchar_t* description)
		{
			return ReportEventLogEntry(sourcename, EVENTLOG_SUCCESS, category, eventid, numberofstrings, description);
		}

		//
		// ReportErrorEventLogEntry ############################################
		//

		//
		// Function: ReportErrorEventLogEntry
		//
		static bool ReportErrorEventLogEntry(const wstring& sourcename, const wstring& text)
		{
			if (sourcename.size() == 0)
			{
				return false;
			}

			return ReportErrorEventLogEntry(sourcename, 0, 0, 1, text.c_str());
		}

		//
		// Function: ReportErrorEventLogEntry
		//
		static bool ReportErrorEventLogEntry(const wstring& sourcename, short category, long eventid, short numberofstrings, const wchar_t* description)
		{
			return ReportEventLogEntry(sourcename, EVENTLOG_ERROR_TYPE, category, eventid, numberofstrings, description);
		}

		//
		// ReportWarningEventLogEntry #########################################################
		//

		//
		// Function: ReportWarningEventLogEntry
		//
		static bool ReportWarningEventLogEntry(const wstring& sourcename, const wstring& text)
		{
			if (sourcename.size() == 0)
			{
				return false;
			}

			return ReportWarningEventLogEntry(sourcename, 0, 0, 1, text.c_str());
		}

		//
		// Function: ReportWarningEventLogEntry
		//
		static bool ReportWarningEventLogEntry(const wstring& sourcename, short category, long eventid, short numberofstrings, const wchar_t* description)
		{
			return ReportEventLogEntry(sourcename, EVENTLOG_WARNING_TYPE, category, eventid, numberofstrings, description);
		}

		//
		// ReportInformationEventLogEntry #########################################################
		//

		//
		// Function: ReportInformationEventLogEntry
		//
		static bool ReportInformationEventLogEntry(const wstring& sourcename, const wstring& text)
		{
			if (sourcename.size() == 0)
			{
				return false;
			}

			return ReportInformationEventLogEntry(sourcename, 0, 0, 1, text.c_str());
		}

		//
		// Function: ReportInformationEventLogEntry
		//
		static bool ReportInformationEventLogEntry(const wstring& sourcename, short category, long eventid, short numberofstrings, const wchar_t* description)
		{
			return ReportEventLogEntry(sourcename, EVENTLOG_INFORMATION_TYPE, category, eventid, numberofstrings, description);
		}

		//
		// ReportAuditSuccessEventLogEntry #########################################################
		//

		//
		// Function: ReportAuditSuccessEventLogEntry
		//
		static bool ReportAuditSuccessEventLogEntry(const wstring& sourcename, const wstring& text)
		{
			if (sourcename.size() == 0)
			{
				return false;
			}

			return ReportAuditSuccessEventLogEntry(sourcename, 0, 0, 1, text.c_str());
		}

		//
		// Function: ReportAuditSuccessEventLogEntry
		//
		static bool ReportAuditSuccessEventLogEntry(const wstring& sourcename, short category, long eventid, short numberofstrings, const wchar_t* description)
		{
			return ReportEventLogEntry(sourcename, EVENTLOG_AUDIT_SUCCESS, category, eventid, numberofstrings, description);
		}

		//
		// ReportAuditFailureEventLogEntry #########################################################
		//

		//
		// Function: ReportAuditFailureEventLogEntry
		//
		static bool ReportAuditFailureEventLogEntry(const wstring& sourcename, const wstring& text)
		{
			if (sourcename.size() == 0)
			{
				return false;
			}

			return ReportAuditFailureEventLogEntry(sourcename, 0, 0, 1, text.c_str());
		}

		//
		// Function: ReportAuditFailureEventLogEntry
		//
		static bool ReportAuditFailureEventLogEntry(const wstring& sourcename, short category, long eventid, short numberofstrings, const wchar_t* description)
		{
			return ReportEventLogEntry(sourcename, EVENTLOG_AUDIT_FAILURE, category, eventid, numberofstrings, description);
		}


		//
		// Function: FromBase64String
		//
		static vector<BYTE> FromBase64String(wstring str)
		{
			if (str.empty()) return {};
			if (str.size() > (std::numeric_limits<DWORD>::max)() || str.find(L'\0') != wstring::npos) {
				throw ItsException(L"Invalid Base64 input length or embedded NUL");
			}

			const auto length = static_cast<DWORD>(str.size());
			DWORD bytes = 0;
			if (!CryptStringToBinaryW(str.c_str(), length, CRYPT_STRING_BASE64, nullptr, &bytes, nullptr, nullptr)) {
				throw ItsException(L"CryptStringToBinary failed");
			}

			vector<BYTE> decoded(bytes);
			if (!CryptStringToBinaryW(str.c_str(), length, CRYPT_STRING_BASE64, decoded.data(), &bytes, nullptr, nullptr)) {
				throw ItsException(L"CryptStringToBinary failed");
			}
			decoded.resize(bytes);
			return decoded;
		}

		//
		// Function: ToBase64String
		//
		static wstring ToBase64String(vector<BYTE> data)
		{
			if (data.empty()) return {};
			if (data.size() > (std::numeric_limits<DWORD>::max)()) throw ItsException(L"Base64 input is too large");

			const auto length = static_cast<DWORD>(data.size());
			DWORD count = 0;
			if (!CryptBinaryToStringW(data.data(), length, CRYPT_STRING_BASE64, nullptr, &count) || !count) {
				throw ItsException(L"CryptBinaryToString failed");
			}
			wstring encoded(count, L'\0');
			if (!CryptBinaryToStringW(data.data(), length, CRYPT_STRING_BASE64, encoded.data(), &count) ||
				count >= encoded.size()) {
				throw ItsException(L"CryptBinaryToString failed");
			}
			encoded.resize(count);
			return encoded;
		}

		//
		// Function: FromBase64StringToString
		//
		static wstring FromBase64StringToString(wstring str)
		{
			const auto decoded = FromBase64String(std::move(str));
			if (decoded.size() % sizeof(wchar_t) != 0) {
				throw ItsException(L"Base64 data is not a sequence of wide characters");
			}
			wstring result(decoded.size() / sizeof(wchar_t), L'\0');
			if (!decoded.empty()) std::memcpy(result.data(), decoded.data(), decoded.size());
			return result;
		}

		//
		// Function: ToBase64StringFromString
		//
		static wstring ToBase64StringFromString(wstring str)
		{
			if (str.empty()) return {};
			if (str.size() > (std::numeric_limits<DWORD>::max)() / sizeof(wchar_t)) {
				throw ItsException(L"Base64 input is too large");
			}

			const auto* bytes = reinterpret_cast<const BYTE*>(str.data());
			return ToBase64String(vector<BYTE>(bytes, bytes + str.size() * sizeof(wchar_t)));
		}
	};
}// namespace ItSoftware::Win
