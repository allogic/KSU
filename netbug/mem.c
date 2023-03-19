#include "mem.h"

PPHYSICAL_MEMORY_RANGE gPhysicalMemoryRange = NULL;
UINT32 gPhysicalMemoryRangeCount = 0;

BOOLEAN
NbIsInPhysicalMemoryRange(
  PHYSICAL_ADDRESS Address)
{
  for (UINT64 i = 0; i < gPhysicalMemoryRangeCount; i++)
  {
    if ((Address.QuadPart >= gPhysicalMemoryRange[i].BaseAddress.QuadPart) && (Address.QuadPart <= gPhysicalMemoryRange[i].BaseAddress.QuadPart + gPhysicalMemoryRange[i].NumberOfBytes.QuadPart))
    {
      return TRUE;
    }
  }

  return FALSE;
}

UINT32
NbGetPhysicalMemoryRangeCount()
{
  UINT32 count = 0;

  while (gPhysicalMemoryRange[count].BaseAddress.QuadPart || gPhysicalMemoryRange[count].NumberOfBytes.QuadPart) count++;

  return count;
}