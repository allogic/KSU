#ifndef SOCKET_H
#define SOCKET_H

#include <ntddk.h>
#include <wsk.h>

#define HTONS(N) (((((UINT16)(N) & 0xFF)) << 8) | (((UINT16)(N) & 0xFF00) >> 8))

typedef struct _KSOCKET KSOCKET, * PKSOCKET;

NTSTATUS
WskInitialize();

VOID
WskDestroy();

NTSTATUS
WskGetAddrInfo(
  PUNICODE_STRING NodeName,
  PUNICODE_STRING ServiceName,
  PADDRINFOEXW Hints,
  PADDRINFOEXW* Result);

VOID
WskFreeAddrInfo(
  PADDRINFOEXW AddrInfo);

NTSTATUS
WskCreateSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol,
  UINT32 Flags);

NTSTATUS
WskCreateConnectionSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol);

NTSTATUS
WskCreateListenSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol);

NTSTATUS
WskCreateDatagramSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol);

NTSTATUS
WskCloseSocket(
  PKSOCKET Socket);

NTSTATUS
WskBind(
  PKSOCKET Socket,
  PSOCKADDR LocalAddress);

NTSTATUS
WskAccept(
  PKSOCKET Socket,
  PKSOCKET* NewSocket,
  PSOCKADDR LocalAddress,
  PSOCKADDR RemoteAddress);

NTSTATUS
WskConnect(
  PKSOCKET Socket,
  PSOCKADDR RemoteAddress);

NTSTATUS
WskSendRecv(
  PKSOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags,
  BOOLEAN Send);

NTSTATUS
WskSend(
  PKSOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags);

NTSTATUS
WskRecv(
  PKSOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags);

#endif