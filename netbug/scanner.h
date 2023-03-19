#ifndef SCANNER_H
#define SCANNER_H

#include "core.h"

VOID
NbResetScanList();

VOID
NbArrayOfBytesScan(
  PCHAR Address,
  UINT32 Length,
  LPCSTR Bytes,
  UINT32 NumberOfBytes);

#endif