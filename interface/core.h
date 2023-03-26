#ifndef CORE_H
#define CORE_H

#define WIN32_LEAN_AND_MEAN

// Ucrt
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Um
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

///////////////////////////////////////////////////////////////
// Public Definitions
///////////////////////////////////////////////////////////////

#define LOG(FMT, ...) printf(FMT, __VA_ARGS__)
#define STATUS_UNHANDLED_EXCEPTION 0xC0000144L

#endif