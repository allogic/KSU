#include "undoc.h"
#include "pagewalk.h"
#include "scanner.h"
#include "mem.h"

VOID
NbIteratePage(
  PHYSICAL_ADDRESS Address,
  LPCSTR Bytes,
  UINT32 NumberOfBytes)
{
  PVOID page = MmGetVirtualForPhysical(Address);
  if (page && MmIsAddressValid(page))
  {
    if (NbIsInPhysicalMemoryRange(Address))
    {
      NbArrayOfBytesScan((PCHAR)page, 0X1000, Bytes, NumberOfBytes);
    }
  }
  else
  {
    //LOG("Invalid page\n");
  }
}

VOID
NbIteratePageTable(
  PHYSICAL_ADDRESS Address,
  BOOLEAN LargePage,
  LPCSTR Bytes,
  UINT32 NumberOfBytes)
{
  PPT pageTable = (PPT)MmGetVirtualForPhysical(Address);
  if (pageTable && MmIsAddressValid(pageTable))
  {
    if (LargePage)
    {
      NbArrayOfBytesScan((PCHAR)pageTable, 0X1000000, Bytes, NumberOfBytes);
    }
    else
    {
      for (UINT32 i = 0; i < 512; i++)
      {
        if (pageTable[i].Flags.Present)
        {
          PHYSICAL_ADDRESS address = { .QuadPart = pageTable[i].Flags.PageFrameNumber << 12 };
          NbIteratePage(address, Bytes, NumberOfBytes);
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
NbIteratePageDirectoryTable(
  PHYSICAL_ADDRESS Address,
  BOOLEAN LargePage,
  LPCSTR Bytes,
  UINT32 NumberOfBytes)
{
  PPDT pageDirectoryTable = (PPDT)MmGetVirtualForPhysical(Address);
  if (pageDirectoryTable && MmIsAddressValid(pageDirectoryTable))
  {
    if (LargePage)
    {
      NbArrayOfBytesScan((PCHAR)pageDirectoryTable, 0X40000000, Bytes, NumberOfBytes);
    }
    else
    {
      for (UINT32 i = 0; i < 512; i++)
      {
        if (pageDirectoryTable[i].Flags.Present)
        {
          PHYSICAL_ADDRESS address = { .QuadPart = pageDirectoryTable[i].Flags.PageFrameNumber << 12 };
          NbIteratePageTable(address, (BOOLEAN)pageDirectoryTable[i].Flags.LargePage, Bytes, NumberOfBytes);
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
NbIteratePageDirectoryPointerTable(
  PHYSICAL_ADDRESS Address,
  LPCSTR Bytes,
  UINT32 NumberOfBytes)
{
  PPDPT pageDirectoryPointerTable = (PPDPT)MmGetVirtualForPhysical(Address);
  if (pageDirectoryPointerTable && MmIsAddressValid(pageDirectoryPointerTable))
  {
    for (UINT32 i = 0; i < 512; i++)
    {
      if (pageDirectoryPointerTable[i].Flags.Present)
      {
        PHYSICAL_ADDRESS address = { .QuadPart = pageDirectoryPointerTable[i].Flags.PageFrameNumber << 12 };
        NbIteratePageDirectoryTable(address, (BOOLEAN)pageDirectoryPointerTable[i].Flags.LargePage, Bytes, NumberOfBytes);
      }
    }
  }
  else
  {
    //LOG("Invalid page directory pointer table\n");
  }
}

VOID
NbIteratePageMapLevel4Table(
  PHYSICAL_ADDRESS Address,
  LPCSTR Bytes,
  UINT32 NumberOfBytes)
{
  PPML4T pageMapLevel4Table = (PPML4T)MmGetVirtualForPhysical(Address);
  if (pageMapLevel4Table && MmIsAddressValid(pageMapLevel4Table))
  {
    for (UINT32 i = 0; i < 512; i++)
    {
      if (pageMapLevel4Table[i].Flags.Present)
      {
        PHYSICAL_ADDRESS address = { .QuadPart = pageMapLevel4Table[i].Flags.PageFrameNumber << 12 };
        NbIteratePageDirectoryPointerTable(address, Bytes, NumberOfBytes);
      }
    }
  }
  else
  {
    //LOG("Invalid page map level 4 table\n");
  }
}