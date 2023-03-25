#include "findaddr.h"
#include "undoc.h"

PVOID
KmGetKernelModuleBase(
  PDRIVER_OBJECT DriverObject,
  PUNICODE_STRING ModuleName)
{
  PLDR_DATA_TABLE_ENTRY ldrDataList = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
  PLDR_DATA_TABLE_ENTRY ldrDataEntry = NULL;

  // Iterate LDR
  PLIST_ENTRY ldrListEntry = ldrDataList->InLoadOrderLinks.Flink;
  while (ldrListEntry != &ldrDataList->InLoadOrderLinks)
  {
    // Get LDR entry
    ldrDataEntry = CONTAINING_RECORD(ldrListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

    // Compare image name
    if (!ldrDataEntry->BaseDllName.Buffer)
    {
      if (RtlCompareUnicodeString(&ldrDataEntry->BaseDllName, ModuleName, TRUE) == 0)
      {
        return ldrDataEntry->DllBase;
      }
    }

    // Increment to the next record
    ldrListEntry = ldrListEntry->Flink;
  }

  return NULL;
}