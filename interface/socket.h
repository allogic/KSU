#ifndef SOCKET_H
#define SOCKET_H

#include <winsock2.h>
#include <ws2tcpip.h>

typedef LONG NTSTATUS;

#define STATUS_SUCCESS      ((NTSTATUS)0x00000000L)
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

NTSTATUS
WsaInitialize();

VOID
WsaDestroy();

NTSTATUS
WsaCreateSocket(
  SOCKET* Socket,
  PCSTR Address,
  PCSTR Port,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol,
  ADDRINFO* RemoteAddress);

NTSTATUS
WsaConnect(
  SOCKET* Socket,
  ADDRINFO* RemoteAddress);

NTSTATUS
WsaCloseSocket(
  SOCKET* Socket);

NTSTATUS
WsaSend(
  SOCKET* Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags);

NTSTATUS
WsaRecv(
  SOCKET* Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags);

#endif