#include "stdafx.h"

int Base64decode_len(const char *bufcoded);
int Base64decode(char *bufplain, const char *bufcoded);
int Base64encode_len(int len);
__int64 Base64encode(char *encoded, const char *string, int len);