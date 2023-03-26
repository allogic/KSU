#include "thread.h"
#include "baseaddr.h"

///////////////////////////////////////////////////////////////
// Private Definitions
///////////////////////////////////////////////////////////////

#define PS_CROSS_THREAD_FLAGS_TERMINATED 0x00000001UL

///////////////////////////////////////////////////////////////
// Private Types
///////////////////////////////////////////////////////////////

typedef UINT32(*KESUSPENDTHREADPROC)(PKTHREAD Thread);
typedef UINT32(*KERESUMETHREADPROC)(PKTHREAD Thread, UINT32 SuspendCount);

///////////////////////////////////////////////////////////////
// Local Variables
///////////////////////////////////////////////////////////////

static KESUSPENDTHREADPROC sKeSuspendThread = NULL;
static KERESUMETHREADPROC sKeResumeThread = NULL;

///////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////

VOID
KmInitializeThreading()
{
  if (sKeSuspendThread == NULL)
  {
    sKeSuspendThread = (KESUSPENDTHREADPROC)(((UINT64)gNtosKrnlBase) + 0x2F6010);
  }

  if (sKeResumeThread == NULL)
  {
    sKeResumeThread = (KERESUMETHREADPROC)(((UINT64)gNtosKrnlBase) + 0x2F6754);
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