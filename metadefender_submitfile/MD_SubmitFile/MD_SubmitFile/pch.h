// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include <string>
#include "XWF.h"
#include "MetaDefenderAPI.h"
#include <WinInet.h>
#include <strsafe.h>
#include <stdlib.h>
#include "cJSON.h"

// Limit max upload file size to 10MB
#define MD_MAX_SUBMIT_FILE_SIZE 10000000

#endif //PCH_H
