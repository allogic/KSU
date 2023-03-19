#include "scanner.h"

typedef struct _SCAN_ENTRY
{
  LIST_ENTRY List;
  PVOID Address;
} SCAN_ENTRY, * PSCAN_ENTRY;

LIST_ENTRY gScanList = { 0 };
UINT64 gScanCount = 0;

VOID
NbResetScanList()
{
  // Free entries
  while (IsListEmpty(&gScanList) == FALSE)
  {
    PLIST_ENTRY listEntry = RemoveHeadList(&gScanList);
    PSCAN_ENTRY scanEntry = CONTAINING_RECORD(listEntry, SCAN_ENTRY, List);

    ExFreePoolWithTag(scanEntry, MEMORY_TAG);
  }

  // Reset scan list
  InitializeListHead(&gScanList);

  // Reset scan count
  gScanCount = 0;
}

VOID
NbArrayOfBytesScan(
  PVOID Address,
  UINT32 Length,
  LPCSTR Bytes,
  UINT32 NumberOfBytes)
{
  // Create MDL for specified range
  PMDL mdl = IoAllocateMdl(Address, Length, FALSE, FALSE, NULL);
  if (mdl)
  {
    // Lock pages
    MmProbeAndLockPages(mdl, KernelMode, IoReadAccess);

    PCHAR ptr = Address;
    for (UINT32 i = 0; i <= (Length - NumberOfBytes); i++)
    {
      BOOL found = FALSE;
      for (UINT32 j = 0; j < NumberOfBytes; j++)
      {
        if (MmIsAddressValid(ptr + i + j))
        {
          if (ptr[i + j] != Bytes[j])
          {
            found = FALSE;
            break;
          }
        }
        else
        {
          found = FALSE;
          break;
        }
      }

      if (found)
      {
        // Insert scan result
        PSCAN_ENTRY scanEntry = ExAllocatePoolWithTag(NonPagedPool, sizeof(SCAN_ENTRY), MEMORY_TAG);
        if (scanEntry)
        {
          scanEntry->Address = ptr + i;
          InsertTailList(&gScanList, &scanEntry->List);

          // Increment scan count
          gScanCount++;
        }
      }
    }
  
    // Unlock pages
    MmUnlockPages(mdl);
  
    // Free MDL
    IoFreeMdl(mdl);
  }

  //PCHAR ptr = Address;
  //for (UINT32 i = 0; i <= (Length - NumberOfBytes); i++)
  //{
  //  BOOL found = FALSE;
  //  for (UINT32 j = 0; j < NumberOfBytes; j++)
  //  {
  //    if (MmIsAddressValid(ptr + i + j))
  //    {
  //      if (ptr[i + j] != Bytes[j])
  //      {
  //        found = FALSE;
  //        break;
  //      }
  //    }
  //    else
  //    {
  //      found = FALSE;
  //      break;
  //    }
  //  }
  //
  //  if (found)
  //  {
  //    // Insert scan result
  //    PSCAN_ENTRY scanEntry = ExAllocatePoolWithTag(NonPagedPool, sizeof(SCAN_ENTRY), MEMORY_TAG);
  //    if (scanEntry)
  //    {
  //      scanEntry->Address = ptr + i;
  //      InsertTailList(&gScanList, &scanEntry->List);
  //
  //      // Increment scan count
  //      gScanCount++;
  //    }
  //  }
  //}
}