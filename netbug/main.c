#include "core.h"
#include "mem.h"
#include "undoc.h"
#include "socket.h"
#include "intercom.h"
#include "pagewalk.h"
#include "except.h"

static BOOL sDriverIsShuttingDown = FALSE;
static KEVENT sTcpServerStoppedEvent = { 0 };
static HANDLE sTcpServerThreadHandle = INVALID_HANDLE_VALUE;

NTSTATUS
NbScanRequest(
  PKSOCKET Socket,
  PREQUEST_SCAN Request)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  __try
  {
    // Open process
    PEPROCESS process = NULL;
    status = PsLookupProcessByProcessId((HANDLE)Request->Pid, &process);
    if (NT_SUCCESS(status))
    {
      LOG("Found process %u\n", Request->Pid);

      // Receive bytes to scan for
      PVOID requestValue = ExAllocatePoolWithTag(NonPagedPool, Request->NumberOfBytes, MEMORY_TAG);
      if (requestValue)
      {
        UINT32 requestValueSize = Request->NumberOfBytes;
        status = NbRecv(Socket, &requestValue, &requestValueSize, 0);
        if (NT_SUCCESS(status))
        {
          LOG("Received %u bytes to scan for\n", Request->NumberOfBytes);
          LOG("Scanning...\n");

          // Start scanning pages
          PHYSICAL_ADDRESS address = { .QuadPart = process->DirectoryTableBase };
          NbIteratePageMapLevel4Table(address, requestValue, Request->NumberOfBytes);

          LOG("Finished scan\n");
        }

        ExFreePoolWithTag(requestValue, MEMORY_TAG);
      }

      // Close process
      ObDereferenceObject(process);

      // Operation completed successfully
      status = STATUS_SUCCESS;
    }
    else
    {
      LOG("Invalid process\n");
    }
  }
  __except (DEFAULT_EXCEPTION_HANDLER)
  {
    status = STATUS_UNHANDLED_EXCEPTION;
  }

  return status;
}

VOID
NbTcpServerThread(
  PVOID Context)
{
  UNREFERENCED_PARAMETER(Context);

  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Initialize winsock
  status = NbSocketInitialize();
  if (NT_SUCCESS(status))
  {
    LOG("TCP server started\n");

    __try
    {
      // Create sockets
      PKSOCKET serverSocket = NULL;
      status = NbCreateListenSocket(&serverSocket, (ADDRESS_FAMILY)AF_INET, (UINT16)SOCK_STREAM, IPPROTO_TCP);
      if (NT_SUCCESS(status))
      {
        // Bind local address and port
        SOCKADDR_IN address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = HTONS(9095);
        status = NbBind(serverSocket, (PSOCKADDR)&address);
        if (NT_SUCCESS(status))
        {
          LOG("Listening...\n");

          while (sDriverIsShuttingDown == FALSE)
          {
            // Accept client connection
            PKSOCKET clientSocket = NULL;
            status = NbAccept(serverSocket, &clientSocket, NULL, (PSOCKADDR)&address);
            if (NT_SUCCESS(status))
            {
              LOG("Connected to client\n");

              // Receive request type
              REQUEST_TYPE requestType = { 0 };
              UINT32 requestTypeSize = sizeof(requestType);
              status = NbRecv(clientSocket, &requestType, &requestTypeSize, 0);

              switch (requestType)
              {
                case REQUEST_TYPE_SCAN:
                {
                  REQUEST_SCAN requestScan = { 0 };
                  UINT32 requestScanSize = sizeof(requestScan);
                  status = NbRecv(clientSocket, &requestScan, &requestScanSize, 0);
                  status = NbScanRequest(clientSocket, &requestScan);
                  break;
                }
              }

              //UINT32 sendBufferSize = sizeof(sendBuffer);
              //status = NbSend(clientSocket, sendBuffer, &sendBufferSize, 0);

              // Close client socket
              NbCloseSocket(clientSocket);

              LOG("Disconnected from client\n");
            }
          }
        }

        // Close server socket
        NbCloseSocket(serverSocket);
      }
    }
    __except (DEFAULT_EXCEPTION_HANDLER)
    {
      status = STATUS_UNHANDLED_EXCEPTION;
    }

    // Deinitialize winsock
    NbSocketDeinitialize();

    LOG("TCP server stopped\n");
  }

  KeSetEvent(&sTcpServerStoppedEvent, IO_NO_INCREMENT, FALSE);
}

VOID
DriverUnload(
  PDRIVER_OBJECT Driver)
{
  UNREFERENCED_PARAMETER(Driver);
  
  // Stop listen for incoming connections
  sDriverIsShuttingDown = TRUE;

  // Wait till TCP server has beed stopped
  KeWaitForSingleObject(&sTcpServerStoppedEvent, Executive, KernelMode, FALSE, NULL);

  LOG("Driver stopped\n");
}

NTSTATUS
DriverEntry(
  PDRIVER_OBJECT Driver,
  PUNICODE_STRING RegPath)
{
  UNREFERENCED_PARAMETER(RegPath);

  LOG("Driver starting\n");

  // Setup driver unload procedure
  Driver->DriverUnload = DriverUnload;

  // Initialize events
  KeInitializeEvent(&sTcpServerStoppedEvent, SynchronizationEvent, FALSE);

  // Get physical memory ranges
  gPhysicalMemoryRange = MmGetPhysicalMemoryRanges();
  gPhysicalMemoryRangeCount = NbGetPhysicalMemoryRangeCount();

  // Create TCP listen thread
  return PsCreateSystemThread(&sTcpServerThreadHandle, THREAD_ALL_ACCESS, 0, 0, 0, NbTcpServerThread, 0);
}