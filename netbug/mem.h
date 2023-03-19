#ifndef MEM_H
#define MEM_H

#include "core.h"

extern PPHYSICAL_MEMORY_RANGE gPhysicalMemoryRange;
extern UINT32 gPhysicalMemoryRangeCount;

BOOLEAN
NbIsInPhysicalMemoryRange(
  PHYSICAL_ADDRESS Address);

UINT32
NbGetPhysicalMemoryRangeCount();

#endif