#include "stdafx.h"

/* STATIC HTTP REQUEST URIs*/
const static wchar_t * fmtGetFileReputationURI = L"/v4/hash/%s";
const static wchar_t * UploadFileURI = L"/api/uploads/";

// Get information from the server
// Returns a char * buffer that is allocated in this function; caller's obligation to free the buffer
char* MD_Get(wchar_t * URI) {
	// The buffer to hold the response
	char* ResponseBuffer = NULL;

	// Header (Auth token)
	const static wchar_t * fmtHeader = L"apikey: %s";

	// Array of accept types
	PCWSTR AcceptTypes[] = { L"application/json", NULL };

	// Return Error Code
	DWORD dwErrorCode = ERROR_SUCCESS;

	// Internet handles
	HINTERNET hInternet, hConnect, hRequest;

	// Authorization Header
	wchar_t * AuthHeader = NULL;

	// Authorization Data
	char * szAuthString = NULL;

	// Base64 encoded Authorization String data (multibyte string)
	char * szB64AuthString = NULL;

	// Base64 encoded Authorization String data (unicode string)
	wchar_t * B64AuthString = NULL;

	// Check to make sure that we have a valid API key, server address, and server port
	// and build the Authorization Header
	if (wcslen(MetaDefenderApiKey) > 0 && wcslen(MetaDefenderServer) > 0 && MetaDefenderPort > 0) {
		AuthHeader = (wchar_t *)malloc(128 * sizeof(wchar_t));
		ZeroMemory(AuthHeader, 128 * sizeof(wchar_t));
		StringCchPrintf(AuthHeader, 128, fmtHeader, MetaDefenderApiKey);
	}
	// Check to make sure we are connected to the Internet...
	DWORD dwInternetConnectionFlags = 0;
	if (!InternetGetConnectedState(&dwInternetConnectionFlags, 0)) {
		dwErrorCode = MD_ERROR_NO_INTERNET;
		SetLastError(dwErrorCode);
		return false;
	}

	// Open the connection
	hInternet = InternetOpen(L"MetaDefender X-Ways Forensics Plugin",
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0);

	if (hInternet) {
		// Connect
		hConnect = InternetConnect(hInternet,
			MetaDefenderServer,
			(INTERNET_PORT)MetaDefenderPort,
			NULL,
			NULL,
			INTERNET_SERVICE_HTTP,
			0,
			0);

		if (hConnect) {
			// Make the HTTP SSL connection
			hRequest = HttpOpenRequestW(hConnect,
				L"GET",
				URI,
				NULL,
				NULL,
				AcceptTypes,
				INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID,
				0);

			if (hRequest) {
				// Send the HTTP request
				if (HttpSendRequestW(hRequest, AuthHeader, (DWORD)wcslen(AuthHeader), NULL, 0)) {
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
						ResponseBuffer = (char*)malloc(dwContentLength + 1);
						ZeroMemory(ResponseBuffer, dwContentLength + 1);

						DWORD dwBytesRead = 0;
						DWORD offset = 0;

						// Read the response
						DWORD dwBufRemaining = dwContentLength;

						while (dwBufRemaining > 0) {
							if (dwBufRemaining > 1024)
								InternetReadFile(hRequest, ResponseBuffer + offset, 1024, &dwBytesRead);
							else
								InternetReadFile(hRequest, ResponseBuffer + offset, dwBufRemaining, &dwBytesRead);

							if (dwBytesRead == 0) {
								break;
							} else {
								if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
									dwErrorCode = MD_ERROR_BUFFER_OVERRUN;
								}
								offset += dwBytesRead;
								dwBufRemaining -= dwBytesRead;
							}
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
					int foo = 0;
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

	// Free the Auth Header
	if (AuthHeader != NULL) {
		free(AuthHeader);
		AuthHeader = NULL;
	}

	// Set the error code
	SetLastError(dwErrorCode);

	// Return success
	return ResponseBuffer;
}

// Post data to the server
BOOL MD_Post(const wchar_t * URI, char * ResponseBuffer, LPVOID PostBuffer, DWORD PostBufferLength) {
	// Header (Auth token & Content Type)
	const static wchar_t * fmtHeader = L"apikey: %s\r\nContent-Type: multipart/form-data; boundary=XWFMDAPIj3QgbJtcq4lbDkn7\r\nConnection: keep-alive\r\n";

	// Array of accept types
	PCWSTR AcceptTypes[] = { L"*/*", NULL };

	// Return value
	BOOL bSuccess = false;

	// Return Error Code
	DWORD dwErrorCode = ERROR_SUCCESS;

	// Internet handles
	HINTERNET hInternet, hConnect, hRequest;

	// Authorization Header
	wchar_t * Header;

	// Check to make sure that we have a valid API key, server address, and server port
	// and build the Authorization Header
	if (wcslen(MetaDefenderApiKey) > 0 && wcslen(MetaDefenderServer) > 0 && MetaDefenderPort > 0) {
		Header = (wchar_t *)malloc(256 * sizeof(wchar_t));
		ZeroMemory(Header, 256 * sizeof(wchar_t));
		StringCchPrintf(Header, 256, fmtHeader, MetaDefenderApiKey);
	}
	else {
		dwErrorCode = MD_ERROR_NO_CONFIG;
		SetLastError(dwErrorCode);
		return false;
	}

	// Check to make sure we are connected to the Internet...
	DWORD dwInternetConnectionFlags = 0;
	if (!InternetGetConnectedState(&dwInternetConnectionFlags, 0)) {
		dwErrorCode = MD_ERROR_NO_INTERNET;
		SetLastError(dwErrorCode);
		return false;
	}

	// Open the connection
	hInternet = InternetOpen(L"MetaDefender X-Ways Forensics Plugin",
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0);

	if (hInternet) {
		// Connect
		hConnect = InternetConnect(hInternet,
			MetaDefenderServer,
			(INTERNET_PORT)MetaDefenderPort,
			NULL,
			NULL,
			INTERNET_SERVICE_HTTP,
			0,
			0);

		if (hConnect) {
			// Make the HTTP SSL connection
			hRequest = HttpOpenRequestW(hConnect,
				L"POST",
				URI,
				NULL,
				NULL,
				AcceptTypes,
				INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID,
				0);

			if (hRequest) {
				// Send the HTTP request
				if (HttpSendRequestW(hRequest, Header, (DWORD)wcslen(Header), PostBuffer, PostBufferLength)) {
					// Buffer to hold the response status code
					DWORD dwResponseTextSize = 16 * sizeof(wchar_t);
					wchar_t* QueryResult = (wchar_t*)malloc(dwResponseTextSize);

					if (QueryResult == NULL) {
						return NULL;
					}

					ZeroMemory(QueryResult, dwResponseTextSize);

					// Get the content length and make sure our buffer is big enough to handle it
					HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH, QueryResult, &dwResponseTextSize, NULL);

					long ContentLength = _wtol(QueryResult);

					ResponseBuffer = (char*)malloc(ContentLength);

					if (ResponseBuffer == NULL) {
						free(QueryResult);
						return NULL;
					}

					ZeroMemory(ResponseBuffer, ContentLength);

					// Query the response header
					HttpQueryInfoW(hRequest, HTTP_QUERY_STATUS_CODE, QueryResult, &dwResponseTextSize, NULL);

					if (wcscmp(QueryResult, L"200") == 0 || wcscmp(QueryResult, L"201") == 0) {
						DWORD dwBytesRead = 0;
						DWORD offset = 0;

						// Read the response
						while (InternetReadFile(hRequest, ResponseBuffer + offset, 1024, &dwBytesRead)) {
							if (dwBytesRead == 0) {
								break;
							}
							else {
								if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
									dwErrorCode = MD_ERROR_BUFFER_OVERRUN;
								}
								offset += dwBytesRead;
							}
						}

						// Completed reading the response
						bSuccess = true;
					}
					else if (wcscmp(QueryResult, L"404") == 0) {
						dwErrorCode = MD_ERROR_FILE_NOT_FOUND;
					}
					else if (wcscmp(QueryResult, L"403") == 0) {
						dwErrorCode = MD_ERROR_AUTHORIZATION;
					}
					else {
						dwErrorCode = MD_ERROR_UNKNOWN;
					}

					free(QueryResult);
				}
				else {
					DWORD dwError = GetLastError();
					int foo = 0;
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
	return bSuccess;
}

// Get the File Reputation Score
wchar_t * GetFileReputation(wchar_t * FileHash, LONG nItemID) {
	// TODO: Need to get an idea of maximum / mean / median response size from RL
	// and maybe start with the median size for this buffer and then maybe 
	// implement code to bump it up if the query fails because the response is
	// too big.
	wchar_t * URI = (wchar_t *)malloc(MD_MAX_URL_LENGTH * sizeof(wchar_t));
	ZeroMemory(URI, MD_MAX_URL_LENGTH * sizeof(wchar_t));

	StringCchPrintfW(URI, MD_MAX_URL_LENGTH, fmtGetFileReputationURI, FileHash);

	// Make the request
	char* ResponseBuffer = MD_Get(URI);

	if (ResponseBuffer != NULL) {
		// Parse the JSON response

		//XWF_OutputMessage((wchar_t *)ResponseBuffer, 0x4);

		// Create a map to hold the parsed JSON tokens
		std::map<char *, char *>ParsedResponse;

		// The JSON keys that we are interested in extracting
		char * KEYS[] = {
			"threat_name",
			"scan_all_result_i",
			"total_avs",
			"total_detected_avs"
		};

		if (ParseJSONResponse(ResponseBuffer, &ParsedResponse, KEYS, 4)) {
			std::map<char *, char *>::iterator it;

			it = ParsedResponse.find("scan_all_result_i");
			if (it != ParsedResponse.end()) {
				// The caption for this metadata
				wchar_t* Caption = L"Scan Result:";
				wchar_t* Value = NULL;

				int Result = atoi(it->second);

				switch (Result) {
				case 0:
					Value = L"No Threat Found";
					break;
				case 1: 
					Value = L"Infected";
					break;
				case 2:
					Value = L"Suspicious";
					break;
				case 3:
					Value = L"Scan Failed";
					break;
				case 4:
					Value = L"Cleaned / Deleted";
					break;
				case 5:
					Value = L"Unknown";
					break;
				case 6:
					Value = L"Quarantined";
					break;
				case 7:
					Value = L"Whitelisted";
					break;
				case 8:
					Value = L"Infected";
					break;
				case 9:
					Value = L"Exceeded Archive Depth";
					break;
				case 10:
					Value = L"No scan results";
					break;
				case 11:
					Value = L"Aborted";
					break;
				case 12:
					Value = L"Encrypted";
					break;
				case 13:
					Value = L"Exceeded Archive Size";
					break;
				case 14:
					Value = L"Exceeded Archive File Count";
					break;
				case 15:
					Value = L"Password Protected Document";
					break;
				case 16:
					Value = L"Exceeded Archive Timeout";
					break;
				case 17:
					Value = L"File-Type Mismatch";
					break;
				case 18:
					Value = L"Potentially Vulnerable File";
					break;
				case 19:
					Value = L"Scan Canceled";
					break;
				case 20:
					Value = L"DLP Sensitive Data Detected";
					break;
				case 21:
					Value = L"Yara Rule Matched";
					break;
				case 22:
					Value = L"Potentially Unwanted Application (PUA)";
					break;
				case 23:
					Value = L"File-Type Not Supported";
					break;
				case 254:
					Value = L"In Queue";
					break;
				case 255:
					Value = L"In Progress";
					break;
				default:
					Value = L"Unknown / Invalid Status Code Received from MetaDefender";
					break;
				}

				// Buffer to hold the final comment to be inserted into XWF
				size_t FinalCommentLengthInCharacters = wcslen(Value) + wcslen(Caption) + 2;
				size_t FinalCommentLengthInBytes = FinalCommentLengthInCharacters * sizeof(wchar_t);
				wchar_t* comment = (wchar_t*)malloc(FinalCommentLengthInBytes);
				ZeroMemory(comment, FinalCommentLengthInBytes);

				// Format the final comment string
				StringCchPrintf(comment, FinalCommentLengthInCharacters, L"%s %s", Caption, Value);

				// Add the comment
				if (XWF_GetExtractedMetadata(nItemID) == NULL || wcsstr(XWF_GetExtractedMetadata(nItemID), Caption) == NULL) {
					XWF_AddExtractedMetadata(nItemID, comment, 2);
				}

				// If the threat status is malicious, use X-Ways internal API to flag the file as "known bad hash category"
				if (Result == 1 || Result == 2 || Result == 4 || Result == 6 || Result == 8) {
					BOOL bSuccess = false;
					INT64 iXwFlags = XWF_GetItemInformation(nItemID, XWF_ITEM_INFO_FLAGS, &bSuccess);
					if (bSuccess)
						XWF_SetItemInformation(nItemID, XWF_ITEM_INFO_FLAGS, iXwFlags | 0x00400000);
				}

				// Free the buffers
				free(comment);
				comment = NULL;
			}

			it = ParsedResponse.find("threat_name");
			if (it != ParsedResponse.end()) {
				// The caption for this metadata
				wchar_t* Caption = L"Threat Name:";

				// Get the required buffer size
				int len = MultiByteToWideChar(CP_OEMCP, 0, it->second, -1, NULL, 0);

				// Buffer to hold the JSON value after conversion to wchar_t
				wchar_t* value = (wchar_t*)malloc(len * sizeof(wchar_t));
				ZeroMemory(value, len * sizeof(wchar_t));

				// Convert the text to wchar_t
				MultiByteToWideChar(CP_OEMCP, 0, it->second, -1, value, len);

				// Buffer to hold the final comment to be inserted into XWF
				size_t FinalCommentLengthInCharacters = len + wcslen(Caption) + 2;
				size_t FinalCommentLengthInBytes = FinalCommentLengthInCharacters * sizeof(wchar_t);
				wchar_t* comment = (wchar_t*)malloc(FinalCommentLengthInBytes);
				ZeroMemory(comment, FinalCommentLengthInBytes);

				// Format the final comment string
				StringCchPrintf(comment, FinalCommentLengthInBytes, L"%s %s", Caption, value);

				// Add the comment
				if (XWF_GetExtractedMetadata(nItemID) == NULL || wcsstr(XWF_GetExtractedMetadata(nItemID), Caption) == NULL) {
					XWF_AddExtractedMetadata(nItemID, comment, 2);
				}

				// Free the buffers
				free(comment);
				comment = NULL;

				free(value);
				value = NULL;
			}

			it = ParsedResponse.find("total_avs");
			if (it != ParsedResponse.end()) {
				// The caption for this metadata
				wchar_t* Caption = L"Total Engines:";

				// Get the required buffer size
				int len = MultiByteToWideChar(CP_OEMCP, 0, it->second, -1, NULL, 0);

				// Buffer to hold the JSON value after conversion to wchar_t
				wchar_t * value = (wchar_t *)malloc(len * sizeof(wchar_t));
				ZeroMemory(value, len * sizeof(wchar_t));

				// Convert the text to wchar_t
				MultiByteToWideChar(CP_OEMCP, 0, it->second, -1, value, len);

				// Buffer to hold the final comment to be inserted into XWF
				size_t FinalCommentLengthInCharacters = len + wcslen(Caption) + 2;
				size_t FinalCommentLengthInBytes = FinalCommentLengthInCharacters * sizeof(wchar_t);
				wchar_t* comment = (wchar_t*)malloc(FinalCommentLengthInBytes);
				ZeroMemory(comment, FinalCommentLengthInBytes);

				// Format the final comment string
				StringCchPrintf(comment, FinalCommentLengthInBytes, L"%s %s", Caption, value);

				// Add the comment
				if (XWF_GetExtractedMetadata(nItemID) == NULL || wcsstr(XWF_GetExtractedMetadata(nItemID), Caption) == NULL) {
					XWF_AddExtractedMetadata(nItemID, comment, 2);
				}

				// Free the buffers
				free(comment);
				comment = NULL;

				free(value);
				value = NULL;
			}

			it = ParsedResponse.find("total_detected_avs");
			if (it != ParsedResponse.end()) {
				// The caption for this metadata
				wchar_t* Caption = L"Total Detecting Engines:";

				// Get the required buffer size
				int len = MultiByteToWideChar(CP_OEMCP, 0, it->second, -1, NULL, 0);

				// Buffer to hold the JSON value after conversion to wchar_t
				wchar_t * value = (wchar_t *)malloc(len * sizeof(wchar_t));
				ZeroMemory(value, len * sizeof(wchar_t));

				// Convert the text to wchar_t
				MultiByteToWideChar(CP_OEMCP, 0, it->second, -1, value, len);

				// Buffer to hold the final comment to be inserted into XWF
				size_t FinalCommentLengthInCharacters = len + wcslen(Caption) + 2;
				size_t FinalCommentLengthInBytes = FinalCommentLengthInCharacters * sizeof(wchar_t);
				wchar_t* comment = (wchar_t*)malloc(FinalCommentLengthInBytes);
				ZeroMemory(comment, FinalCommentLengthInBytes);

				// Format the final comment string
				StringCchPrintf(comment, FinalCommentLengthInBytes, L"%s %s", Caption, value);

				// Add the comment
				if (XWF_GetExtractedMetadata(nItemID) == NULL || wcsstr(XWF_GetExtractedMetadata(nItemID), Caption) == NULL) {
					XWF_AddExtractedMetadata(nItemID, comment, 2);
				}

				// Free the buffers
				free(comment);
				comment = NULL;

				free(value);
				value = NULL;
			}

			XWF_OutputMessage(L"File hash successfully queried against MetaDefender. Check the Metadata column in X-Ways for status information.", 0);
		}
		else {
			// Mark the file as unknown in metadefender
			if (XWF_GetExtractedMetadata(nItemID) == NULL || wcsstr(XWF_GetExtractedMetadata(nItemID), L"MetaDefender: Unknown") == NULL) {
				XWF_AddExtractedMetadata(nItemID, L"MetaDefender: Unknown", 2);
			}
			return NULL;
		}
	}
	else {
		DWORD dwErrorCode = GetLastError();
		if (dwErrorCode != MD_ERROR_FILE_NOT_FOUND) {
			// TODO: Check if the error was because the buffer was too small. If so, recurse.
			PrintErrorMessage(dwErrorCode);
		}
		else {
			// Mark the file as unknown in metadefender
			if (XWF_GetExtractedMetadata(nItemID) == NULL || wcsstr(XWF_GetExtractedMetadata(nItemID), L"MetaDefender: Unknown") == NULL) {
				XWF_AddExtractedMetadata(nItemID, L"MetaDefender: Unknown", 2);
			}
		}
		return NULL;
	}

	// Free the buffers we created
	if (URI != NULL) {
		free(URI);
		URI = NULL;
	}

	if (ResponseBuffer != NULL) {
		free(ResponseBuffer);
		ResponseBuffer = NULL;
	}

	return NULL;
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