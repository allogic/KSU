#ifndef IA32_H
#define IA32_H

#include <windef.h>
#include <ntdef.h>

typedef union _PML4T
{
  struct
  {
    UINT64 Present : 1;
    UINT64 Write : 1;
    UINT64 Supervisor : 1;
    UINT64 PageLevelWriteThrough : 1;
    UINT64 PageLevelCacheDisable : 1;
    UINT64 Accessed : 1;
    UINT64 Reserved1 : 1;
    UINT64 MustBeZero : 1;
    UINT64 Ignored_1 : 4;
    UINT64 PageFrameNumber : 36;
    UINT64 Reserved2 : 4;
    UINT64 Ignored_2 : 11;
    UINT64 ExecuteDisable : 1;
  } Flags;
  UINT64 Value;
} PML4T, * PPML4T;

typedef union _PDPT
{
  struct
  {
    UINT64 Present : 1;
    UINT64 Write : 1;
    UINT64 Supervisor : 1;
    UINT64 PageLevelWriteThrough : 1;
    UINT64 PageLevelCacheDisable : 1;
    UINT64 Accessed : 1;
    UINT64 Reserved1 : 1;
    UINT64 LargePage : 1;
    UINT64 Ignored_1 : 4;
    UINT64 PageFrameNumber : 36;
    UINT64 Reserved2 : 4;
    UINT64 Ignored_2 : 11;
    UINT64 ExecuteDisable : 1;
  } Flags;
  UINT64 Value;
} PDPT, * PPDPT;

typedef union _PDT
{
  struct
  {
    UINT64 Present : 1;
    UINT64 Write : 1;
    UINT64 Supervisor : 1;
    UINT64 PageLevelWriteThrough : 1;
    UINT64 PageLevelCacheDisable : 1;
    UINT64 Accessed : 1;
    UINT64 Reserved1 : 1;
    UINT64 LargePage : 1;
    UINT64 Ignored_1 : 4;
    UINT64 PageFrameNumber : 36;
    UINT64 Reserved2 : 4;
    UINT64 Ignored_2 : 11;
    UINT64 ExecuteDisable : 1;
  } Flags;
  UINT64 Value;
} PDT, * PPDT;

typedef union _PT
{
  struct
  {
    UINT64 Present : 1;
    UINT64 Write : 1;
    UINT64 Supervisor : 1;
    UINT64 PageLevelWriteThrough : 1;
    UINT64 PageLevelCacheDisable : 1;
    UINT64 Accessed : 1;
    UINT64 Dirty : 1;
    UINT64 Pat : 1;
    UINT64 Global : 1;
    UINT64 Ignored_1 : 3;
    UINT64 PageFrameNumber : 36;
    UINT64 Reserved1 : 4;
    UINT64 Ignored_2 : 7;
    UINT64 ProtectionKey : 4;
    UINT64 ExecuteDisable : 1;
  } Flags;
  UINT64 Value;
} PT, * PPT;

#endif