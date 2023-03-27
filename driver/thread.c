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
    sKeSuspendThread = (KESUSPENDTHREADPROC)(((UINT64)gNtosKrnlBase) + 0x20E3CC);
  }

  if (sKeResumeThread == NULL)
  {
    sKeResumeThread = (KERESUMETHREADPROC)(((UINT64)gNtosKrnlBase) + 0x3695F4);
  }
}

NTSTATUS
KmSuspendProcess(
  EPROCESS* Process)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Protect process from being rundown
  if (ExAcquireRundownProtection(&Process->RundownProtect))
  {
    // Iterate process threads
    PLIST_ENTRY threadListEntry = Process->ThreadListHead.Flink;
    while (threadListEntry != &Process->ThreadListHead)
    {
      // Get thread
      //ETHREAD* thread = CONTAINING_RECORD(threadListEntry, ETHREAD, ThreadListEntry);

      //if (thread)
      //{
      //  LOG("%p\n", thread);
      //}

      LOG("%p\n", threadListEntry);

      // Suspend thread
      //status = KmSuspendThread(thread, NULL);

      // Increment to the next record
      threadListEntry = threadListEntry->Flink;
    }

    // Release rundown protection
    ExReleaseRundownProtection(&Process->RundownProtect);

    status = STATUS_SUCCESS;
  }

  return status;
}

NTSTATUS
KmResumeProcess(
  EPROCESS* Process)
{
  UNREFERENCED_PARAMETER(Process);

  NTSTATUS status = STATUS_UNSUCCESSFUL;

  return status;
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
      UINT32 suspendCount = sKeSuspendThread(&Thread->Tcb);

      if (SuspendCount)
      {
        *SuspendCount = suspendCount;
      }

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

    status = STATUS_SUCCESS;
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

  UINT32 suspendCount = sKeResumeThread(&Thread->Tcb, 1);

  if (SuspendCount)
  {
    *SuspendCount = suspendCount;
  }

  status = STATUS_SUCCESS;

  return status;
}