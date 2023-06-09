#ifndef EXCEPT_H
#define EXCEPT_H

#include "core.h"

///////////////////////////////////////////////////////////////
// Public Definitions
///////////////////////////////////////////////////////////////

#define DEFAULT_EXCEPTION_HANDLER KmExceptionHandler(GetExceptionCode(), GetExceptionInformation())

///////////////////////////////////////////////////////////////
// Public API
///////////////////////////////////////////////////////////////

INT32
KmExceptionHandler(
  INT32 Code,
  PEXCEPTION_POINTERS Exception);

#endif