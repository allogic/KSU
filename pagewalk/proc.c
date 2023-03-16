#include <ntifs.h>

#include "proc.h"

PEPROCESS GetProcessByProcessId(
  HANDLE Handle)
{
  PEPROCESS process = NULL;

  PsLookupProcessByProcessId(Handle, &process);

  return process;
}