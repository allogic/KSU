#include "thread.h"
#include "findaddr.h"

///////////////////////////////////////////////////////////////
// Disclaimer
///////////////////////////////////////////////////////////////

/*
* ULONG KeSuspendThread(IN OUT PKTHREAD Thread);
* KeSuspendThread 7601-22621.819 -> 0x140648a77
* A8 01 0F 85 ?? ?? ?? ?? 48 8B ?? E8 ?? ?? ?? ?? 89 44 24 ??
* A8 01                         test al, 1
* 0F 85 ?? ?? ?? ??             jnz
* 48 8B CE                      mov rcx, rsi
* E8 6B 3E DA FF                call KeSuspendThread -> 0x1402f6010
* 89 44 24 ??                   mov [rsp+??],eax
*/

/*
* ULONG KeResumeThread(IN OUT PKTHREAD Thread, IN ULONG SuspendCount = 1);
* KeResumeThread 17763-22621.819 -> 0x1406ed832
* 48 8B ?? E8 ?? ?? ?? ?? 65 48 8B 14 25 88 01 00 00 8B
* 48 8B ??                      mov ???, rcx
* E8 ?? ?? ?? ??                call KeResumeThread -> 0x1402f6754
* 65 48 8B 14 25 88 01 00 00    mov rdx, gs:188h
* 8B
*/

///////////////////////////////////////////////////////////////
// Private Definitions
///////////////////////////////////////////////////////////////

#define PS_CROSS_THREAD_FLAGS_TERMINATED 0x00000001UL

///////////////////////////////////////////////////////////////
// Private Types
///////////////////////////////////////////////////////////////

typedef UINT32(*KeSuspendThreadProc)(PKTHREAD Thread);
typedef UINT32(*KeResumeThreadProc)(PKTHREAD Thread, UINT32 SuspendCount);

///////////////////////////////////////////////////////////////
// Private Variables
///////////////////////////////////////////////////////////////

static KeSuspendThreadProc sKeSuspendThread = NULL;
static KeResumeThreadProc sKeResumeThread = NULL;

///////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////

VOID
KmInitializeThreading(
  PDRIVER_OBJECT Driver)
{
  UNICODE_STRING imageName = RTL_CONSTANT_STRING(L"ntoskrnl.exe");
  PVOID ntoskrnlBase = KmGetKernelModuleBase(Driver, &imageName);

  if (sKeSuspendThread == NULL)
  {
    sKeSuspendThread = (KeSuspendThreadProc)(((UINT64)ntoskrnlBase) + 0x2F6010);
  }

  if (sKeResumeThread == NULL)
  {
    sKeResumeThread = (KeResumeThreadProc)(((UINT64)ntoskrnlBase) + 0x2F6754);
  }
}

NTSTATUS
KmSuspendThread(
  ETHREAD* Thread,
  PUINT32 SuspendCount)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Protect thread from being rundown
  if (ExAcquireRundownProtection(&Thread->RundownProtect))
  {
    // Don't suspend if being deleted
    if (Thread->CrossThreadFlags & PS_CROSS_THREAD_FLAGS_TERMINATED)
    {
      status = STATUS_THREAD_IS_TERMINATING;
    }
    else
    {
      *SuspendCount = sKeSuspendThread(&Thread->Tcb);
      status = STATUS_SUCCESS;

      // If deletion was started after we suspended then wake up the thread
      //if (Thread->CrossThreadFlags & PS_CROSS_THREAD_FLAGS_TERMINATED)
      //{
      //  KeForceResumeThread(&Thread->Tcb);
      //  *SuspendCount = 0;
      //  status = STATUS_THREAD_IS_TERMINATING;
      //}
    }

    // Release rundown protection
    ExReleaseRundownProtection(&Thread->RundownProtect);
  }
  else
  {
    status = STATUS_THREAD_IS_TERMINATING;
  }

  return status;
}

NTSTATUS
KmResumeThread(
  ETHREAD* Thread,
  PUINT32 SuspendCount)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  *SuspendCount = sKeResumeThread(&Thread->Tcb, 1);
  status = STATUS_SUCCESS;

  return status;
}