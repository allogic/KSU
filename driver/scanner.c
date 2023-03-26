#include "scanner.h"
#include "pagetbl.h"

///////////////////////////////////////////////////////////////
// Private Types
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
// Local Variables
///////////////////////////////////////////////////////////////

static UINT32 sOperationCount = 0;
static LIST_ENTRY sOperationList = { 0 };

///////////////////////////////////////////////////////////////
// Private API
///////////////////////////////////////////////////////////////

VOID
KmIterateBytes(
  PVOID Address,
  UINT32 Size);

VOID
KmNewScanOperation();

VOID
KmCopyPrevOperationConfiguration();

POPERATION_ENTRY
KmGetCurrentScanOperation();

POPERATION_ENTRY
KmGetPreviousScanOperation();

VOID
KmNewScanEntry(
  PVOID Address);

///////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////

VOID
KmIterateBytes(
  PVOID Address,
  UINT32 Size)
{
  // Get operation entry
  POPERATION_ENTRY operationEntry = KmGetCurrentScanOperation();

  // Start byte scan
  for (PBYTE ptr = Address; ptr <= ((((PBYTE)Address) + Size) - operationEntry->NumberOfBytes); ptr++)
  {
    BOOL found = TRUE;

    for (UINT32 i = 0; i < operationEntry->NumberOfBytes; i++)
    {
      if (ptr[i] != operationEntry->Bytes[i])
      {
        found = FALSE;
        break;
      }
    }

    if (found)
    {
      KmNewScanEntry(ptr);
    }
  }
}

VOID
KmNewScanOperation(
  PCHAR Name)
{
  // Prepare operation entry
  POPERATION_ENTRY scanOperation = ExAllocatePoolWithTag(NonPagedPool, sizeof(OPERATION_ENTRY), MEMORY_TAG);
  if (scanOperation)
  {
    // Zero operation entry
    RtlZeroMemory(scanOperation, sizeof(OPERATION_ENTRY));

    // Copy name
    RtlCopyMemory(scanOperation->Name, Name, 32);

    // Setup scan list
    InitializeListHead(&scanOperation->ScanList);

    // Insert operation entry
    InsertHeadList(&sOperationList, &scanOperation->List);

    // Increment operation count
    sOperationCount++;
  }
}

VOID
KmCopyPrevOperationConfiguration()
{
  // Get operation entries
  POPERATION_ENTRY currOperationEntry = KmGetCurrentScanOperation();
  POPERATION_ENTRY prevOperationEntry = KmGetPreviousScanOperation();

  currOperationEntry->PageTableBase = prevOperationEntry->PageTableBase;
  currOperationEntry->NumberOfBytes = prevOperationEntry->NumberOfBytes;
  currOperationEntry->Bytes = ExAllocatePoolWithTag(NonPagedPool, prevOperationEntry->NumberOfBytes, MEMORY_TAG);
  if (currOperationEntry->Bytes && prevOperationEntry->Bytes)
  {
    RtlCopyMemory(currOperationEntry->Bytes, prevOperationEntry->Bytes, prevOperationEntry->NumberOfBytes);
  }
}

VOID
KmUndoScanOperation()
{
  // Remove operation entry
  PLIST_ENTRY operationListEntry = RemoveHeadList(&sOperationList);
  POPERATION_ENTRY operationEntry = CONTAINING_RECORD(operationListEntry, OPERATION_ENTRY, List);

  // Free scan value
  if (operationEntry->Bytes)
  {
    ExFreePoolWithTag(operationEntry->Bytes, MEMORY_TAG);
  }

  // Free scan entries
  while (IsListEmpty(&operationEntry->ScanList) == FALSE)
  {
    PLIST_ENTRY scanListEntry = RemoveHeadList(&operationEntry->ScanList);
    PSCAN_ENTRY scanEntry = CONTAINING_RECORD(scanListEntry, SCAN_ENTRY, List);

    ExFreePoolWithTag(scanEntry, MEMORY_TAG);
  }

  // Free operation entry
  ExFreePoolWithTag(operationEntry, MEMORY_TAG);

  // Decrement operation count
  sOperationCount--;
}

POPERATION_ENTRY
KmGetCurrentScanOperation()
{
  return CONTAINING_RECORD(sOperationList.Flink, OPERATION_ENTRY, List);
}

POPERATION_ENTRY
KmGetPreviousScanOperation()
{
  return CONTAINING_RECORD(sOperationList.Flink->Flink, OPERATION_ENTRY, List);
}

VOID
KmNewScanEntry(
  PVOID Address)
{
  // Prepare scan entry
  PSCAN_ENTRY scanEntry = ExAllocatePoolWithTag(NonPagedPool, sizeof(SCAN_ENTRY), MEMORY_TAG);
  if (scanEntry)
  {
    // Zero scan entry
    RtlZeroMemory(scanEntry, sizeof(SCAN_ENTRY));

    // Store address
    scanEntry->Address = Address;

    // Get operation entry
    POPERATION_ENTRY operationEntry = KmGetCurrentScanOperation();

    // Insert scan result
    InsertHeadList(&operationEntry->ScanList, &scanEntry->List);

    // Increment scan count
    operationEntry->ScanCount++;
  }
}

VOID
KmInitializeScanner()
{
  // Setup operation list
  InitializeListHead(&sOperationList);
}

VOID
KmResetScanner()
{
  // Free operation entries
  while (IsListEmpty(&sOperationList) == FALSE)
  {
    PLIST_ENTRY operationListEntry = RemoveHeadList(&sOperationList);
    POPERATION_ENTRY operationEntry = CONTAINING_RECORD(operationListEntry, OPERATION_ENTRY, List);

    // Free scan value
    if (operationEntry->Bytes)
    {
      ExFreePoolWithTag(operationEntry->Bytes, MEMORY_TAG);
    }
      
    // Free scan entries
    while (IsListEmpty(&operationEntry->ScanList) == FALSE)
    {
      PLIST_ENTRY scanListEntry = RemoveHeadList(&operationEntry->ScanList);
      PSCAN_ENTRY scanEntry = CONTAINING_RECORD(scanListEntry, SCAN_ENTRY, List);

      ExFreePoolWithTag(scanEntry, MEMORY_TAG);
    }

    ExFreePoolWithTag(operationEntry, MEMORY_TAG);
  }

  // Setup operation list
  InitializeListHead(&sOperationList);

  // Reset operation count
  sOperationCount = 0;
}

VOID
KmFirstScanArrayOfBytes(
  PVOID PageTableBase,
  UINT32 NumberOfBytes,
  PBYTE Bytes)
{
  // Create new scan operation
  KmNewScanOperation("First Array Of Bytes");

  // Configure current operation
  POPERATION_ENTRY operationEntry = KmGetCurrentScanOperation();
  operationEntry->PageTableBase = PageTableBase;
  operationEntry->NumberOfBytes = NumberOfBytes;
  operationEntry->Bytes = ExAllocatePoolWithTag(NonPagedPool, NumberOfBytes, MEMORY_TAG);
  if (operationEntry->Bytes && Bytes)
  {
    RtlCopyMemory(operationEntry->Bytes, Bytes, NumberOfBytes);
  }

  // Start scanning process pages
  KmScanProcessPages(PageTableBase, KmIterateBytes);
}

VOID
KmNextScanChanged()
{
  // Create new scan operation
  KmNewScanOperation("Next Changed");

  // Copy previous configuration
  KmCopyPrevOperationConfiguration();

  // Get previous operation entry
  POPERATION_ENTRY prevOperationEntry = KmGetPreviousScanOperation();

  // Iterate previous scans
  PLIST_ENTRY prevScanListEntry = prevOperationEntry->ScanList.Flink;
  while (prevScanListEntry != &prevOperationEntry->ScanList)
  {
    // Get previous scan entry
    PSCAN_ENTRY prevScanEntry = CONTAINING_RECORD(prevScanListEntry, SCAN_ENTRY, List);

    // Compare
    BOOL match = TRUE;

    for (UINT32 i = 0; i < prevOperationEntry->NumberOfBytes; i++)
    {
      if (((PBYTE)prevScanEntry->Address)[i] != prevOperationEntry->Bytes[i])
      {
        match = FALSE;
        break;
      }
    }

    if (!match)
    {
      KmNewScanEntry(prevScanEntry->Address);
    }

    // Increment to the next record
    prevScanListEntry = prevScanListEntry->Flink;
  }
}

VOID
KmNextScanUnchanged()
{
  // Create new scan operation
  KmNewScanOperation("Next Unchanged");

  // Copy previous configuration
  KmCopyPrevOperationConfiguration();

  // Get previous operation entry
  POPERATION_ENTRY prevOperationEntry = KmGetPreviousScanOperation();

  // Iterate previous scans
  PLIST_ENTRY prevScanListEntry = prevOperationEntry->ScanList.Flink;
  while (prevScanListEntry != &prevOperationEntry->ScanList)
  {
    // Get previous scan entry
    PSCAN_ENTRY prevScanEntry = CONTAINING_RECORD(prevScanListEntry, SCAN_ENTRY, List);
  
    // Compare
    BOOL match = TRUE;

    for (UINT32 i = 0; i < prevOperationEntry->NumberOfBytes; i++)
    {
      if (((PBYTE)prevScanEntry->Address)[i] != prevOperationEntry->Bytes[i])
      {
        match = FALSE;
        break;
      }
    }

    if (match)
    {
      KmNewScanEntry(prevScanEntry->Address);
    }
  
    // Increment to the next record
    prevScanListEntry = prevScanListEntry->Flink;
  }
}

VOID
KmPrintScanResults()
{
  if (sOperationCount > 0)
  {
    // Iterate operations
    PLIST_ENTRY operationListEntry = sOperationList.Flink;
    while (operationListEntry != &sOperationList)
    {
      // Get operation entry
      POPERATION_ENTRY operationEntry = CONTAINING_RECORD(operationListEntry, OPERATION_ENTRY, List);

      // Print operation
      LOG("Operation:\n", );
      LOG("  Name: %s\n", operationEntry->Name);
      LOG("  PageTableBase: %p\n", operationEntry->PageTableBase);
      LOG("  NumberOfBytes: %u\n", operationEntry->NumberOfBytes);
      LOG("  Bytes: %p\n", operationEntry->Bytes);
      LOG("  ScanCount: %u\n", operationEntry->ScanCount);
      LOG("  ScanList: %p\n", operationEntry->ScanList.Flink);

      // Iterate scans
      UINT32 scanIndex = 0;
      PLIST_ENTRY scanListEntry = operationEntry->ScanList.Flink;
      while (scanListEntry != &operationEntry->ScanList)
      {
        // Get scan entry
        PSCAN_ENTRY scanEntry = CONTAINING_RECORD(scanListEntry, SCAN_ENTRY, List);

        // Print scan
        LOG("    %10u %p %u\n", scanIndex, scanEntry->Address, *(PINT32)scanEntry->Address);

        // Increment operation index
        scanIndex++;

        // Increment to the next record
        scanListEntry = scanListEntry->Flink;
      }

      // Increment to the next record
      operationListEntry = operationListEntry->Flink;
    }
  }
}