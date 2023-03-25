#ifndef MEMORY_H
#define MEMORY_H

#include "core.h"

///////////////////////////////////////////////////////////////
// Public API
///////////////////////////////////////////////////////////////

NTSTATUS
KmReadKernelMemory(
  PVOID Destination,
  PVOID Source,
  UINT32 Size);

NTSTATUS
KmWriteKernelMemory(
  PVOID Destination,
  PVOID Source,
  UINT32 Size);

NTSTATUS
KmReadProcessMemory(
  UINT32 Pid,
  PVOID Destination,
  PVOID Source,
  UINT32 Size);

NTSTATUS
KmWriteProcessMemory(
  UINT32 Pid,
  PVOID Destination,
  PVOID Source,
  UINT32 Size);

#endif