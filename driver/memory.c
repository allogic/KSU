#include "memory.h"
#include "undoc.h"

///////////////////////////////////////////////////////////////
// Private API
///////////////////////////////////////////////////////////////

NTSTATUS
KmReadMemory(
  PVOID Destination,
  PVOID Source,
  UINT32 Size);

NTSTATUS
KmWriteMemory(
  PVOID Destination,
  PVOID Source,
  UINT32 Size);

///////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////

NTSTATUS
KmReadMemory(
  PVOID Destination,
  PVOID Source,
  UINT32 Size)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Create MDL for supplied range
  PMDL mdl = IoAllocateMdl(Source, Size, FALSE, FALSE, NULL);
  if (mdl)
  {
    // Try lock pages
    MmProbeAndLockPages(mdl, KernelMode, IoReadAccess);

    // Remap to non-paged memory
    PVOID mapped = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, HighPagePriority);
    if (mapped)
    {
      // Set page protection
      status = MmProtectMdlSystemAddress(mdl, PAGE_READONLY);
      if (NT_SUCCESS(status))
      {
        // Copy memory
        RtlCopyMemory(Destination, mapped, Size);
      }

      // Unmap locked pages
      MmUnmapLockedPages(mapped, mdl);
    }

    // Unlock MDL
    MmUnlockPages(mdl);

    // Free MDL
    IoFreeMdl(mdl);
  }

  return status;
}

NTSTATUS
KmWriteMemory(
  PVOID Destination,
  PVOID Source,
  UINT32 Size)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Create MDL for supplied range
  PMDL mdl = IoAllocateMdl(Destination, Size, FALSE, FALSE, NULL);
  if (mdl)
  {
    // Try lock pages
    MmProbeAndLockPages(mdl, KernelMode, IoReadAccess);

    // Remap to non-paged memory
    PVOID mapped = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, HighPagePriority);
    if (mapped)
    {
      // Set page protection
      status = MmProtectMdlSystemAddress(mdl, PAGE_READWRITE);
      if (NT_SUCCESS(status))
      {
        // Copy memory
        RtlCopyMemory(mapped, Source, Size);
      }

      // Unmap locked pages
      MmUnmapLockedPages(mapped, mdl);
    }

    // Unlock MDL
    MmUnlockPages(mdl);

    // Free MDL
    IoFreeMdl(mdl);
  }

  return status;
}

NTSTATUS
KmReadKernelMemory(
  PVOID Destination,
  PVOID Source,
  UINT32 Size)
{
  return KmReadMemory(Destination, Source, Size);
}

NTSTATUS
KmWriteKernelMemory(
  PVOID Destination,
  PVOID Source,
  UINT32 Size)
{
  return KmWriteMemory(Destination, Source, Size);
}

NTSTATUS
KmReadProcessMemory(
  PEPROCESS Process,
  PVOID Destination,
  PVOID Source,
  UINT32 Size)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Attach to process
  KAPC_STATE apc = { 0 };
  KeStackAttachProcess(Process, &apc);

  PVOID base = PsGetProcessSectionBaseAddress(Process);
  LOG("process base at %p\n", base);

  // Read memory
  status = KmReadMemory(Destination, Source, Size);

  // Detach from process
  KeUnstackDetachProcess(&apc);

  return status;
}

NTSTATUS
KmWriteProcessMemory(
  PEPROCESS Process,
  PVOID Destination,
  PVOID Source,
  UINT32 Size)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Attach to process
  KAPC_STATE apc;
  KeStackAttachProcess(Process, &apc);

  PVOID base = PsGetProcessSectionBaseAddress(Process);
  LOG("process base at %p\n", base);

  // Write memory
  status = KmWriteMemory(Destination, Source, Size);

  // Detach from process
  KeUnstackDetachProcess(&apc);

  return status;
}