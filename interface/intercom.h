#ifndef INTERCOM_H
#define INTERCOM_H

#include "core.h"

///////////////////////////////////////////////////////////////
// Public Types
///////////////////////////////////////////////////////////////

typedef enum _REQUEST_TYPE
{
  REQUEST_TYPE_SCAN,
} REQUEST_TYPE, * PREQUEST_TYPE;

typedef struct _REQUEST_SCAN
{
  UINT32 Pid;
  UINT32 NumberOfBytes;
} REQUEST_SCAN, * PREQUEST_SCAN;

#endif