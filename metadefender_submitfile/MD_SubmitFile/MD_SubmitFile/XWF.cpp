#include "pch.h"

fptr_XWF_GetSize XWF_GetSize;
fptr_XWF_GetVolumeName XWF_GetVolumeName;
fptr_XWF_GetVolumeInformation XWF_GetVolumeInformation;
fptr_XWF_GetSectorContents XWF_GetSectorContents;
fptr_XWF_Read XWF_Read;
fptr_XWF_SelectVolumeSnapshot XWF_SelectVolumeSnapshot;
fptr_XWF_GetVSProp XWF_GetVSProp;
fptr_XWF_GetItemCount XWF_GetItemCount;
fptr_XWF_CreateItem XWF_CreateItem;
fptr_XWF_GetItemName XWF_GetItemName;
fptr_XWF_GetItemSize XWF_GetItemSize;
fptr_XWF_SetItemSize XWF_SetItemSize;
fptr_XWF_GetItemOfs XWF_GetItemOfs;
fptr_XWF_SetItemOfs XWF_SetItemOfs;
fptr_XWF_GetItemInformation XWF_GetItemInformation;
fptr_XWF_SetItemInformation XWF_SetItemInformation;
fptr_XWF_GetItemType XWF_GetItemType;
fptr_XWF_SetItemType XWF_SetItemType;
fptr_XWF_GetItemParent XWF_GetItemParent;
fptr_XWF_SetItemParent XWF_SetItemParent;
fptr_XWF_GetReportTableAssocs XWF_GetReportTableAssocs;
fptr_XWF_AddToReportTable XWF_AddToReportTable;
fptr_XWF_GetComment XWF_GetComment;
fptr_XWF_AddComment XWF_AddComment;
fptr_XWF_OutputMessage XWF_OutputMessage;
fptr_XWF_ShowProgress XWF_ShowProgress;
fptr_XWF_SetProgressPercentage XWF_SetProgressPercentage;
fptr_XWF_SetProgressDescription XWF_SetProgressDescription;
fptr_XWF_ShouldStop XWF_ShouldStop;
fptr_XWF_HideProgress XWF_HideProgress;

fptr_XWF_GetBlock XWF_GetBlock;
fptr_XWF_SetBlock XWF_SetBlock;
fptr_XWF_GetCaseProp XWF_GetCaseProp;
fptr_XWF_GetFirstEvObj XWF_GetFirstEvObj;
fptr_XWF_GetNextEvObj XWF_GetNextEvObj;
fptr_XWF_OpenEvObj XWF_OpenEvObj;
fptr_XWF_CloseEvObj XWF_CloseEvObj;
fptr_XWF_GetEvObjProp XWF_GetEvObjProp;
fptr_XWF_GetExtractedMetadata XWF_GetExtractedMetadata;
fptr_XWF_GetMetadata XWF_GetMetadata;
fptr_XWF_AddExtractedMetadata XWF_AddExtractedMetadata;
fptr_XWF_GetHashValue XWF_GetHashValue;
fptr_XWF_AddEvent XWF_AddEvent;
fptr_XWF_GetReportTableInfo XWF_GetReportTableInfo;
fptr_XWF_GetEvObjReportTableAssocs XWF_GetEvObjReportTableAssocs;


///////////////////////////////////////////////////////////////////////////////
// XT_RetrieveFunctionPointers - call this function before calling anything else

LONG __stdcall XT_RetrieveFunctionPointers()
{
	HMODULE Hdl = GetModuleHandle(NULL);

	XWF_GetSize = (fptr_XWF_GetSize)GetProcAddress(Hdl, "XWF_GetSize");
	XWF_GetVolumeName = (fptr_XWF_GetVolumeName)GetProcAddress(Hdl, "XWF_GetVolumeName");
	XWF_GetVolumeInformation = (fptr_XWF_GetVolumeInformation)GetProcAddress(Hdl, "XWF_GetVolumeInformation");
	XWF_GetSectorContents = (fptr_XWF_GetSectorContents)GetProcAddress(Hdl, "XWF_GetSectorContents");
	XWF_Read = (fptr_XWF_Read)GetProcAddress(Hdl, "XWF_Read");

	XWF_SelectVolumeSnapshot = (fptr_XWF_SelectVolumeSnapshot)GetProcAddress(Hdl, "XWF_SelectVolumeSnapshot");
	XWF_GetVSProp = (fptr_XWF_GetVSProp)GetProcAddress(Hdl, "XWF_GetVSProp");
	XWF_GetItemCount = (fptr_XWF_GetItemCount)GetProcAddress(Hdl, "XWF_GetItemCount");

	XWF_CreateItem = (fptr_XWF_CreateItem)GetProcAddress(Hdl, "XWF_CreateItem");
	XWF_GetItemName = (fptr_XWF_GetItemName)GetProcAddress(Hdl, "XWF_GetItemName");
	XWF_GetItemSize = (fptr_XWF_GetItemSize)GetProcAddress(Hdl, "XWF_GetItemSize");
	XWF_SetItemSize = (fptr_XWF_SetItemSize)GetProcAddress(Hdl, "XWF_SetItemSize");
	XWF_GetItemOfs = (fptr_XWF_GetItemOfs)GetProcAddress(Hdl, "XWF_GetItemOfs");
	XWF_SetItemOfs = (fptr_XWF_SetItemOfs)GetProcAddress(Hdl, "XWF_SetItemOfs");
	XWF_GetItemInformation = (fptr_XWF_GetItemInformation)GetProcAddress(Hdl, "XWF_GetItemInformation");
	XWF_SetItemInformation = (fptr_XWF_SetItemInformation)GetProcAddress(Hdl, "XWF_SetItemInformation");
	XWF_GetItemType = (fptr_XWF_GetItemType)GetProcAddress(Hdl, "XWF_GetItemType");
	XWF_SetItemType = (fptr_XWF_SetItemType)GetProcAddress(Hdl, "XWF_SetItemType");
	XWF_GetItemParent = (fptr_XWF_GetItemParent)GetProcAddress(Hdl, "XWF_GetItemParent");
	XWF_SetItemParent = (fptr_XWF_SetItemParent)GetProcAddress(Hdl, "XWF_SetItemParent");
	XWF_GetReportTableAssocs = (fptr_XWF_GetReportTableAssocs)GetProcAddress(Hdl, "XWF_GetReportTableAssocs");
	XWF_AddToReportTable = (fptr_XWF_AddToReportTable)GetProcAddress(Hdl, "XWF_AddToReportTable");
	XWF_GetComment = (fptr_XWF_GetComment)GetProcAddress(Hdl, "XWF_GetComment");
	XWF_AddComment = (fptr_XWF_AddComment)GetProcAddress(Hdl, "XWF_AddComment");

	XWF_OutputMessage = (fptr_XWF_OutputMessage)GetProcAddress(Hdl, "XWF_OutputMessage");
	XWF_ShowProgress = (fptr_XWF_ShowProgress)GetProcAddress(Hdl, "XWF_ShowProgress");
	XWF_SetProgressPercentage = (fptr_XWF_SetProgressPercentage)GetProcAddress(Hdl, "XWF_SetProgressPercentage");
	XWF_SetProgressDescription = (fptr_XWF_SetProgressDescription)GetProcAddress(Hdl, "XWF_SetProgressDescription");
	XWF_ShouldStop = (fptr_XWF_ShouldStop)GetProcAddress(Hdl, "XWF_ShouldStop");
	XWF_HideProgress = (fptr_XWF_HideProgress)GetProcAddress(Hdl, "XWF_HideProgress");

	XWF_GetBlock = (fptr_XWF_GetBlock)GetProcAddress(Hdl, "XWF_GetBlock");
	XWF_SetBlock = (fptr_XWF_SetBlock)GetProcAddress(Hdl, "XWF_SetBlock");
	XWF_GetCaseProp = (fptr_XWF_GetCaseProp)GetProcAddress(Hdl, "XWF_GetCaseProp");
	XWF_GetFirstEvObj = (fptr_XWF_GetFirstEvObj)GetProcAddress(Hdl, "XWF_GetFirstEvObj");
	XWF_GetNextEvObj = (fptr_XWF_GetNextEvObj)GetProcAddress(Hdl, "XWF_GetNextEvObj");
	XWF_OpenEvObj = (fptr_XWF_OpenEvObj)GetProcAddress(Hdl, "XWF_OpenEvObj");
	XWF_CloseEvObj = (fptr_XWF_CloseEvObj)GetProcAddress(Hdl, "XWF_CloseEvObj");
	XWF_GetEvObjProp = (fptr_XWF_GetEvObjProp)GetProcAddress(Hdl, "XWF_GetEvObjProp");
	XWF_GetExtractedMetadata = (fptr_XWF_GetExtractedMetadata)GetProcAddress(Hdl, "XWF_GetExtractedMetadata");
	XWF_GetMetadata = (fptr_XWF_GetMetadata)GetProcAddress(Hdl, "XWF_GetMetadata");
	XWF_AddExtractedMetadata = (fptr_XWF_AddExtractedMetadata)GetProcAddress(Hdl, "XWF_AddExtractedMetadata");
	XWF_GetHashValue = (fptr_XWF_GetHashValue)GetProcAddress(Hdl, "XWF_GetHashValue");
	XWF_AddEvent = (fptr_XWF_AddEvent)GetProcAddress(Hdl, "XWF_AddEvent");
	XWF_GetReportTableInfo = (fptr_XWF_GetReportTableInfo)GetProcAddress(Hdl, "XWF_GetReportTableInfo");
	XWF_GetEvObjReportTableAssocs = (fptr_XWF_GetEvObjReportTableAssocs)GetProcAddress(Hdl, "XWF_GetEvObjReportTableAssocs");

	return 1;
}

LONG __stdcall XT_Init(CallerInfo info, DWORD nFlags, HANDLE hMainWnd, void* lpReserved) {
	// Make sure we have an adequate version of X-Ways Forensics that supports the required functions.
	if (info.version < 1770) {
		MessageBox(NULL, L"Error: This X-Tension requires X-Ways Forensics v17.7 or higher.", L"Error", MB_OK);
		return -1;
	}

	XT_RetrieveFunctionPointers();

	return 1;
}

LONG __stdcall XT_Done(void* lpReserved) {
	return 0;
}

LONG __stdcall XT_About(HANDLE hParentWnd, void* lpReserved) {
	MessageBox(NULL, L"MetaDefender X-Tension.\nCopyright 2021 Polito, Inc.\nAll Rights Reserved.", L"About", MB_OK);
	return 0;
}

// This is the method that is called by XWays Forensics on each selected item.
LONG __stdcall XT_ProcessItemEx(LONG nItemID, HANDLE hItem, void* lpReserved) {

	// Check to make sure our functions have been resolved
	if (XWF_GetItemSize == NULL) {
		MessageBox(NULL, L"Error: XWF_GetItemSize function pointer is NULL.", L"Error", MB_OK);
		return 0;
	}

	// TODO: Get file size, ensure that it's below sanity threshold, get file bytes, submit file via HTTP POST
	INT64 i64ItemSize = XWF_GetItemSize(nItemID);
	if (i64ItemSize > INT_MAX) {
		XWF_OutputMessage(L"Error: File size is too large.", 0);
		return 0;
	}

	if (i64ItemSize > MD_MAX_SUBMIT_FILE_SIZE) {
		XWF_OutputMessage(L"Error: File is larger than max file size (10MB)", 0);
		return 0;
	}

	if (hItem != NULL) {
		BYTE* ItemContents = (BYTE*)malloc((size_t)i64ItemSize);

		if (ItemContents != NULL) {
			DWORD dwNumBytesRead = XWF_Read(hItem, 0, ItemContents, (DWORD)i64ItemSize);

			// Make sure we have read the entire file contents
			if (dwNumBytesRead != i64ItemSize) {
				XWF_OutputMessage(L"Error reading file contents from evidence object.", 0);
				return 0;
			}

			// Submit the file via HTTP POST
			std::string fileResultsURL;
			if (UploadFile(ItemContents, (DWORD)i64ItemSize, &fileResultsURL)) {
				std::string message = "File successfully uploaded. View analysis results at ";
				message += fileResultsURL;
				XWF_OutputMessage((const wchar_t*)message.c_str(), 4);
			}

			// Free the buffers
			free(ItemContents);
		}
		else {
			XWF_OutputMessage(L"Unable to allocate memory for file contents", 0);
			return 0;
		}
	}

	return 0;
}
