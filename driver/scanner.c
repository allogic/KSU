#include "scanner.h"
#include "undoc.h"

///////////////////////////////////////////////////////////////
// Private Types
///////////////////////////////////////////////////////////////

typedef struct _SCAN_ENTRY
{
  LIST_ENTRY List;
  PCHAR Address;
} SCAN_ENTRY, * PSCAN_ENTRY;

typedef struct _OPERATION_ENTRY
{
  LIST_ENTRY List;
  CHAR Name[32];
  LIST_ENTRY ScanList;
  UINT32 ScanCount;
} OPERATION_ENTRY, * POPERATION_ENTRY;

///////////////////////////////////////////////////////////////
// Private Variables
///////////////////////////////////////////////////////////////

static PPHYSICAL_MEMORY_RANGE sPhysicalMemoryRange = NULL;
static UINT32 sPhysicalMemoryRangeCount = 0;

static LIST_ENTRY sOperationList = { 0 };
static UINT32 sOperationCount = 0;

static UINT32 sPid = 0;
static PEPROCESS sProcess = NULL;
static UINT32 sNumberOfBytes = 0;
static PCHAR sValue = NULL;

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
  PCHAR Address,
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
KmScanOperationNew();

VOID
KmScanOperationUndo();

VOID
KmScanEntryNew(
  PCHAR Address);

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
  PCHAR Address,
  UINT32 Size)
{
  // Start byte scan
  for (PCHAR ptr = Address; ptr <= ((Address + Size) - sNumberOfBytes); ptr++)
  {
    BOOL found = TRUE;

    for (UINT32 j = 0; j < sNumberOfBytes; j++)
    {
      if (ptr[j] != sValue[j])
      {
        found = FALSE;
        break;
      }
    }

    if (found)
    {
      KmScanEntryNew(ptr);
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
      KmIterateBytes((PCHAR)page, 0X1000);
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
      //KmIterateBytes((PCHAR)pageTable, 0X1000000);
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
      //KmIterateBytes((PCHAR)pageDirectoryTable, 0X40000000);
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
KmScanOperationNew(
  PCHAR Name)
{
  // Prepare operation entry
  POPERATION_ENTRY scanOperation = ExAllocatePoolWithTag(NonPagedPool, sizeof(OPERATION_ENTRY), MEMORY_TAG);
  if (scanOperation)
  {
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
KmScanOperationUndo()
{

}

VOID
KmScanEntryNew(
  PCHAR Address)
{
  // Prepare scan entry
  PSCAN_ENTRY scanEntry = ExAllocatePoolWithTag(NonPagedPool, sizeof(SCAN_ENTRY), MEMORY_TAG);
  if (scanEntry)
  {
    // Store address
    scanEntry->Address = Address;

    // Get operation entry
    POPERATION_ENTRY operationEntry = CONTAINING_RECORD(sOperationList.Flink, OPERATION_ENTRY, List);

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
KmConfigureScanner(
  UINT32 Pid,
  PEPROCESS Process,
  UINT32 NumberOfBytes,
  PCHAR Value)
{
  sPid = Pid;
  sProcess = Process;
  sNumberOfBytes = NumberOfBytes;
  sValue = Value;
}

VOID
KmResetScanner()
{
  // Reset configuration
  sPid = 0;
  sProcess = NULL;
  sNumberOfBytes = 0;
  sValue = NULL;

  // Free operation entries
  while (IsListEmpty(&sOperationList) == FALSE)
  {
    PLIST_ENTRY operationListEntry = RemoveHeadList(&sOperationList);
    POPERATION_ENTRY operationEntry = CONTAINING_RECORD(operationListEntry, OPERATION_ENTRY, List);

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
KmFirstScanArrayOfBytes()
{
  // Create new scan operation
  KmScanOperationNew("First Array Of Bytes");

  // Start iterating physical pages
  PHYSICAL_ADDRESS address = { .QuadPart = sProcess->DirectoryTableBase };
  KmIteratePageMapLevel4Table(address);
}

VOID
KmNextChangedScan()
{
  // Create new scan operation
  KmScanOperationNew("Next Changed");

  // Get previous operation entry
  POPERATION_ENTRY prevOperationEntry = CONTAINING_RECORD(sOperationList.Blink, OPERATION_ENTRY, List);

  // Iterate previous scans
  PLIST_ENTRY prevScanListEntry = prevOperationEntry->ScanList.Flink;
  while (prevScanListEntry != &prevOperationEntry->ScanList)
  {
    // Get previous scan entry
    PSCAN_ENTRY prevScanEntry = CONTAINING_RECORD(prevScanListEntry, SCAN_ENTRY, List);

    // Compare
    BOOL match = TRUE;

    for (UINT32 i = 0; i < sNumberOfBytes; i++)
    {
      if (prevScanEntry->Address[i] != sValue[i])
      {
        match = FALSE;
        break;
      }
    }

    if (!match)
    {
      KmScanEntryNew(prevScanEntry->Address);
    }

    // Increment to the next record
    prevScanListEntry = prevScanListEntry->Flink;
  }
}

VOID
KmNextUnchangedScan()
{
  // Create new scan operation
  KmScanOperationNew("Next Unchanged");

  // Get previous operation entry
  POPERATION_ENTRY prevOperationEntry = CONTAINING_RECORD(sOperationList.Blink, OPERATION_ENTRY, List);

  // Iterate previous scans
  PLIST_ENTRY prevScanListEntry = prevOperationEntry->ScanList.Flink;
  while (prevScanListEntry != &prevOperationEntry->ScanList)
  {
    // Get previous scan entry
    PSCAN_ENTRY prevScanEntry = CONTAINING_RECORD(prevScanListEntry, SCAN_ENTRY, List);
  
    // Compare
    BOOL match = TRUE;

    for (UINT32 i = 0; i < sNumberOfBytes; i++)
    {  
      if (prevScanEntry->Address[i] != sValue[i])
      {
        match = FALSE;
        break;
      }
    }

    if (match)
    {
      KmScanEntryNew(prevScanEntry->Address);
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
    UINT32 operationIndex = 0;
    PLIST_ENTRY operationListEntry = sOperationList.Flink;
    while (operationListEntry != &sOperationList)
    {
      // Get operation entry
      POPERATION_ENTRY operationEntry = CONTAINING_RECORD(operationListEntry, OPERATION_ENTRY, List);

      // Print operation
      LOG("%u Operation: %s\n", operationIndex, operationEntry->Name);

      // Iterate scans
      UINT32 scanIndex = 0;
      PLIST_ENTRY scanListEntry = operationEntry->ScanList.Flink;
      while (scanListEntry != &operationEntry->ScanList)
      {
        // Get scan entry
        PSCAN_ENTRY scanEntry = CONTAINING_RECORD(scanListEntry, SCAN_ENTRY, List);

        // Print scan
        LOG("  %10u %p %10d\n", scanIndex, scanEntry->Address, *(PINT32)scanEntry->Address);

        // Increment operation index
        scanIndex++;

        // Increment to the next record
        scanListEntry = scanListEntry->Flink;
      }

      // Increment operation index
      operationIndex++;

      // Increment to the next record
      operationListEntry = operationListEntry->Flink;
    }
  }
}