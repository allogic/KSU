#ifndef SCANNER_H
#define SCANNER_H

#include "core.h"

extern LIST_ENTRY gScanList;
extern UINT64 gScanCount;

VOID
NbInitializeScanner();

VOID
NbResetScanList();

VOID
NbArrayOfBytesScan(
  PCHAR Address,
  UINT32 Length,
  LPCSTR Bytes,
  UINT32 NumberOfBytes);

VOID
NbPrintScanResults();

#endif