#include "scanner.h"
#include "undoc.h"

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
  PVOID DirectoryTableBase;
  UINT32 NumberOfBytes;
  PBYTE Bytes;
  UINT32 ScanCount;
  LIST_ENTRY ScanList;
} OPERATION_ENTRY, * POPERATION_ENTRY;

///////////////////////////////////////////////////////////////
// Private Variables
///////////////////////////////////////////////////////////////

static PPHYSICAL_MEMORY_RANGE sPhysicalMemoryRange = NULL;
static UINT32 sPhysicalMemoryRangeCount = 0;

static UINT32 sOperationCount = 0;
static LIST_ENTRY sOperationList = { 0 };

///////////////////////////////////////////////////////////////
// Private API
///////////////////////////////////////////////////////////////

BOOLEAN
KmIsInPhysicalMemoryRange(
  PHYSICAL_ADDRESS Address);

UINT32
KmGetPhysicalMemoryRangeCount();

VOID
KmIterateBytes(
  PBYTE Address,
  UINT32 Size);

VOID
KmIteratePage(
  PHYSICAL_ADDRESS Address);

VOID
KmIteratePageTable(
  PHYSICAL_ADDRESS Address,
  BOOLEAN LargePage);

VOID
KmIteratePageDirectoryTable(
  PHYSICAL_ADDRESS Address,
  BOOLEAN LargePage);

VOID
KmIteratePageDirectoryPointerTable(
  PHYSICAL_ADDRESS Address);

VOID
KmIteratePageMapLevel4Table(
  PHYSICAL_ADDRESS Address);

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

BOOLEAN
KmIsInPhysicalMemoryRange(
  PHYSICAL_ADDRESS Address)
{
  for (UINT64 i = 0; i < sPhysicalMemoryRangeCount; i++)
  {
    if ((Address.QuadPart >= sPhysicalMemoryRange[i].BaseAddress.QuadPart) && (Address.QuadPart <= sPhysicalMemoryRange[i].BaseAddress.QuadPart + sPhysicalMemoryRange[i].NumberOfBytes.QuadPart))
    {
      return TRUE;
    }
  }

  return FALSE;
}

UINT32
KmGetPhysicalMemoryRangeCount()
{
  UINT32 count = 0;

  while (sPhysicalMemoryRange[count].BaseAddress.QuadPart || sPhysicalMemoryRange[count].NumberOfBytes.QuadPart) count++;

  return count;
}

VOID
KmIterateBytes(
  PBYTE Address,
  UINT32 Size)
{
  // Get operation entry
  POPERATION_ENTRY operationEntry = KmGetCurrentScanOperation();

  // Start byte scan
  for (PBYTE ptr = Address; ptr <= ((Address + Size) - operationEntry->NumberOfBytes); ptr++)
  {
    BOOL found = TRUE;

    for (UINT32 j = 0; j < operationEntry->NumberOfBytes; j++)
    {
      if (ptr[j] != operationEntry->Bytes[j])
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
KmIteratePage(
  PHYSICAL_ADDRESS Address)
{
  PVOID page = MmGetVirtualForPhysical(Address);
  if (page && MmIsAddressValid(page))
  {
    if (KmIsInPhysicalMemoryRange(Address))
    {
      KmIterateBytes((PBYTE)page, 0X1000);
    }
  }
  else
  {
    //LOG("Invalid page\n");
  }
}

VOID
KmIteratePageTable(
  PHYSICAL_ADDRESS Address,
  BOOLEAN LargePage)
{
  PPT pageTable = (PPT)MmGetVirtualForPhysical(Address);
  if (pageTable && MmIsAddressValid(pageTable))
  {
    if (LargePage)
    {
      //KmIterateBytes((PBYTE)pageTable, 0X1000000);
    }
    else
    {
      for (UINT32 i = 0; i < 512; i++)
      {
        if (pageTable[i].Flags.Present)
        {
          PHYSICAL_ADDRESS address = { .QuadPart = pageTable[i].Flags.PageFrameNumber << 12 };
          KmIteratePage(address);
        }
      }
    }
  }
  else
  {
    //LOG("Invalid page table\n");
  }
}

VOID
KmIteratePageDirectoryTable(
  PHYSICAL_ADDRESS Address,
  BOOLEAN LargePage)
{
  PPDT pageDirectoryTable = (PPDT)MmGetVirtualForPhysical(Address);
  if (pageDirectoryTable && MmIsAddressValid(pageDirectoryTable))
  {
    if (LargePage)
    {
      //KmIterateBytes((PBYTE)pageDirectoryTable, 0X40000000);
    }
    else
    {
      for (UINT32 i = 0; i < 512; i++)
      {
        if (pageDirectoryTable[i].Flags.Present)
        {
          PHYSICAL_ADDRESS address = { .QuadPart = pageDirectoryTable[i].Flags.PageFrameNumber << 12 };
          KmIteratePageTable(address, (BOOLEAN)pageDirectoryTable[i].Flags.LargePage);
        }
      }
    }
  }
  else
  {
    //LOG("Invalid page directory table\n");
  }
}

VOID
KmIteratePageDirectoryPointerTable(
  PHYSICAL_ADDRESS Address)
{
  PPDPT pageDirectoryPointerTable = (PPDPT)MmGetVirtualForPhysical(Address);
  if (pageDirectoryPointerTable && MmIsAddressValid(pageDirectoryPointerTable))
  {
    for (UINT32 i = 0; i < 512; i++)
    {
      if (pageDirectoryPointerTable[i].Flags.Present)
      {
        PHYSICAL_ADDRESS address = { .QuadPart = pageDirectoryPointerTable[i].Flags.PageFrameNumber << 12 };
        KmIteratePageDirectoryTable(address, (BOOLEAN)pageDirectoryPointerTable[i].Flags.LargePage);
      }
    }
  }
  else
  {
    //LOG("Invalid page directory pointer table\n");
  }
}

VOID
KmIteratePageMapLevel4Table(
  PHYSICAL_ADDRESS Address)
{
  PPML4T pageMapLevel4Table = (PPML4T)MmGetVirtualForPhysical(Address);
  if (pageMapLevel4Table && MmIsAddressValid(pageMapLevel4Table))
  {
    for (UINT32 i = 0; i < 512; i++)
    {
      if (pageMapLevel4Table[i].Flags.Present)
      {
        PHYSICAL_ADDRESS address = { .QuadPart = pageMapLevel4Table[i].Flags.PageFrameNumber << 12 };
        KmIteratePageDirectoryPointerTable(address);
      }
    }
  }
  else
  {
    //LOG("Invalid page map level 4 table\n");
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

  currOperationEntry->DirectoryTableBase = prevOperationEntry->DirectoryTableBase;
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

  // Get physical memory ranges
  sPhysicalMemoryRange = MmGetPhysicalMemoryRanges();
  sPhysicalMemoryRangeCount = KmGetPhysicalMemoryRangeCount();
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
  PVOID DirectoryTableBase,
  UINT32 NumberOfBytes,
  PBYTE Bytes)
{
  // Create new scan operation
  KmNewScanOperation("First Array Of Bytes");

  // Configure current operation
  POPERATION_ENTRY operationEntry = KmGetCurrentScanOperation();
  operationEntry->DirectoryTableBase = DirectoryTableBase;
  operationEntry->NumberOfBytes = NumberOfBytes;
  operationEntry->Bytes = ExAllocatePoolWithTag(NonPagedPool, NumberOfBytes, MEMORY_TAG);
  if (operationEntry->Bytes && Bytes)
  {
    RtlCopyMemory(operationEntry->Bytes, Bytes, NumberOfBytes);
  }

  // Start iterating physical pages
  PHYSICAL_ADDRESS address = { .QuadPart = (UINT64)operationEntry->DirectoryTableBase };
  KmIteratePageMapLevel4Table(address);
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
      LOG("  DirectoryTableBase: %p\n", operationEntry->DirectoryTableBase);
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