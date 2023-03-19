#ifndef PAGEWALK_H
#define PAGEWALK_H

#include "core.h"

VOID
NbIteratePage(
  PHYSICAL_ADDRESS Address,
  LPCSTR Bytes,
  UINT32 NumberOfBytes);

VOID
NbIteratePageTable(
  PHYSICAL_ADDRESS Address,
  BOOLEAN LargePage,
  LPCSTR Pattern,
  UINT32 NumberOfBytes);

VOID
NbIteratePageDirectoryTable(
  PHYSICAL_ADDRESS Address,
  BOOLEAN LargePage,
  LPCSTR Pattern,
  UINT32 NumberOfBytes);

VOID
NbIteratePageDirectoryPointerTable(
  PHYSICAL_ADDRESS Address,
  LPCSTR Pattern,
  UINT32 NumberOfBytes);

VOID
NbIteratePageMapLevel4Table(
  PHYSICAL_ADDRESS Address,
  LPCSTR Pattern,
  UINT32 NumberOfBytes);

#endif