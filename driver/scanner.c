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

///////////////////////////////////////////////////////////////
// Private Variables
///////////////////////////////////////////////////////////////

static PPHYSICAL_MEMORY_RANGE sPhysicalMemoryRange = NULL;
static UINT32 sPhysicalMemoryRangeCount = 0;

static LIST_ENTRY sScanList = { 0 };
static UINT64 sScanCount = 0;

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
KmScanArrayOfBytes(
  PCHAR Address,
  UINT32 Size);

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
KmIteratePage(
  PHYSICAL_ADDRESS Address)
{
  PVOID page = MmGetVirtualForPhysical(Address);
  if (page && MmIsAddressValid(page))
  {
    if (KmIsInPhysicalMemoryRange(Address))
    {
      KmScanArrayOfBytes((PCHAR)page, 0X1000);
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
      //KmScanArrayOfBytes((PCHAR)pageTable, 0X1000000);
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
      //KmScanArrayOfBytes((PCHAR)pageDirectoryTable, 0X40000000);
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
KmScanArrayOfBytes(
  PCHAR Address,
  UINT32 Size)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  //ProbeForWrite()
  //ZwProtectVirtualMemory()

  // Attach to process
  //KAPC_STATE apc;
  //KeStackAttachProcess(sProcess, &apc);

  // Create MDL for supplied range
  PMDL mdl = IoAllocateMdl(Address, Size, FALSE, FALSE, NULL);
  if (mdl)
  {
    // Try lock pages
    MmProbeAndLockPages(mdl, KernelMode, IoReadAccess);
  
    // Remap to non-paged memory
    PCHAR mapped = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, HighPagePriority);
    if (mapped)
    {
      // Set page protection
      status = MmProtectMdlSystemAddress(mdl, PAGE_READONLY);
      if (NT_SUCCESS(status))
      {
        LOG("Success\n");

        // Query virtual memory informations
        //MEMORY_BASIC_INFORMATION mbi = { 0 };
        //status = ZwQueryVirtualMemory((HANDLE)sPid, mapped, MemoryBasicInformation, &mbi, sizeof(mbi), NULL);
        //LOG("Status:0x%08X\n", status);
        //if (NT_SUCCESS(status))
        //{
        //  LOG("Scanning %p %p %p\n", Address, mapped, mbi.BaseAddress);
        //  // Skip non-committed, no-access and guard pages
        //  if ((mbi.State == MEM_COMMIT) && (mbi.Protect != PAGE_NOACCESS) && ((mbi.Protect & PAGE_GUARD) == FALSE))
        //  {
        //    // Start byte scan
        //    for (PCHAR ptr = mapped; ptr <= ((mapped + Size) - sNumberOfBytes); ptr++)
        //    {
        //      BOOL found = TRUE;
        //
        //      for (UINT32 j = 0; j < sNumberOfBytes; j++)
        //      {
        //        if (ptr[j] != sValue[j])
        //        {
        //          found = FALSE;
        //          break;
        //        }
        //      }
        //
        //      if (found)
        //      {
        //        LOG("Found at %p\n", ptr);
        //      }
        //    }
        //  }
        //}
      }
    
      // Unmap locked pages
      MmUnmapLockedPages(mapped, mdl);
    }
  
    // Unlock MDL
    MmUnlockPages(mdl);
  
    // Free MDL
    IoFreeMdl(mdl);
  }

  // Detach from process
  //KeUnstackDetachProcess(&apc);
}

VOID
KmInitializeScanner()
{
  // Setup scan list
  InitializeListHead(&sScanList);

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

  // Free entries
  while (IsListEmpty(&sScanList) == FALSE)
  {
    PLIST_ENTRY listEntry = RemoveHeadList(&sScanList);
    PSCAN_ENTRY scanEntry = CONTAINING_RECORD(listEntry, SCAN_ENTRY, List);

    ExFreePoolWithTag(scanEntry, MEMORY_TAG);
  }

  // Reset scan list
  InitializeListHead(&sScanList);

  // Reset scan count
  sScanCount = 0;
}

VOID
KmNewScan()
{
  // Start iterating physical pages
  PHYSICAL_ADDRESS address = { .QuadPart = sProcess->DirectoryTableBase };
  KmIteratePageMapLevel4Table(address);
}

VOID
KmPrintScanResults()
{
  LOG("Results:\n");

  // Iterate scans
  PLIST_ENTRY listEntry = sScanList.Flink;
  while (listEntry != &sScanList)
  {
    // Get scan entry
    PSCAN_ENTRY scanEntry = CONTAINING_RECORD(listEntry, SCAN_ENTRY, List);
    
    // Print result
    LOG("  %p\n", scanEntry->Address);

    // Increment to the next record
    listEntry = listEntry->Flink;
  }
}