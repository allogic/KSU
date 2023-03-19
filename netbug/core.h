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

#define LOG(FMT, ...) DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, FMT, __VA_ARGS__)
#define HTONS(N) ((((UINT16)(N) & 0xFF)) << 8) | (((UINT16)(N) & 0xFF00) >> 8)
#define MEMORY_TAG '  bN'

#endif