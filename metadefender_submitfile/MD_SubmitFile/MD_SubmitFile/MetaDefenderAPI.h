#pragma once

#define MD_ERROR_CONNECTING 0x00000001
#define MD_ERROR_AUTHORIZATION 0x00000002
#define MD_ERROR_HTTP 0x00000003
#define MD_ERROR_BUFFER_OVERRUN 0x00000004
#define MD_ERROR_FILE_NOT_FOUND 0x00000005
#define MD_ERROR_NO_CONFIG 0x00000006
#define MD_ERROR_NO_INTERNET 0x00000007
#define MD_ERROR_MEMORY_BUFFER 0x00000008
#define MD_ERROR_DATA_CONVERSION 0x00000009
#define MD_ERROR_UNKNOWN 0x00000010
#define MD_ERROR_INVALID_RESPONSE 0x00000011

#define MD_GET_FILEREPUTATION 0x00000001
#define MD_GET_MULTIAVSCANRESULTS 0X00000002
#define MD_GET_FILEREPUTATIONEX 0x00000003
#define MD_PUT_FILE 0x00000004
#define MD_RESCAN_FILE 0x00000005
#define MD_DELETE_FILE 0x00000006
#define MD_FIND_SIMILARFILES 0x00000007

#define MD_MAX_SUBMIT_FILE_SIZE 10000000

#define MD_AUTH_TYPE_TOKEN 0
#define MD_AUTH_TYPE_BASIC 1
#define MD_AUTH_HEADER_LEN 1024

#define MD_MAX_URL_LENGTH 1024

#define MAX_THREAT_NAME 64

typedef struct _MD_RESULT {
	int scan_all_result_i;
	int total_avs;
	int total_detected_avs;
	char threat_name[MAX_THREAT_NAME];
} MD_RESULT, *PMD_RESULT;


// Upload a file
BOOL UploadFile(PVOID FileContents, DWORD FileSize, std::string* FileURL);

// Print an error message
void PrintErrorMessage(DWORD dwErrorCode);

// Read in the configuration file containing the API key
BOOL ReadAPIConfigFile(HMODULE hModule);

// Retrieve the hash value for an X-ways item (in string format)
wchar_t* GetHashString(LONG nItemID, INT64 hashType);