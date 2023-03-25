#ifndef SOCKET_H
#define SOCKET_H

#include "core.h"

///////////////////////////////////////////////////////////////
// Public Definitions
///////////////////////////////////////////////////////////////

#define HTONS(N) ((((UINT16)(N) & 0xFF)) << 8) | (((UINT16)(N) & 0xFF00) >> 8)

///////////////////////////////////////////////////////////////
// Public Types
///////////////////////////////////////////////////////////////

typedef struct _KSOCKET KSOCKET, * PKSOCKET;

///////////////////////////////////////////////////////////////
// Public API
///////////////////////////////////////////////////////////////

NTSTATUS
KmInitializeWsk();

VOID
KmDeinitializeWsk();

NTSTATUS
KmGetAddrInfo(
  PUNICODE_STRING NodeName,
  PUNICODE_STRING ServiceName,
  PADDRINFOEXW Hints,
  PADDRINFOEXW* Result);

VOID
KmFreeAddrInfo(
  PADDRINFOEXW AddressInfo);

NTSTATUS
KmCreateSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol,
  UINT32 Flags);

NTSTATUS
KmCreateConnectionSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol);

NTSTATUS
KmCreateListenSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol);

NTSTATUS
KmCreateDatagramSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol);

NTSTATUS
KmCloseSocket(
  PKSOCKET Socket);

NTSTATUS
KmBind(
  PKSOCKET Socket,
  PSOCKADDR LocalAddress);

NTSTATUS
KmAccept(
  PKSOCKET Socket,
  PKSOCKET* NewSocket,
  PSOCKADDR LocalAddress,
  PSOCKADDR RemoteAddress);

NTSTATUS
KmConnect(
  PKSOCKET Socket,
  PSOCKADDR RemoteAddress);

NTSTATUS
KmSendRecv(
  PKSOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags,
  BOOLEAN Send);

NTSTATUS
KmSend(
  PKSOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags);

NTSTATUS
KmRecv(
  PKSOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags);

NTSTATUS
KmSendSafe(
  PKSOCKET Socket,
  PVOID Buffer,
  UINT32 Length,
  UINT32 Flags);

NTSTATUS
KmRecvSafe(
  PKSOCKET Socket,
  PVOID Buffer,
  UINT32 Length,
  UINT32 Flags);

#endif