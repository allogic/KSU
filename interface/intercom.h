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

typedef enum _SCAN_TYPE
{
  SCAN_TYPE_RESET,
  SCAN_TYPE_FIRST_ARRAY_OF_BYTES,
  SCAN_TYPE_NEXT_CHANGED,
  SCAN_TYPE_NEXT_UNCHANGED,
  SCAN_TYPE_UNDO,
} SCAN_TYPE, * PSCAN_TYPE;

typedef struct _REQUEST_SCAN
{
  UINT32 Pid;
  UINT32 NumberOfBytes;
} REQUEST_SCAN, * PREQUEST_SCAN;

#endif