#include "socket.h"

///////////////////////////////////////////////////////////////
// Private Types
///////////////////////////////////////////////////////////////

typedef struct _ASOCKET
{
  SOCKET Socket;
  ADDRINFO Address;
} ASOCKET, * PASOCKET;

///////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////

INT32
UmInitializeWsa()
{
  INT32 status = 0;

  // Setup winsock
  WSADATA wsaData;
  status = WSAStartup(MAKEWORD(2, 2), &wsaData);

  return status;
}

VOID
UmDeinitializeWsa()
{
  WSACleanup();
}

INT32
UmGetAddrInfo(
  PCSTR Address,
  PCSTR Port,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol,
  PADDRINFOA* AddressInfo)
{
  INT32 status = 0;

  ADDRINFO hints = { 0 };
  hints.ai_family = AddressFamily;
  hints.ai_socktype = SocketType;
  hints.ai_protocol = Protocol;

  // Resolve the address and port
  status = getaddrinfo(Address, Port, &hints, AddressInfo);

  return status;
}

VOID
UmFreeAddrInfo(
  PADDRINFOA AddressInfo)
{
  freeaddrinfo(AddressInfo);
}

INT32
UmCreateSocket(
  PASOCKET* Socket,
  PADDRINFOA Address)
{
  INT32 status = 1;

  // Allocate memory for the socket structure
  PASOCKET newSocket = malloc(sizeof(ASOCKET));
  if (newSocket)
  {    
    // Attempt to connect to an address until one succeeds
    for (ADDRINFO* address = Address; address != NULL; address = address->ai_next)
    {
      // Create a socket
      newSocket->Socket = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
      newSocket->Address = *address;

      if (newSocket->Socket != INVALID_SOCKET)
      {
        *Socket = newSocket;
        status = 0;

        break;
      }
    }
  }

  return status;
}

INT32
UmConnect(
  PASOCKET Socket)
{
  INT32 status = 0;

  // Connect to server
  status = connect(Socket->Socket, Socket->Address.ai_addr, (INT32)Socket->Address.ai_addrlen);

  return status;
}

INT32
UmShutdownSocket(
  PASOCKET Socket)
{
  INT32 status = 0;

  // Shutdown the socket
  status = shutdown(Socket->Socket, SD_BOTH);

  return status;
}

INT32
UmCloseSocket(
  PASOCKET Socket)
{
  INT32 status = 0;

  // Close the socket
  status= closesocket(Socket->Socket);
  if (status == 0)
  {
    free(Socket);
  }

  return status;
}

INT32
UmSend(
  PASOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags)
{
  INT32 status = 0;

  // Send buffer
  status = send(Socket->Socket, Buffer, *Length, Flags);
  if (status != SOCKET_ERROR)
  {
    *Length = status;
  }

  return status;
}

INT32
UmRecv(
  PASOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags)
{
  INT32 status = 0;

  // Receive buffer
  status = recv(Socket->Socket, Buffer, *Length, Flags);
  if (status != SOCKET_ERROR)
  {
    *Length = status;
  }

  return status;
}