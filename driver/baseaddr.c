#include "baseaddr.h"
#include "undoc.h"

///////////////////////////////////////////////////////////////
// Disclaimer
///////////////////////////////////////////////////////////////

/*
* KeSuspendThread 7601-22621.819 -> 0x140648a77
*
* A8 01 0F 85 ?? ?? ?? ?? 48 8B ?? E8 ?? ?? ?? ?? 89 44 24 ??
*
* A8 01                         test al, 1
* 0F 85 ?? ?? ?? ??             jnz
* 48 8B CE                      mov rcx, rsi
* E8 6B 3E DA FF                call KeSuspendThread -> 0x1402f6010
* 89 44 24 ??                   mov [rsp+??],eax
*/

/*
* KeResumeThread 17763-22621.819 -> 0x1406ed832
*
* 48 8B ?? E8 ?? ?? ?? ?? 65 48 8B 14 25 88 01 00 00 8B
*
* 48 8B ??                      mov ???, rcx
* E8 ?? ?? ?? ??                call KeResumeThread -> 0x1402f6754
* 65 48 8B 14 25 88 01 00 00    mov rdx, gs:188h
* 8B
*/

///////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////

PVOID gNtosKrnlBase = NULL;
UINT32 gNtosKrnlSize = 0;

///////////////////////////////////////////////////////////////
// Private API
///////////////////////////////////////////////////////////////

PVOID
KmGetKernelModuleBase(
  PDRIVER_OBJECT DriverObject,
  PUNICODE_STRING ModuleName,
  PUINT32 Size);

///////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////

VOID
KmInitializeBaseAddresses(
  PDRIVER_OBJECT Driver)
{
  UNICODE_STRING ntosKrnlImageName = RTL_CONSTANT_STRING(L"ntoskrnl.exe");
  gNtosKrnlBase = KmGetKernelModuleBase(Driver, &ntosKrnlImageName, &gNtosKrnlSize);



  LOG("ntoskrnl at %p\n", gNtosKrnlBase);
}

PVOID
KmGetKernelModuleBase(
  PDRIVER_OBJECT DriverObject,
  PUNICODE_STRING ModuleName,
  PUINT32 Size)
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
    if (ldrDataEntry->BaseDllName.Buffer)
    {
      if (RtlCompareUnicodeString(&ldrDataEntry->BaseDllName, ModuleName, TRUE) == 0)
      {
        if (Size)
        {
          *Size = ldrDataEntry->SizeOfImage;
        }

        return ldrDataEntry->DllBase;
      }
    }

    // Increment to the next record
    ldrListEntry = ldrListEntry->Flink;
  }

  return NULL;
}