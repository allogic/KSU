#ifndef THREAD_H
#define THREAD_H

#include "core.h"
#include "undoc.h"

///////////////////////////////////////////////////////////////
// Public API
///////////////////////////////////////////////////////////////

VOID
KmInitializeThreading();

NTSTATUS
KmSuspendProcess(
  EPROCESS* Process);

NTSTATUS
KmResumeProcess(
  EPROCESS* Process);

NTSTATUS
KmSuspendThread(
  ETHREAD* Thread,
  PUINT32 SuspendCount);

NTSTATUS
KmResumeThread(
  ETHREAD* Thread,
  PUINT32 SuspendCount);

#endif