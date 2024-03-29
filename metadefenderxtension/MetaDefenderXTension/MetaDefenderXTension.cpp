// MetaDefenderXTension.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


// RLXTension.cpp : Defines the exported functions for the DLL application.
//
// This file contains routines to initialize the plugin, get the API key from
// the config file, and convert hashes to human readable format.

#include "stdafx.h"

#pragma comment(lib, "WININET")

// Global Variables
wchar_t MetaDefenderServer[1024];
DWORD MetaDefenderPort;
wchar_t MetaDefenderApiKey[41];
wchar_t MetaDefenderUsername[128];
wchar_t MetaDefenderPassword[128];

// Read the configuration file;
// This is called when the DLL is first loaded. It loads the API key
// into a global variable for later use.
BOOL ReadAPIConfigFile(HMODULE hModule) {
	// Return value
	BOOL bSuccess = false;

	// Zero out the string variables
	ZeroMemory(MetaDefenderServer, 1024 * sizeof(wchar_t));
	ZeroMemory(MetaDefenderApiKey, 41 * sizeof(wchar_t));

	// Get the Module Path
	wchar_t FilePath[MAX_PATH];
	GetModuleFileName(hModule, FilePath, MAX_PATH);

	// Zero out the module filename and append the config file name
	wchar_t * substr = wcsrchr(FilePath, '\\');
	__int64 index = substr - FilePath + 1;
	ZeroMemory(FilePath + index, (MAX_PATH - index) * sizeof(wchar_t));
	StringCchCopy(substr + 1, MAX_PATH - index, L"mdconfig.txt");

	// Open the file
	HANDLE hFile = CreateFile(FilePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE) {
		// Get the file size
		DWORD dwFileSize = GetFileSize(hFile, NULL);

		if (dwFileSize != INVALID_FILE_SIZE) {
			// Read the file contents into a buffer
			wchar_t * ConfigFileBuffer = (wchar_t *)VirtualAlloc(NULL, dwFileSize, MEM_COMMIT, PAGE_READWRITE);
			DWORD dwNumberOfBytesRead = 0;

			if (ReadFile(hFile, ConfigFileBuffer, dwFileSize, &dwNumberOfBytesRead, NULL)) {
				// File should contain single line of the format <hostname>:<port>:<API key>
				int index = 0;

				// Read the tokenized string from the config file
				wchar_t * nextToken = NULL;
				wchar_t * token = wcstok_s(ConfigFileBuffer, L":", &nextToken);

				while (token != NULL) {
					switch (index) {
					case 0:
						StringCchCopy(MetaDefenderServer, 1024, token);
						break;
					case 1:
						MetaDefenderPort = _wtoi(token);
						break;
					case 2:
						StringCchCopy(MetaDefenderApiKey, 41, token);
						break;
					}
					index++;
					token = wcstok_s(NULL, L":", &nextToken);
				}
			}

			if (ConfigFileBuffer != NULL) {
				VirtualFree(ConfigFileBuffer, 0, MEM_RELEASE);
				ConfigFileBuffer = NULL;
			}
		}

		CloseHandle(hFile);
		bSuccess = true;
	}

	return bSuccess;
}

// Function to convert the binary hash value to human readable format
wchar_t * GetHashString(LONG nItemID, INT64 hashType) {
	int bufSize = 0;

	switch (hashType) {
	case 7:
		bufSize = 16;		// MD5
		break;
	case 8:
		bufSize = 20;		// SHA-1
		break;
	case 9:
		bufSize = 32;		// SHA-256
		break;
	default:
		return NULL;		// Invalid
	}

	// Get the hash value
	DWORD dwOperation = 0x01;
	BYTE * hashBuf = (BYTE *)malloc(bufSize);
	ZeroMemory(hashBuf, bufSize);
	memcpy(hashBuf, (const void *)&dwOperation, sizeof(DWORD));
	if (!XWF_GetHashValue(nItemID, hashBuf))
		return NULL;

	// String to hold the human-readable hash value
	wchar_t * out = (wchar_t *)malloc((bufSize * 2 + 1) * sizeof(wchar_t));
	ZeroMemory(out, (bufSize * 2 + 1) * sizeof(wchar_t));

	// Convert to human readable string
	int n;
	for (n = 0; n < bufSize; ++n) {
		StringCchPrintf(&(out[n * 2]), bufSize * 2, L"%02x", (unsigned int)hashBuf[n]);
	}

	// Free the heap vars
	free(hashBuf);

	// return the result
	return out;
}