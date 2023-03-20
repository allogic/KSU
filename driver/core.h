#ifndef CORE_H
#define CORE_H

// Km
#include <ntifs.h>
#include <ntddk.h>
#include <wsk.h>

// Shared
#include <windef.h>
#include <ntdef.h>

// Um
#include <handleapi.h>

///////////////////////////////////////////////////////////////
// Public Definitions
///////////////////////////////////////////////////////////////

#define LOG(FMT, ...) DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, FMT, __VA_ARGS__)
#define MEMORY_TAG '  bN'

#endif