#include "pch.h"

/* STATIC HTTP REQUEST URIs*/
const static std::string UploadFileURI = "/v4/file";

// Global Variables
std::string MetaDefenderServer;
DWORD MetaDefenderPort;
std::string MetaDefenderApiKey;

// Get information from the server
// Returns a char * buffer that is allocated in this function; caller's obligation to free the buffer
char* MD_UploadFile(std::string URI, PVOID fileBuffer, DWORD dwFileSize, DWORD* dwResponseSize) {
	// The buffer to hold the response
	char* ResponseBuffer = NULL;

	// Header (Auth token)
	std::string authTokenHeader = "apikey: ";

	// Header (content type)
	std::string contentTypeHeader = "Content-Type: application/octet-stream";

	// Array of accept types
	const char* AcceptTypes[] = { "application/json", NULL };

	// Return Error Code
	DWORD dwErrorCode = ERROR_SUCCESS;

	// Internet handles
	HINTERNET hInternet, hConnect, hRequest;

	// Whether or not we are using SSL
	BOOL bUseSSL = TRUE;

	// Shortened server name without the protocol prefix
	std::string server;

	// Check to make sure that we have a valid API key, server address, and server port
	// and build the Authorization Header
	if (MetaDefenderApiKey.length() > 0 && MetaDefenderServer.length() > 0 && MetaDefenderPort > 0) {
		authTokenHeader += MetaDefenderApiKey;
	}
	else {
		return NULL;
	}

	// Add all the headers together
	std::string headers = authTokenHeader + "\r\n" + contentTypeHeader;

	// Check to make sure we are connected to the Internet...
	DWORD dwInternetConnectionFlags = 0;
	if (!InternetGetConnectedState(&dwInternetConnectionFlags, 0)) {
		dwErrorCode = MD_ERROR_NO_INTERNET;
		SetLastError(dwErrorCode);
		return NULL;
	}

	// Check and see if we are to use SSL
	if (MetaDefenderServer.rfind("http://", 0) != std::string::npos) {
		bUseSSL = FALSE;
	}
	else if (MetaDefenderServer.rfind("https://")) {
		bUseSSL = TRUE;
	}

	// Remove the protocol specification from the front of the server name
	server = MetaDefenderServer.substr(MetaDefenderServer.find_last_of("/", 0), std::string::npos);

	// Open the connection
	hInternet = InternetOpenA("MetaDefender X-Ways Forensics Plugin",
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0);

	if (hInternet) {
		// Connect
		hConnect = InternetConnectA(hInternet,
			server.c_str(),
			(INTERNET_PORT)MetaDefenderPort,
			NULL,
			NULL,
			INTERNET_SERVICE_HTTP,
			0,
			0);

		if (hConnect) {
			if (bUseSSL) {
				// Make the HTTP SSL connection
				hRequest = HttpOpenRequestA(hConnect,
					"POST",
					URI.c_str(),
					NULL,
					NULL,
					AcceptTypes,
					INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID,
					0);
			}
			else {
				hRequest = HttpOpenRequestA(hConnect,
					"POST",
					URI.c_str(),
					NULL,
					NULL,
					AcceptTypes,
					INTERNET_FLAG_NO_CACHE_WRITE,
					0);
			}

			if (hRequest) {
				// Send the HTTP request
				if (HttpSendRequestA(hRequest, headers.c_str(), (DWORD)headers.length(), fileBuffer, dwFileSize)) {
					DWORD dwContentLength = 0;
					DWORD length = sizeof(DWORD);
					
					// Get the content length and make sure our buffer is big enough to handle it
					HttpQueryInfo(hRequest, 
						HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
						&dwContentLength,
						&length,
						NULL);

					// Get the status code
					DWORD dwStatusCode = 0;
					length = sizeof(DWORD);

					HttpQueryInfo(hRequest,
						HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, 
						&dwStatusCode, 
						&length, NULL);

					
					if (dwStatusCode == 200) {
						ResponseBuffer = (char*)malloc(dwContentLength);
						if (ResponseBuffer != NULL) {
							ZeroMemory(ResponseBuffer, dwContentLength);

							DWORD dwBytesRead = 0;

							if (InternetReadFile(hRequest, ResponseBuffer, dwContentLength, &dwBytesRead)) {
								if (dwBytesRead == dwContentLength) {
									*dwResponseSize = dwContentLength;
									dwErrorCode = ERROR_SUCCESS;
								}
								else {
									*dwResponseSize = 0;
									free(ResponseBuffer);
									ResponseBuffer = NULL;
									dwErrorCode = MD_ERROR_HTTP;
								}
							}
							else {
								*dwResponseSize = 0;
								free(ResponseBuffer);
								ResponseBuffer = NULL;
								dwErrorCode = MD_ERROR_HTTP;
							}
						}
						else {
							*dwResponseSize = 0;
							dwErrorCode = MD_ERROR_MEMORY_BUFFER;
						}
					}
					else if (dwStatusCode == 404) {
						dwErrorCode = MD_ERROR_FILE_NOT_FOUND;
					}
					else if (dwStatusCode == 403) {
						dwErrorCode = MD_ERROR_AUTHORIZATION;
					}
				}
				else {
					DWORD dwError = GetLastError();
				}
				InternetCloseHandle(hRequest);
			}
			else {
				DWORD dwError = GetLastError();
				dwErrorCode = MD_ERROR_HTTP;
			}
			InternetCloseHandle(hConnect);
		}
		else {
			dwErrorCode = MD_ERROR_CONNECTING;
		}
		InternetCloseHandle(hInternet);
	}
	else {
		dwErrorCode = MD_ERROR_CONNECTING;
	}

	// Set the error code
	SetLastError(dwErrorCode);

	// Return success
	return ResponseBuffer;
}

BOOL UploadFile(PVOID fileContents, DWORD dwFileSize, std::string* fileURL)
{
	BOOL bResult = FALSE;
	DWORD dwResponseLength = 0;
	char* server_response = MD_UploadFile(UploadFileURI, (PVOID)fileContents, dwFileSize, &dwResponseLength);

	if (server_response != NULL) {
		cJSON* responseJSON = cJSON_ParseWithLength(server_response, (size_t)dwResponseLength);

		if (responseJSON != NULL) {
			cJSON* dataId = cJSON_GetObjectItemCaseSensitive(responseJSON, "data_id");
			if (dataId != NULL && cJSON_IsString(dataId)) {
				*fileURL = MetaDefenderServer;
				*fileURL += "/v4/file/";
				*fileURL += dataId->valuestring;
				bResult = TRUE;
			}
			else {
				PrintErrorMessage(MD_ERROR_INVALID_RESPONSE);
			}

			cJSON_Delete(responseJSON);
		}
		else {
			PrintErrorMessage(MD_ERROR_INVALID_RESPONSE);
		}

		free(server_response);
	}
	else {
		PrintErrorMessage(GetLastError());
	}

	return bResult;
}

// Print an error message to the user in the X-Ways Messages window
void PrintErrorMessage(DWORD dwErrorCode) {
	switch (dwErrorCode) {
	case MD_ERROR_AUTHORIZATION:
		XWF_OutputMessage(L"Error performing operation: Unauthorized", 0);
		break;
	case MD_ERROR_BUFFER_OVERRUN:
		XWF_OutputMessage(L"Error performing operation: Buffer size too small for server response.", 0);
		break;
	case MD_ERROR_CONNECTING:
		XWF_OutputMessage(L"Error connecting to server.", 0);
		break;
	case MD_ERROR_FILE_NOT_FOUND:
		XWF_OutputMessage(L"Error: File not found in MetaDefender.", 0);
		break;
	case MD_ERROR_HTTP:
		XWF_OutputMessage(L"Error performing operation: Unable to establish HTTP connection.", 0);
		break;
	case MD_ERROR_NO_INTERNET:
		XWF_OutputMessage(L"Error: Internet connection currently unavailable.", 0);
		break;
	case MD_ERROR_MEMORY_BUFFER:
		XWF_OutputMessage(L"Error: Unable to allocate memory for operation.", 0);
		break;
	case MD_ERROR_DATA_CONVERSION:
		XWF_OutputMessage(L"Error converting data format", 0);
		break;
	case MD_ERROR_NO_CONFIG:
		XWF_OutputMessage(L"Error: Server configuration file not found or invalid format.", 0);
		break;
	case MD_ERROR_INVALID_RESPONSE:
		XWF_OutputMessage(L"The MetaDefender server returned a response that this XTension does not understand.", 0);
		break;
	case MD_ERROR_UNKNOWN:
	default:
		XWF_OutputMessage(L"An unknown error occurred during operation.", 0);
	}
}

BOOL ReadAPIConfigFile(HMODULE hModule) {
	// Return value
	BOOL bSuccess = false;

	// Get the Module Path
	wchar_t FilePath[MAX_PATH];
	GetModuleFileName(hModule, FilePath, MAX_PATH);

	// Zero out the module filename and append the config file name
	wchar_t* substr = wcsrchr(FilePath, '\\');
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
			char* ConfigFileBuffer = (char*)VirtualAlloc(NULL, dwFileSize, MEM_COMMIT, PAGE_READWRITE);
			DWORD dwNumberOfBytesRead = 0;

			if (ReadFile(hFile, ConfigFileBuffer, dwFileSize, &dwNumberOfBytesRead, NULL)) {
				// File should contain single line of the format <hostname>:<port>:<API key>
				int index = 0;

				// Read the tokenized string from the config file
				char* nextToken = NULL;
				char* token = strtok_s(ConfigFileBuffer, "|", &nextToken);

				while (token != NULL) {
					switch (index) {
					case 0:
						MetaDefenderServer += token;
						break;
					case 1:
						MetaDefenderPort = atoi(token);
						break;
					case 2:
						MetaDefenderApiKey += token;
						break;
					}
					index++;
					token = strtok_s(NULL, "|", &nextToken);
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