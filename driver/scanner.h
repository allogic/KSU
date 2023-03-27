#ifndef SCANNER_H
#define SCANNER_H

#include "core.h"

///////////////////////////////////////////////////////////////
// Public Types
///////////////////////////////////////////////////////////////

typedef struct _SCAN_ENTRY
{
  LIST_ENTRY List;
  PVOID Address;
} SCAN_ENTRY, * PSCAN_ENTRY;

typedef struct _OPERATION_ENTRY
{
  LIST_ENTRY List;
  CHAR Name[32];
  PVOID PageTableBase;
  UINT32 NumberOfBytes;
  PBYTE Bytes;
  UINT32 ScanCount;
  LIST_ENTRY ScanList;
} OPERATION_ENTRY, * POPERATION_ENTRY;

///////////////////////////////////////////////////////////////
// Public API
///////////////////////////////////////////////////////////////

VOID
KmInitializeScanner();

POPERATION_ENTRY
KmGetCurrentScanOperation();

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