#ifndef SOCKET_H
#define SOCKET_H

#include "core.h"

typedef struct _KSOCKET KSOCKET, * PKSOCKET;

typedef struct _KSOCKET_ASYNC_CONTEXT
{
  KEVENT CompletionEvent;
  PIRP Irp;
} KSOCKET_ASYNC_CONTEXT, * PKSOCKET_ASYNC_CONTEXT;

NTSTATUS
NbAsyncContextAllocate(
  PKSOCKET_ASYNC_CONTEXT AsyncContext);

VOID
NbAsyncContextFree(
  PKSOCKET_ASYNC_CONTEXT AsyncContext);

VOID
NbAsyncContextReset(
  PKSOCKET_ASYNC_CONTEXT AsyncContext);

NTSTATUS
NbAsyncContextCompletionRoutine(
  PDEVICE_OBJECT	DeviceObject,
  PIRP Irp,
  PKEVENT CompletionEvent);

NTSTATUS
NbAsyncContextWaitForCompletion(
  PKSOCKET_ASYNC_CONTEXT AsyncContext,
  PNTSTATUS Status);

NTSTATUS
NbSocketInitialize();

VOID
NbSocketDeinitialize();

NTSTATUS
NbGetAddrInfo(
  PUNICODE_STRING NodeName,
  PUNICODE_STRING ServiceName,
  PADDRINFOEXW Hints,
  PADDRINFOEXW* Result);

VOID
NbFreeAddrInfo(
  PADDRINFOEXW AddressInfo);

NTSTATUS
NbCreateSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol,
  UINT32 Flags);

NTSTATUS
NbCreateConnectionSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol);

NTSTATUS
NbCreateListenSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol);

NTSTATUS
NbCreateDatagramSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol);

NTSTATUS
NbCloseSocket(
  PKSOCKET Socket);

NTSTATUS
NbBind(
  PKSOCKET Socket,
  PSOCKADDR LocalAddress);

NTSTATUS
NbAccept(
  PKSOCKET Socket,
  PKSOCKET* NewSocket,
  PSOCKADDR LocalAddress,
  PSOCKADDR RemoteAddress);

NTSTATUS
NbConnect(
  PKSOCKET Socket,
  PSOCKADDR RemoteAddress);

NTSTATUS
NbSendRecv(
  PKSOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags,
  BOOLEAN Send);

NTSTATUS
NbSend(
  PKSOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags);

NTSTATUS
NbRecv(
  PKSOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags);

#endif