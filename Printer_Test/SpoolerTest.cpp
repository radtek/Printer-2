#include "stdafx.h"
#include "CppUnitTest.h"
#include "Spooler.h"
#include "SpoolException.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

/* Stubbed Windows Print API Methods */
static bool open  = false;
static bool doc   = false;
static bool page  = false;
static bool write = false;


BOOL WINAPI OpenPrinter(_In_opt_ LPWSTR n, _Out_ LPHANDLE h, _In_opt_  LPPRINTER_DEFAULTSW d) {
	return open;
}

DWORD WINAPI StartDocPrinterW(
_In_             HANDLE  h,
_In_range_(1, 3) DWORD    l,
_When_(Level == 1, _In_reads_bytes_(sizeof(DOC_INFO_1)))
_When_(Level == 3, _In_reads_bytes_(sizeof(DOC_INFO_3))) LPBYTE  d
) {
	return doc;
}

BOOL WINAPI StartPagePrinter(_In_ HANDLE  hPrinter) {
	return page;
}

BOOL WINAPI WritePrinter( _In_ HANDLE  hPrinter, _In_reads_bytes_(cbBuf) LPVOID  pBuf, DWORD   cbBuf, _Out_   LPDWORD pcWritten) {
	return write;
}

/* Stubbed Windows Print API Methods */

void setSuccess(bool openIn, bool docIn, bool pageIn, bool writeIn) {
	open = openIn;
	doc = docIn;
	page = pageIn;
	write = writeIn;
}

LPTSTR printer = printer;
LPTSTR valid = L"../Printer_Test/valid.txt";

namespace Printer_Test
{		
	TEST_CLASS(SpoolerTest)
	{
	public:
		TEST_METHOD(TestSuccess)
		{
			setSuccess(true, true, true, true);
			bool status = Spooler::spool(printer, valid);
			Assert::IsTrue(status);
		}

		TEST_METHOD(TestInvalidFileThrowsException)
		{
			setSuccess(true, true, true, true);
			auto func = [] () {return Spooler::spool(printer, L"invalid.doc");};
			Assert::ExpectException<SpoolException>(func);
		}

		TEST_METHOD(TestOpenPrinterFailureThrowsException)
		{
			setSuccess(false, true, true, true);
			auto func = [] () {return Spooler::spool(printer, valid);};
			Assert::ExpectException<SpoolException>(func);
		}

		TEST_METHOD(TestStartDocFailureThrowsException)
		{
			setSuccess(true, false, true, true);
			auto func = [] () {return Spooler::spool(printer, valid);};
			Assert::ExpectException<SpoolException>(func);
		}

		TEST_METHOD(TestStartPageFailureThrowsException)
		{
			setSuccess(true, true, false, true);
			auto func = [] () {return Spooler::spool(printer, valid);};
			Assert::ExpectException<SpoolException>(func);
		}

		TEST_METHOD(TestWritePrinterFailureThrowsException)
		{
			setSuccess(true, true, true, false);
			auto func = [] () {return Spooler::spool(printer, valid);};
			Assert::ExpectException<SpoolException>(func);
		}

	};
}