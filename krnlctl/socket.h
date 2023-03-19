#ifndef SOCKET_H
#define SOCKET_H

#include "core.h"

typedef struct _ASOCKET ASOCKET, * PASOCKET;

INT32
KcSocketInitialize();

VOID
KcSocketDeinitialize();

INT32
KcGetAddrInfo(
  PCSTR Address,
  PCSTR Port,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol,
  PADDRINFOA* AddressInfo);

VOID
KcFreeAddrInfo(
  PADDRINFOA AddressInfo);

INT32
KcCreateSocket(
  PASOCKET* Socket,
  PADDRINFOA Address);

INT32
KcConnect(
  PASOCKET Socket);

INT32
KcShutdownSocket(
  PASOCKET Socket);

INT32
KcCloseSocket(
  PASOCKET Socket);

INT32
KcSend(
  PASOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags);

INT32
KcRecv(
  PASOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags);

#endif