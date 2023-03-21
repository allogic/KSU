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
  UINT32 Pid,
  PEPROCESS Process,
  UINT32 NumberOfBytes,
  PCHAR Value);

VOID
KmResetScanner();

VOID
KmNewScan();

VOID
KmPrintScanResults();

#endif