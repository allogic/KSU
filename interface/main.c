#include <stdlib.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "ioctl.h"
#include "socket.h"

#define LOG(FMT, ...) printf(FMT, __VA_ARGS__)

static HANDLE sDriverHandle = INVALID_HANDLE_VALUE;

INT32 main(INT32 Argc, PCHAR Argv[])
{
  // Open driver device
  sDriverHandle = CreateFileA("\\\\.\\PageWalk", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

  if (sDriverHandle != INVALID_HANDLE_VALUE)
  {
    if (strcmp("scan", Argv[1]) == 0)
    {
      UINT32 pid = atoi(Argv[2]);
      DeviceIoControl(sDriverHandle, IOCTRL_SCAN_PROCESS_PHYSICAL_PAGES, &pid, sizeof(UINT32), NULL, 0, NULL, NULL);
    }
  }

  // Close driver device
  CloseHandle(sDriverHandle);  
  
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Initialize winsock
  status = WsaInitialize();
  if (NT_SUCCESS(status))
  {
    __try
    {
      CHAR sendBuffer[] = "this is a test";
      CHAR recvBuffer[1024] = { 0 };

      SOCKET connectSocket = INVALID_SOCKET;
      ADDRINFO remoteAddress = { 0 };
      status = WsaCreateSocket(&connectSocket, Argv[1], Argv[2], (ADDRESS_FAMILY)AF_INET, (UINT16)SOCK_STREAM, IPPROTO_TCP, &remoteAddress);
      if (NT_SUCCESS(status))
      {
        status = WsaConnect(&connectSocket, &remoteAddress);
        if (NT_SUCCESS(status))
        {
          UINT32 sendBufferSize = sizeof(sendBuffer);
          status = WsaSend(&connectSocket, sendBuffer, &sendBufferSize, 0);

          UINT32 recvBufferSize = sizeof(recvBuffer) - 1;
          status = WsaRecv(&connectSocket, recvBuffer, &recvBufferSize, 0);

          LOG("Received: %s\n", recvBuffer);
        }

        status = WsaCloseSocket(&connectSocket);
      }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
      LOG("Something went wrong\n");
    }

    // Deinitialize winsock
    WsaDestroy();
  }

  return 0;
}