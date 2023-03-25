#include "strconv.h"

///////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////

VOID
UmUtf16ToUtf8(
  PWCHAR Utf16,
  PCHAR Utf8)
{
  wcstombs_s(NULL, Utf8, wcslen(Utf16) + 1, Utf16, _TRUNCATE);
}

VOID
UmUtf8ToUtf16(
  PCHAR Utf8,
  PWCHAR Utf16)
{
  mbstowcs_s(NULL, Utf16, strlen(Utf8) + 1, Utf8, _TRUNCATE);
}

VOID
UmHexToBytes(
  PBYTE Bytes,
  PCHAR String)
{
  CHAR byte[2] = { 0 };
  for (UINT32 i = 0, j = 0; i < strlen(String) - 1; i += 2, j++)
  {
    strncpy(byte, String + i, 2);
    Bytes[j] = (BYTE)strtoul(byte, NULL, 16);
  }
}