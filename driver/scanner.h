#ifndef SCANNER_H
#define SCANNER_H

#include "core.h"

///////////////////////////////////////////////////////////////
// Public API
///////////////////////////////////////////////////////////////

VOID
KmInitializeScanner();

VOID
KmConfigureScanner(
  PEPROCESS Process,
  UINT32 NumberOfBytes,
  PCHAR Value);

VOID
KmResetScanner();

VOID
KmNewScan(
  PVOID Address);

VOID
KmPrintScanResults();

#endif