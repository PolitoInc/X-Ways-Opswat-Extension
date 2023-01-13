#pragma once

// Global Variables
extern wchar_t MetaDefenderServer[1024];
extern DWORD MetaDefenderPort;
extern wchar_t MetaDefenderApiKey[41];

// Function to read in the config file and store the config values in global vars
BOOL ReadAPIConfigFile(HMODULE hModule);

// Function to read in the config file with username/password and store the config values in global vars
BOOL ReadUsernamePasswordConfigFile(HMODULE hModule);

// Function to convert the binary hash value to human readable format
wchar_t * GetHashString(LONG nItemID, INT64 hashType);