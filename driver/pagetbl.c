#include "pagetbl.h"
#include "undoc.h"

///////////////////////////////////////////////////////////////
// Private API
///////////////////////////////////////////////////////////////

VOID
KmIteratePage(
  PHYSICAL_ADDRESS Address,
  PAGE_PROC Procedure);

VOID
KmIteratePageTable(
  PHYSICAL_ADDRESS Address,
  PAGE_PROC Procedure,
  BOOLEAN LargePage);

VOID
KmIteratePageDirectoryTable(
  PHYSICAL_ADDRESS Address,
  PAGE_PROC Procedure,
  BOOLEAN LargePage);

VOID
KmIteratePageDirectoryPointerTable(
  PHYSICAL_ADDRESS Address,
  PAGE_PROC Procedure);

VOID
KmIteratePageMapLevel4Table(
  PHYSICAL_ADDRESS Address,
  PAGE_PROC Procedure);

///////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////

VOID
KmIteratePage(
  PHYSICAL_ADDRESS Address,
  PAGE_PROC Procedure)
{
  PVOID page = MmGetVirtualForPhysical(Address);
  if (page && MmIsAddressValid(page))
  {
    Procedure((PBYTE)page, 0X1000);
  }
  else
  {
    //LOG("Invalid page\n");
  }
}

VOID
KmIteratePageTable(
  PHYSICAL_ADDRESS Address,
  PAGE_PROC Procedure,
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
          KmIteratePage(address, Procedure);
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
  PAGE_PROC Procedure,
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
          KmIteratePageTable(address, Procedure, (BOOLEAN)pageDirectoryTable[i].Flags.LargePage);
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
  PHYSICAL_ADDRESS Address,
  PAGE_PROC Procedure)
{
  PPDPT pageDirectoryPointerTable = (PPDPT)MmGetVirtualForPhysical(Address);
  if (pageDirectoryPointerTable && MmIsAddressValid(pageDirectoryPointerTable))
  {
    for (UINT32 i = 0; i < 512; i++)
    {
      if (pageDirectoryPointerTable[i].Flags.Present)
      {
        PHYSICAL_ADDRESS address = { .QuadPart = pageDirectoryPointerTable[i].Flags.PageFrameNumber << 12 };
        KmIteratePageDirectoryTable(address, Procedure, (BOOLEAN)pageDirectoryPointerTable[i].Flags.LargePage);
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
  PHYSICAL_ADDRESS Address,
  PAGE_PROC Procedure)
{
  PPML4T pageMapLevel4Table = (PPML4T)MmGetVirtualForPhysical(Address);
  if (pageMapLevel4Table && MmIsAddressValid(pageMapLevel4Table))
  {
    for (UINT32 i = 0; i < 512; i++)
    {
      if (pageMapLevel4Table[i].Flags.Present)
      {
        PHYSICAL_ADDRESS address = { .QuadPart = pageMapLevel4Table[i].Flags.PageFrameNumber << 12 };
        KmIteratePageDirectoryPointerTable(address, Procedure);
      }
    }
  }
  else
  {
    //LOG("Invalid page map level 4 table\n");
  }
}

VOID
KmScanProcessPages(
  PVOID Address,
  PAGE_PROC Procedure)
{
  // Start iterating physical pages
  PHYSICAL_ADDRESS address = { .QuadPart = (UINT64)Address };
  KmIteratePageMapLevel4Table(address, Procedure);
}