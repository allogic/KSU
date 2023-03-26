#ifndef STRCONV_H
#define STRCONV_H

#include "core.h"

///////////////////////////////////////////////////////////////
// Public API
///////////////////////////////////////////////////////////////

VOID
UmUtf16ToUtf8(
  PWCHAR Utf16,
  PCHAR Utf8);

VOID
UmUtf8ToUtf16(
  PCHAR Utf8,
  PWCHAR Utf16);

VOID
UmHexToBytes(
  PBYTE Hex,
  PBYTE Bytes);

#endif