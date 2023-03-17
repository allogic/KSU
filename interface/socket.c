#include "socket.h"

NTSTATUS
WsaInitialize()
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  INT32 result = 0;

  // Setup winsock
  WSADATA wsaData;
  result = WSAStartup(MAKEWORD(2, 2), &wsaData);
  status = (result == 0) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;

  return status;
}

VOID
WsaDestroy()
{
  WSACleanup();
}

NTSTATUS
WsaCreateSocket(
  SOCKET* Socket,
  PCSTR Address,
  PCSTR Port,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol,
  ADDRINFO* RemoteAddress)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  INT32 result = 0;

  ADDRINFO hints = { 0 };
  hints.ai_family = AddressFamily;
  hints.ai_socktype = SocketType;
  hints.ai_protocol = Protocol;

  ADDRINFO* address = NULL;

  // Resolve the server address and port
  result = getaddrinfo(Address, Port, &hints, &address);
  status = (result == 0) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
  if (result == 0)
  {
    // Attempt to connect to an address until one succeeds
    for (ADDRINFO* info = address; info != NULL; info = info->ai_next)
    {
      // Create a socket for connecting to server
      *Socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
      status = (*Socket == INVALID_SOCKET) ? STATUS_UNSUCCESSFUL : STATUS_SUCCESS;
    }

    freeaddrinfo(address);
  }

  return status;
}

NTSTATUS
WsaConnect(
  SOCKET* Socket,
  ADDRINFO* RemoteAddress)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  INT32 result = 0;

  // Connect to server
  result = connect(*Socket, RemoteAddress->ai_addr, (INT32)RemoteAddress->ai_addrlen);
  status = (result == SOCKET_ERROR) ? STATUS_UNSUCCESSFUL : STATUS_SUCCESS;

  return status;
}

NTSTATUS
WsaCloseSocket(
  SOCKET* Socket)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  INT32 result = 0;

  // Close the socket
  result = closesocket(*Socket);
  status = (result == 0) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;

  return status;
}

NTSTATUS
WsaSend(
  SOCKET* Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  INT32 result = 0;

  // Send buffer
  result = send(*Socket, Buffer, *Length, Flags);
  if (result == SOCKET_ERROR)
  {
    status = STATUS_UNSUCCESSFUL;
  }
  else
  {
    *Length = result;
    status = STATUS_SUCCESS;
  }

  return result;
}

NTSTATUS
WsaRecv(
  SOCKET* Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  INT32 result = 0;

  // Receive buffer
  result = recv(*Socket, Buffer, *Length, Flags);
  if (result == SOCKET_ERROR)
  {
    status = STATUS_UNSUCCESSFUL;
  }
  else
  {
    *Length = result;
    status = STATUS_SUCCESS;
  }

  return result;
}