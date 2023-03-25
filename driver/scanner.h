#ifndef SCANNER_H
#define SCANNER_H

#include "core.h"

///////////////////////////////////////////////////////////////
// Public API
///////////////////////////////////////////////////////////////

VOID
KmInitializeScanner();

VOID
KmResetScanner();

VOID
KmFirstScanArrayOfBytes(
  PVOID PageTableBase,
  UINT32 NumberOfBytes,
  PBYTE Bytes);

VOID
KmNextScanChanged();

VOID
KmNextScanUnchanged();

VOID
KmUndoScanOperation();

VOID
KmPrintScanResults();

#endif