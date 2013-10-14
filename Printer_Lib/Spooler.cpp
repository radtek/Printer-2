#include "stdafx.h"

#include <winspool.h>
#include "Spooler.h"
#include "SpoolException.h"

using namespace std;

/**
 * This method allows you to instruct local print spooler
 * to write to the specified printer. A `SpoolStatus` is 
 * returned which can be used to get success of the spooling 
 * operation or a job identifier.
 *
 * A SpoolException is thrown if there is failure when...
 * 1. obtaining a handle to a printer or printer server
 * 2. notifying the spooler a document will be spooled
 * 3. notifying the spooler a page will be printed
 * 4. reading the file
 * 5. notifying the spooler the data should be written to the printer
 * 6. notifying the spooler at end of page
 * 7. notifying the spooler at end of document
 *
 * @param  <LPTSTR> printer name
 * @param  <LPTSTR> file path
 * @return <SpoolStatus> spool success
 * @throws <SpoolException>
 **/
bool Spooler::spool(LPTSTR printerName, LPTSTR filePath) {

	HANDLE printerHandle = NULL;
	if (!OpenPrinter(printerName, &printerHandle, NULL)) {
		throw SpoolException("Failed to obtain a handle to the printer or printer server");
	}
	
	DWORD sd = 0;
	DOC_INFO_1 docinfo = {0};
	docinfo.pDocName = L"MAR File";
	docinfo.pDatatype = L"raw";

	sd = StartDocPrinter(printerHandle, 1, (PBYTE)&docinfo);
	if (!sd) {
		ClosePrinter(printerHandle);
		throw SpoolException("Failed to notify the spooler a document will be spooled");
	}
	
	if (!StartPagePrinter(printerHandle)) {
		EndDocPrinter(printerHandle);
		ClosePrinter(printerHandle);
		throw SpoolException("Failed to notify the spooler a page will be printed");
	}
	
	HANDLE hFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		EndPagePrinter(printerHandle);
		EndDocPrinter(printerHandle);
		ClosePrinter(printerHandle);

		char buffer[1000];
		sprintf_s(buffer, 1000, "Failed reading the file '%S'", filePath);
		throw SpoolException(buffer);
	}

	char buffer[1024] = {0};

	for (int size = GetFileSize(hFile, NULL); size; )
	{
		DWORD dwRead = 0, dwWritten = 0;
		
		if (!ReadFile(hFile, buffer, min(size, sizeof(buffer)), &dwRead, NULL)) {
			break;
		}
				
		if (!WritePrinter(printerHandle, buffer, (DWORD) sizeof(buffer), &dwWritten)) {
			EndPagePrinter(printerHandle);
			EndDocPrinter(printerHandle);
			ClosePrinter(printerHandle);
			throw SpoolException("Failed to notify the spooler the data should be written to the printer");
		}

		size -= dwRead;
	}

	CloseHandle(hFile);
	EndPagePrinter(printerHandle);
	EndDocPrinter(printerHandle);
	ClosePrinter(printerHandle);

	return true;
}