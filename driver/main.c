#include "core.h"
#include "undoc.h"
#include "socket.h"
#include "intercom.h"
#include "except.h"
#include "scanner.h"
#include "thread.h"
#include "baseaddr.h"
#include "memory.h"
#include "config.h"
#include "version.h"

///////////////////////////////////////////////////////////////
// Local Variables
///////////////////////////////////////////////////////////////

static BOOL sDriverIsShuttingDown = FALSE;
static HANDLE sTcpServerThreadHandle = INVALID_HANDLE_VALUE;

#ifdef USE_TCP_SERVER

static KEVENT sTcpServerStoppedEvent = { 0 };

#endif

///////////////////////////////////////////////////////////////
// Private API
///////////////////////////////////////////////////////////////

#ifdef USE_TCP_SERVER

NTSTATUS
KmShutdownRequest(
  PKSOCKET Socket);

NTSTATUS
KmInfoRequest(
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

#endif

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

#ifdef USE_TCP_SERVER

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
KmInfoRequest(
  PKSOCKET Socket)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Raise IRQ to dispatch level
  KIRQL prevInterruptRequestLevel = PASSIVE_LEVEL;
  KeRaiseIrql(DISPATCH_LEVEL, &prevInterruptRequestLevel);

  __try
  {
    // Receive info type
    INFO_TYPE infoType = INFO_TYPE_NONE;
    status = KmRecvSafe(Socket, &infoType, sizeof(INFO_TYPE), 0);
    if (NT_SUCCESS(status))
    {
      switch (infoType)
      {
        case INFO_TYPE_PROCESS:
        {
          // Receive process id
          UINT32 processId = 0;
          status = KmRecvSafe(Socket, &processId, sizeof(UINT32), 0);
          if (NT_SUCCESS(status))
          {
            // Open process
            PEPROCESS process = NULL;
            status = PsLookupProcessByProcessId((HANDLE)processId, &process);
            if (NT_SUCCESS(status))
            {
              // Get base address
              UINT64 base = (UINT64)PsGetProcessSectionBaseAddress(process);

              // Send base address
              status = KmSendSafe(Socket, &base, sizeof(UINT64), 0);

              // Close process
              ObDereferenceObject(process);
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

                    // Get current operation entry
                    POPERATION_ENTRY operationEntry = KmGetCurrentScanOperation();

                    // Send scan count
                    status = KmSendSafe(Socket, &operationEntry->ScanCount, sizeof(UINT32), 0);
                    if (NT_SUCCESS(status))
                    {
                      // Iterate scan list
                      PLIST_ENTRY scanListEntry = operationEntry->ScanList.Flink;
                      while (scanListEntry != &operationEntry->ScanList)
                      {
                        // Get scan entry
                        PSCAN_ENTRY scanEntry = CONTAINING_RECORD(scanListEntry, SCAN_ENTRY, List);

                        // Send address
                        status = KmSendSafe(Socket, &scanEntry->Address, sizeof(UINT64), 0);

                        // Increment to the next record
                        scanListEntry = scanListEntry->Flink;
                      }
                    }

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

          // Get current operation entry
          POPERATION_ENTRY operationEntry = KmGetCurrentScanOperation();

          // Send scan count
          status = KmSendSafe(Socket, &operationEntry->ScanCount, sizeof(UINT32), 0);
          if (NT_SUCCESS(status))
          {
            // Iterate scan list
            PLIST_ENTRY scanListEntry = operationEntry->ScanList.Flink;
            while (scanListEntry != &operationEntry->ScanList)
            {
              // Get scan entry
              PSCAN_ENTRY scanEntry = CONTAINING_RECORD(scanListEntry, SCAN_ENTRY, List);

              // Send address
              status = KmSendSafe(Socket, &scanEntry->Address, sizeof(UINT64), 0);

              // Increment to the next record
              scanListEntry = scanListEntry->Flink;
            }
          }

          break;
        }
        case SCAN_TYPE_NEXT_UNCHANGED:
        {
          // Scan unchanged
          KmNextScanUnchanged();

          // Get current operation entry
          POPERATION_ENTRY operationEntry = KmGetCurrentScanOperation();

          // Send scan count
          status = KmSendSafe(Socket, &operationEntry->ScanCount, sizeof(UINT32), 0);
          if (NT_SUCCESS(status))
          {
            // Iterate scan list
            PLIST_ENTRY scanListEntry = operationEntry->ScanList.Flink;
            while (scanListEntry != &operationEntry->ScanList)
            {
              // Get scan entry
              PSCAN_ENTRY scanEntry = CONTAINING_RECORD(scanListEntry, SCAN_ENTRY, List);

              // Send address
              status = KmSendSafe(Socket, &scanEntry->Address, sizeof(UINT64), 0);

              // Increment to the next record
              scanListEntry = scanListEntry->Flink;
            }
          }

          break;
        }
        case SCAN_TYPE_UNDO:
        {
          // Undo previous scan
          KmUndoScanOperation();

          // Get current operation entry
          POPERATION_ENTRY operationEntry = KmGetCurrentScanOperation();

          // Send scan count
          status = KmSendSafe(Socket, &operationEntry->ScanCount, sizeof(UINT32), 0);
          if (NT_SUCCESS(status))
          {
            // Iterate scan list
            PLIST_ENTRY scanListEntry = operationEntry->ScanList.Flink;
            while (scanListEntry != &operationEntry->ScanList)
            {
              // Get scan entry
              PSCAN_ENTRY scanEntry = CONTAINING_RECORD(scanListEntry, SCAN_ENTRY, List);

              // Send address
              status = KmSendSafe(Socket, &scanEntry->Address, sizeof(UINT64), 0);

              // Increment to the next record
              scanListEntry = scanListEntry->Flink;
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
                      // Read kernel memory
                      status = KmReadKernelMemory(bytes, (PVOID)baseAddress, numberOfBytes);
                      if (NT_SUCCESS(status))
                      {
                        // Send bytes
                        status = KmSendSafe(Socket, bytes, numberOfBytes, 0);
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
                        // Write kernel memory
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
                        // Open process
                        PEPROCESS process = NULL;
                        status = PsLookupProcessByProcessId((HANDLE)processId, &process);
                        if (NT_SUCCESS(status))
                        {
                          // Read process memory
                          status = KmReadProcessMemory(process, bytes, (PVOID)baseAddress, numberOfBytes);
                          if (NT_SUCCESS(status))
                          {
                            // Send bytes
                            status = KmSendSafe(Socket, bytes, numberOfBytes, 0);
                          }

                          // Close process
                          ObDereferenceObject(process);
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
                          // Open process
                          PEPROCESS process;
                          status = PsLookupProcessByProcessId((HANDLE)processId, &process);
                          if (NT_SUCCESS(status))
                          {
                            // Write process memory
                            status = KmWriteProcessMemory(process, (PVOID)baseAddress, bytes, numberOfBytes);

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
                  case REQUEST_TYPE_SHUTDOWN: status = KmShutdownRequest(clientSocket); break;
                  case REQUEST_TYPE_INFO:     status = KmInfoRequest(clientSocket); break;
                  case REQUEST_TYPE_SCAN:     status = KmScanRequest(clientSocket); break;
                  case REQUEST_TYPE_BREAK:    status = KmBreakRequest(clientSocket); break;
                  case REQUEST_TYPE_MEMORY:   status = KmMemoryRequest(clientSocket); break;
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

#endif

///////////////////////////////////////////////////////////////
// Driver Entry
///////////////////////////////////////////////////////////////

VOID
DriverUnload(
  PDRIVER_OBJECT Driver)
{
  UNREFERENCED_PARAMETER(Driver);

#ifdef USE_TCP_SERVER

  // Wait till TCP server has beed stopped
  KeWaitForSingleObject(&sTcpServerStoppedEvent, Executive, KernelMode, FALSE, NULL);

#endif

  // Reset scanner which will free all memory
  KmResetScanner();

  LOG("Driver stopped\n");
}

NTSTATUS
DriverEntry(
  PDRIVER_OBJECT Driver,
  PUNICODE_STRING RegPath)
{
  UNREFERENCED_PARAMETER(RegPath);

  NTSTATUS status = STATUS_SUCCESS;

  // Setup driver unload procedure
  Driver->DriverUnload = DriverUnload;

  // Dump system version
  KmDumpSystemVersion();

  // Raise IRQ to dispatch level
  KIRQL prevInterruptRequestLevel = PASSIVE_LEVEL;
  KeRaiseIrql(DISPATCH_LEVEL, &prevInterruptRequestLevel);

  __try
  {
    // Initialize base addresses
    KmInitializeBaseAddresses(Driver);

    // Initialize threading
    KmInitializeThreading();

    // Initialize memory scanner
    KmInitializeScanner();

#ifdef USE_TCP_SERVER

    // Initialize events
    KeInitializeEvent(&sTcpServerStoppedEvent, SynchronizationEvent, FALSE);

#endif
  }
  __except (DEFAULT_EXCEPTION_HANDLER)
  {
    status = STATUS_UNHANDLED_EXCEPTION;
  }

  // Lower IRQ to previous level
  KeLowerIrql(prevInterruptRequestLevel);

  if (NT_SUCCESS(status))
  {
#ifdef USE_TCP_SERVER

    // Create TCP listen thread
    status = PsCreateSystemThread(&sTcpServerThreadHandle, THREAD_ALL_ACCESS, 0, 0, 0, KmTcpServerThread, 0);

#endif

    if (NT_SUCCESS(status))
    {
      LOG("Driver started\n");
    }
  }

  return status;
}