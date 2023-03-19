#ifndef EXCEPT_H
#define EXCEPT_H

#include "core.h"

INT32
NbExceptionHandler(
  INT32 Code,
  PEXCEPTION_POINTERS Exception);

#define DEFAULT_EXCEPTION_HANDLER NbExceptionHandler(GetExceptionCode(), GetExceptionInformation())

#endif