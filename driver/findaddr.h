#ifndef FINDADDR_H
#define FINDADDR_H

#include "core.h"

PVOID
KmGetKernelModuleBase(
  PDRIVER_OBJECT DriverObject,
  PUNICODE_STRING ModuleName);

#endif