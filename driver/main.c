#include "core.h"
#include "undoc.h"
#include "socket.h"
#include "intercom.h"
#include "except.h"
#include "scanner.h"
#include "thread.h"
#include "memory.h"

///////////////////////////////////////////////////////////////
// Private Variables
///////////////////////////////////////////////////////////////

static BOOL sDriverIsShuttingDown = FALSE;
static KEVENT sTcpServerStoppedEvent = { 0 };
static HANDLE sTcpServerThreadHandle = INVALID_HANDLE_VALUE;

///////////////////////////////////////////////////////////////
// Private API
///////////////////////////////////////////////////////////////

NTSTATUS
KmShutdownRequest(
  PKSOCKET Socket);

NTSTATUS
KmScanRequest(
  PKSOCKET Socket);

NTSTATUS
KmBreakRequest(
  PKSOCKET Socket);

NTSTATUS
KmMemoryRequest(
  PKSOCKET Socket);

VOID
KmTcpServerThread(
  PVOID Context);

VOID
DriverUnload(
  PDRIVER_OBJECT Driver);

NTSTATUS
DriverEntry(
  PDRIVER_OBJECT Driver,
  PUNICODE_STRING RegPath);

///////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////

NTSTATUS
KmShutdownRequest(
  PKSOCKET Socket)
{
  UNREFERENCED_PARAMETER(Socket);

  NTSTATUS status = STATUS_UNSUCCESSFUL;

  sDriverIsShuttingDown = TRUE;

  return status;
}

NTSTATUS
KmScanRequest(
  PKSOCKET Socket)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Raise IRQ to dispatch level
  KIRQL prevInterruptRequestLevel = PASSIVE_LEVEL;
  KeRaiseIrql(DISPATCH_LEVEL, &prevInterruptRequestLevel);

  __try
  {
    // Receive scan type
    SCAN_TYPE scanType = SCAN_TYPE_NONE;
    status = KmRecvSafe(Socket, &scanType, sizeof(SCAN_TYPE), 0);
    if (NT_SUCCESS(status))
    {
      switch (scanType)
      {
        case SCAN_TYPE_RESET:
        {
          // Reset the scanner
          KmResetScanner();

          break;
        }
        case SCAN_TYPE_FIRST_ARRAY_OF_BYTES:
        {
          // Receive process id
          UINT32 processId = 0;
          status = KmRecvSafe(Socket, &processId, sizeof(UINT32), 0);
          if (NT_SUCCESS(status))
          {
            // Receive number of bytes
            UINT32 numberOfBytes = 0;
            status = KmRecvSafe(Socket, &numberOfBytes, sizeof(UINT32), 0);
            if (NT_SUCCESS(status))
            {
              // Allocate buffer to hold bytes
              PBYTE bytes = ExAllocatePoolWithTag(NonPagedPool, numberOfBytes, MEMORY_TAG);
              if (bytes)
              {
                // Receive bytes
                status = KmRecvSafe(Socket, bytes, numberOfBytes, 0);
                if (NT_SUCCESS(status))
                {
                  // Open process
                  PEPROCESS process = NULL;
                  status = PsLookupProcessByProcessId((HANDLE)processId, &process);
                  if (NT_SUCCESS(status))
                  {
                    // Scan array of bytes
                    KmFirstScanArrayOfBytes((PVOID)process->DirectoryTableBase, numberOfBytes, bytes);

                    // Print current findings
                    KmPrintScanResults();

                    // Close process
                    ObDereferenceObject(process);
                  }
                }

                // Free bytes
                ExFreePoolWithTag(bytes, MEMORY_TAG);
              }
            }
          }

          break;
        }
        case SCAN_TYPE_NEXT_CHANGED:
        {
          // Scan changed
          KmNextScanChanged();

          // Print current findings
          KmPrintScanResults();

          break;
        }
        case SCAN_TYPE_NEXT_UNCHANGED:
        {
          // Scan unchanged
          KmNextScanUnchanged();

          // Print current findings
          KmPrintScanResults();

          break;
        }
        case SCAN_TYPE_UNDO:
        {
          // Undo previous scan
          KmUndoScanOperation();

          // Print current findings
          KmPrintScanResults();

          break;
        }
      }
    }
  }
  __except (DEFAULT_EXCEPTION_HANDLER)
  {
    status = STATUS_UNHANDLED_EXCEPTION;
  }

  // Lower IRQ to previous level
  KeLowerIrql(prevInterruptRequestLevel);

  return status;
}

NTSTATUS
KmBreakRequest(
  PKSOCKET Socket)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Raise IRQ to dispatch level
  KIRQL prevInterruptRequestLevel = PASSIVE_LEVEL;
  KeRaiseIrql(DISPATCH_LEVEL, &prevInterruptRequestLevel);

  __try
  {
    // Receive break type
    BREAK_TYPE breakType = BREAK_TYPE_NONE;
    status = KmRecvSafe(Socket, &breakType, sizeof(BREAK_TYPE), 0);
    if (NT_SUCCESS(status))
    {
      switch (breakType)
      {
        case BREAK_TYPE_SET:
        {
          break;
        }
        case BREAK_TYPE_CLEAR:
        {
          break;
        }
      }
    }
  }
  __except (DEFAULT_EXCEPTION_HANDLER)
  {
    status = STATUS_UNHANDLED_EXCEPTION;
  }

  // Lower IRQ to previous level
  KeLowerIrql(prevInterruptRequestLevel);

  return status;
}

NTSTATUS
KmMemoryRequest(
  PKSOCKET Socket)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Raise IRQ to dispatch level
  KIRQL prevInterruptRequestLevel = PASSIVE_LEVEL;
  KeRaiseIrql(DISPATCH_LEVEL, &prevInterruptRequestLevel);

  __try
  {
    // Receive memory type
    MEMORY_TYPE memoryType = MEMORY_TYPE_NONE;
    status = KmRecvSafe(Socket, &memoryType, sizeof(MEMORY_TYPE), 0);
    if (NT_SUCCESS(status))
    {
      switch (memoryType)
      {
        case MEMORY_TYPE_KERNEL:
        {
          // Receive memory operation
          MEMORY_OPERATION_TYPE memoryOperationType = MEMORY_OPERATION_TYPE_NONE;
          status = KmRecvSafe(Socket, &memoryOperationType, sizeof(MEMORY_OPERATION_TYPE), 0);
          if (NT_SUCCESS(status))
          {
            switch (memoryOperationType)
            {
              case MEMORY_OPERATION_TYPE_READ:
              {
                // Receive base address
                UINT64 baseAddress = 0;
                status = KmRecvSafe(Socket, &baseAddress, sizeof(UINT64), 0);
                if (NT_SUCCESS(status))
                {
                  // Receive number of bytes
                  UINT32 numberOfBytes = 0;
                  status = KmRecvSafe(Socket, &numberOfBytes, sizeof(UINT32), 0);
                  if (NT_SUCCESS(status))
                  {
                    // Allocate buffer to hold bytes
                    PBYTE bytes = ExAllocatePoolWithTag(NonPagedPool, numberOfBytes, MEMORY_TAG);
                    if (bytes)
                    {
                      // Read process memory
                      status = KmReadKernelMemory(bytes, (PVOID)baseAddress, numberOfBytes);

                      for (UINT32 i = 0; i < numberOfBytes; i++)
                      {
                        LOG("%02X\n", bytes[i]);
                      }

                      // Free bytes
                      ExFreePoolWithTag(bytes, MEMORY_TAG);
                    }
                  }
                }

                break;
              }
              case MEMORY_OPERATION_TYPE_WRITE:
              {
                // Receive base address
                UINT64 baseAddress = 0;
                status = KmRecvSafe(Socket, &baseAddress, sizeof(UINT64), 0);
                if (NT_SUCCESS(status))
                {
                  // Receive number of bytes
                  UINT32 numberOfBytes = 0;
                  status = KmRecvSafe(Socket, &numberOfBytes, sizeof(UINT32), 0);
                  if (NT_SUCCESS(status))
                  {
                    // Allocate buffer to hold bytes
                    PBYTE bytes = ExAllocatePoolWithTag(NonPagedPool, numberOfBytes, MEMORY_TAG);
                    if (bytes)
                    {
                      // Receive bytes
                      status = KmRecvSafe(Socket, bytes, numberOfBytes, 0);
                      if (NT_SUCCESS(status))
                      {
                        // Write process memory
                        status = KmWriteKernelMemory((PVOID)baseAddress, bytes, numberOfBytes);
                      }

                      // Free bytes
                      ExFreePoolWithTag(bytes, MEMORY_TAG);
                    }
                  }
                }

                break;
              }
            }
          }

          break;
        }
        case MEMORY_TYPE_PROCESS:
        {
          // Receive process id
          UINT32 processId = 0;
          status = KmRecvSafe(Socket, &processId, sizeof(UINT32), 0);
          if (NT_SUCCESS(status))
          {
            // Receive memory operation
            MEMORY_OPERATION_TYPE memoryOperationType = MEMORY_OPERATION_TYPE_NONE;
            status = KmRecvSafe(Socket, &memoryOperationType, sizeof(MEMORY_OPERATION_TYPE), 0);
            if (NT_SUCCESS(status))
            {
              switch (memoryOperationType)
              {
                case MEMORY_OPERATION_TYPE_READ:
                {
                  // Receive base address
                  UINT64 baseAddress = 0;
                  status = KmRecvSafe(Socket, &baseAddress, sizeof(UINT64), 0);
                  if (NT_SUCCESS(status))
                  {
                    // Receive number of bytes
                    UINT32 numberOfBytes = 0;
                    status = KmRecvSafe(Socket, &numberOfBytes, sizeof(UINT32), 0);
                    if (NT_SUCCESS(status))
                    {
                      // Allocate buffer to hold bytes
                      PBYTE bytes = ExAllocatePoolWithTag(NonPagedPool, numberOfBytes, MEMORY_TAG);
                      if (bytes)
                      {
                        // Read process memory
                        status = KmReadProcessMemory(processId, bytes, (PVOID)baseAddress, numberOfBytes);

                        for (UINT32 i = 0; i < numberOfBytes; i++)
                        {
                          LOG("%02X\n", bytes[i]);
                        }

                        // Free bytes
                        ExFreePoolWithTag(bytes, MEMORY_TAG);
                      }
                    }
                  }

                  break;
                }
                case MEMORY_OPERATION_TYPE_WRITE:
                {
                  // Receive base address
                  UINT64 baseAddress = 0;
                  status = KmRecvSafe(Socket, &baseAddress, sizeof(UINT64), 0);
                  if (NT_SUCCESS(status))
                  {
                    // Receive number of bytes
                    UINT32 numberOfBytes = 0;
                    status = KmRecvSafe(Socket, &numberOfBytes, sizeof(UINT32), 0);
                    if (NT_SUCCESS(status))
                    {
                      // Allocate buffer to hold bytes
                      PBYTE bytes = ExAllocatePoolWithTag(NonPagedPool, numberOfBytes, MEMORY_TAG);
                      if (bytes)
                      {
                        // Receive bytes
                        status = KmRecvSafe(Socket, bytes, numberOfBytes, 0);
                        if (NT_SUCCESS(status))
                        {
                          // Write process memory
                          status = KmWriteProcessMemory(processId, (PVOID)baseAddress, bytes, numberOfBytes);
                        }

                        // Free bytes
                        ExFreePoolWithTag(bytes, MEMORY_TAG);
                      }
                    }
                  }

                  break;
                }
              }
            }
          }

          break;
        }
      }
    }
  }
  __except (DEFAULT_EXCEPTION_HANDLER)
  {
    status = STATUS_UNHANDLED_EXCEPTION;
  }

  // Lower IRQ to previous level
  KeLowerIrql(prevInterruptRequestLevel);

  return status;
}

VOID
KmTcpServerThread(
  PVOID Context)
{
  UNREFERENCED_PARAMETER(Context);

  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Initialize winsock
  status = KmInitializeWsk();
  if (NT_SUCCESS(status))
  {
    LOG("TCP server started\n");

    __try
    {
      // Create sockets
      PKSOCKET serverSocket = NULL;
      status = KmCreateListenSocket(&serverSocket, (ADDRESS_FAMILY)AF_INET, (UINT16)SOCK_STREAM, IPPROTO_TCP);
      if (NT_SUCCESS(status))
      {
        // Bind local address and port
        SOCKADDR_IN address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = HTONS(9095);
        status = KmBind(serverSocket, (PSOCKADDR)&address);
        if (NT_SUCCESS(status))
        {
          LOG("Listening...\n");

          while (sDriverIsShuttingDown == FALSE)
          {
            // Accept client connection
            PKSOCKET clientSocket = NULL;
            status = KmAccept(serverSocket, &clientSocket, NULL, (PSOCKADDR)&address);
            if (NT_SUCCESS(status))
            {
              // Receive request type
              REQUEST_TYPE requestType = REQUEST_TYPE_NONE;
              status = KmRecvSafe(clientSocket, &requestType, sizeof(REQUEST_TYPE), 0);
              if (NT_SUCCESS(status))
              {
                switch (requestType)
                {
                  case REQUEST_TYPE_SHUTDOWN:
                  {
                    // Shutdown TCP server
                    status = KmShutdownRequest(clientSocket);

                    break;
                  }
                  case REQUEST_TYPE_SCAN:
                  {
                    // Scan process memory
                    status = KmScanRequest(clientSocket);

                    break;
                  }
                  case REQUEST_TYPE_BREAK:
                  {
                    // Manage breakpoints
                    status = KmBreakRequest(clientSocket);

                    break;
                  }
                  case REQUEST_TYPE_MEMORY:
                  {
                    // Read/Write kernel/process memory
                    status = KmMemoryRequest(clientSocket);

                    break;
                  }
                }
              }

              // Close client socket
              KmCloseSocket(clientSocket);
            }
          }
        }

        // Close server socket
        KmCloseSocket(serverSocket);
      }
    }
    __except (DEFAULT_EXCEPTION_HANDLER)
    {
      status = STATUS_UNHANDLED_EXCEPTION;
    }

    // Deinitialize winsock
    KmDeinitializeWsk();

    LOG("TCP server stopped\n");
  }

  KeSetEvent(&sTcpServerStoppedEvent, IO_NO_INCREMENT, FALSE);
}

///////////////////////////////////////////////////////////////
// Driver Entry
///////////////////////////////////////////////////////////////

VOID
DriverUnload(
  PDRIVER_OBJECT Driver)
{
  UNREFERENCED_PARAMETER(Driver);

  // Wait till TCP server has beed stopped
  KeWaitForSingleObject(&sTcpServerStoppedEvent, Executive, KernelMode, FALSE, NULL);

  // Reset scanner which will free all its memory
  KmResetScanner();

  LOG("Driver stopped\n");
}

NTSTATUS
DriverEntry(
  PDRIVER_OBJECT Driver,
  PUNICODE_STRING RegPath)
{
  UNREFERENCED_PARAMETER(RegPath);

  LOG("Driver started\n");

  // Setup driver unload procedure
  Driver->DriverUnload = DriverUnload;

  // Initialize thread API
  KmInitializeThreading(Driver);

  // Initialize events
  KeInitializeEvent(&sTcpServerStoppedEvent, SynchronizationEvent, FALSE);

  // Initialize memory scanner
  KmInitializeScanner();

  // Create TCP listen thread
  return PsCreateSystemThread(&sTcpServerThreadHandle, THREAD_ALL_ACCESS, 0, 0, 0, KmTcpServerThread, 0);
}