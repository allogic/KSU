#ifndef SOCKET_H
#define SOCKET_H

#include "core.h"

///////////////////////////////////////////////////////////////
// Public Types
///////////////////////////////////////////////////////////////

typedef struct _ASOCKET ASOCKET, * PASOCKET;

///////////////////////////////////////////////////////////////
// Public API
///////////////////////////////////////////////////////////////

INT32
UmInitializeWsa();

VOID
UmDeinitializeWsa();

INT32
UmGetAddrInfo(
  PCSTR Address,
  PCSTR Port,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol,
  PADDRINFOA* AddressInfo);

VOID
UmFreeAddrInfo(
  PADDRINFOA AddressInfo);

INT32
UmCreateSocket(
  PASOCKET* Socket,
  PADDRINFOA Address);

INT32
UmConnect(
  PASOCKET Socket);

INT32
UmShutdownSocket(
  PASOCKET Socket);

INT32
UmCloseSocket(
  PASOCKET Socket);

INT32
UmSend(
  PASOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags);

INT32
UmRecv(
  PASOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags);

INT32
UmSendSafe(
  PASOCKET Socket,
  PVOID Buffer,
  UINT32 Length,
  UINT32 Flags);

INT32
UmRecvSafe(
  PASOCKET Socket,
  PVOID Buffer,
  UINT32 Length,
  UINT32 Flags);

#endif