#include "core.h"
#include "socket.h"
#include "intercom.h"
#include "except.h"
#include "strconv.h"

///////////////////////////////////////////////////////////////
// Private API
///////////////////////////////////////////////////////////////

REQUEST_TYPE
UmRequestNameToType(
  PCHAR Name);

SCAN_TYPE
UmScanNameToType(
  PCHAR Name);

BREAK_TYPE
UmBreakNameToType(
  PCHAR Name);

MEMORY_TYPE
UmMemoryNameToType(
  PCHAR Name);

MEMORY_OPERATION_TYPE
UmMemoryOperationNameToType(
  PCHAR Name);

INT32
UmShutdownRequest(
  PASOCKET Socket,
  PCHAR Argv[]);

INT32
UmScanRequest(
  PASOCKET Socket,
  PCHAR Argv[]);

INT32
UmBreakRequest(
  PASOCKET Socket,
  PCHAR Argv[]);

INT32
UmMemoryRequest(
  PASOCKET Socket,
  PCHAR Argv[]);

INT32
main(
  INT32 Argc,
  PCHAR Argv[]);

///////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////

REQUEST_TYPE
UmRequestNameToType(
  PCHAR Name)
{
  if (strcmp("shutdown", Name) == 0) return REQUEST_TYPE_SHUTDOWN;
  if (strcmp("scan", Name) == 0) return REQUEST_TYPE_SCAN;
  if (strcmp("break", Name) == 0) return REQUEST_TYPE_BREAK;
  if (strcmp("memory", Name) == 0) return REQUEST_TYPE_MEMORY;

  return REQUEST_TYPE_NONE;
}

SCAN_TYPE
UmScanNameToType(
  PCHAR Name)
{
  if (strcmp("reset", Name) == 0) return SCAN_TYPE_RESET;
  if (strcmp("aob", Name) == 0) return SCAN_TYPE_FIRST_ARRAY_OF_BYTES;
  if (strcmp("changed", Name) == 0) return SCAN_TYPE_NEXT_CHANGED;
  if (strcmp("unchanged", Name) == 0) return SCAN_TYPE_NEXT_UNCHANGED;
  if (strcmp("undo", Name) == 0) return SCAN_TYPE_UNDO;

  return SCAN_TYPE_NONE;
}

BREAK_TYPE
UmBreakNameToType(
  PCHAR Name)
{
  if (strcmp("set", Name) == 0) return BREAK_TYPE_SET;
  if (strcmp("clear", Name) == 0) return BREAK_TYPE_CLEAR;

  return BREAK_TYPE_NONE;
}

MEMORY_TYPE
UmMemoryNameToType(
  PCHAR Name)
{
  if (strcmp("kernel", Name) == 0) return MEMORY_TYPE_KERNEL;
  if (strcmp("process", Name) == 0) return MEMORY_TYPE_PROCESS;

  return MEMORY_TYPE_NONE;
}

MEMORY_OPERATION_TYPE
UmMemoryOperationNameToType(
  PCHAR Name)
{
  if (strcmp("read", Name) == 0) return MEMORY_OPERATION_TYPE_READ;
  if (strcmp("write", Name) == 0) return MEMORY_OPERATION_TYPE_WRITE;

  return MEMORY_OPERATION_TYPE_NONE;
}

INT32
UmShutdownRequest(
  PASOCKET Socket,
  PCHAR Argv[])
{
  INT32 status = 0;

  return status;
}

INT32
UmScanRequest(
  PASOCKET Socket,
  PCHAR Argv[])
{
  INT32 status = 0;

  // Send scan type
  SCAN_TYPE scanType = UmScanNameToType(Argv[4]);
  status = UmSendSafe(Socket, &scanType, sizeof(SCAN_TYPE), 0);
  if (status == 0)
  {
    switch (scanType)
    {
      case SCAN_TYPE_RESET:
      {
        break;
      }
      case SCAN_TYPE_FIRST_ARRAY_OF_BYTES:
      {
        // Send process id
        UINT32 processId = strtoul(Argv[5], NULL, 10);
        status = UmSendSafe(Socket, &processId, sizeof(UINT32), 0);
        if (status == 0)
        {
          // Send number of bytes
          UINT32 numberOfBytes = (UINT32)strlen(Argv[6]) / 2;
          status = UmSendSafe(Socket, &numberOfBytes, sizeof(UINT32), 0);
          if (status == 0)
          {
            // Allocate buffer to hold bytes
            PBYTE bytes = calloc(numberOfBytes, sizeof(BYTE));
            if (bytes)
            {
              // Copy bytes into buffer
              UmHexToBytes(Argv[6], bytes);

              // Send buffer
              status = UmSendSafe(Socket, bytes, numberOfBytes, 0);

              // Free bytes
              free(bytes);
            }
          }
        }

        break;
      }
      case SCAN_TYPE_NEXT_CHANGED:
      {
        break;
      }
      case SCAN_TYPE_NEXT_UNCHANGED:
      {
        break;
      }
      case SCAN_TYPE_UNDO:
      {
        break;
      }
    }
  }

  return status;
}

INT32
UmBreakRequest(
  PASOCKET Socket,
  PCHAR Argv[])
{
  INT32 status = 0;

  // Send break type
  BREAK_TYPE breakType = UmBreakNameToType(Argv[4]);
  status = UmSendSafe(Socket, &breakType, sizeof(BREAK_TYPE), 0);
  if (status == 0)
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

  return status;
}

INT32
UmMemoryRequest(
  PASOCKET Socket,
  PCHAR Argv[])
{
  INT32 status = 0;

  // Send memory type
  MEMORY_TYPE memoryType = UmMemoryNameToType(Argv[4]);
  status = UmSendSafe(Socket, &memoryType, sizeof(MEMORY_TYPE), 0);
  if (status == 0)
  {
    switch (memoryType)
    {
      case MEMORY_TYPE_KERNEL:
      {
        // Send memory operation
        MEMORY_OPERATION_TYPE memoryOperationType = UmMemoryOperationNameToType(Argv[5]);
        status = UmSendSafe(Socket, &memoryOperationType, sizeof(MEMORY_OPERATION_TYPE), 0);
        if (status == 0)
        {
          switch (memoryOperationType)
          {
            case MEMORY_OPERATION_TYPE_READ:
            {
              // Send base address
              UINT64 baseAddress = strtoull(Argv[6], NULL, 16);
              status = UmSendSafe(Socket, &baseAddress, sizeof(UINT64), 0);
              if (status == 0)
              {
                // Send number of bytes
                UINT32 numberOfBytes = strtoul(Argv[7], NULL, 10);
                status = UmSendSafe(Socket, &numberOfBytes, sizeof(UINT32), 0);
              }

              break;
            }
            case MEMORY_OPERATION_TYPE_WRITE:
            {
              // Send base address
              UINT64 baseAddress = strtoull(Argv[6], NULL, 16);
              status = UmSendSafe(Socket, &baseAddress, sizeof(UINT64), 0);
              if (status == 0)
              {
                // Send number of bytes
                UINT32 numberOfBytes = (UINT32)strlen(Argv[7]) / 2;
                status = UmSendSafe(Socket, &numberOfBytes, sizeof(UINT32), 0);
                if (status == 0)
                {
                  // Allocate buffer to hold bytes
                  PBYTE bytes = calloc(numberOfBytes, sizeof(BYTE));
                  if (bytes)
                  {
                    // Copy bytes into buffer
                    UmHexToBytes(Argv[7], bytes);

                    // Send buffer
                    status = UmSendSafe(Socket, bytes, numberOfBytes, 0);

                    // Free bytes
                    free(bytes);
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
        // Send process id
        UINT32 processId = strtoul(Argv[5], NULL, 10);
        status = UmSendSafe(Socket, &processId, sizeof(UINT32), 0);
        if (status == 0)
        {
          // Send memory operation
          MEMORY_OPERATION_TYPE memoryOperationType = UmMemoryOperationNameToType(Argv[6]);
          status = UmSendSafe(Socket, &memoryOperationType, sizeof(MEMORY_OPERATION_TYPE), 0);
          if (status == 0)
          {
            switch (memoryOperationType)
            {
              case MEMORY_OPERATION_TYPE_READ:
              {
                // Send base address
                UINT64 baseAddress = strtoull(Argv[7], NULL, 16);
                status = UmSendSafe(Socket, &baseAddress, sizeof(UINT64), 0);
                if (status == 0)
                {
                  // Send number of bytes
                  UINT32 numberOfBytes = strtoul(Argv[8], NULL, 10);
                  status = UmSendSafe(Socket, &numberOfBytes, sizeof(UINT32), 0);
                }

                break;
              }
              case MEMORY_OPERATION_TYPE_WRITE:
              {
                // Send base address
                UINT64 baseAddress = strtoull(Argv[7], NULL, 16);
                status = UmSendSafe(Socket, &baseAddress, sizeof(UINT64), 0);
                if (status == 0)
                {
                  // Send number of bytes
                  UINT32 numberOfBytes = (UINT32)strlen(Argv[8]) / 2;
                  status = UmSendSafe(Socket, &numberOfBytes, sizeof(UINT32), 0);
                  if (status == 0)
                  {
                    // Allocate buffer to hold bytes
                    PBYTE bytes = calloc(numberOfBytes, sizeof(BYTE));
                    if (bytes)
                    {
                      // Copy bytes into buffer
                      UmHexToBytes(Argv[8], bytes);

                      // Send buffer
                      status = UmSendSafe(Socket, bytes, numberOfBytes, 0);

                      // Free bytes
                      free(bytes);
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

  return status;
}

///////////////////////////////////////////////////////////////
// Program Entry
///////////////////////////////////////////////////////////////

INT32
main(
  INT32 Argc,
  PCHAR Argv[])
{
  INT32 status = 0;

  // Initialize winsock
  status = UmInitializeWsa();
  if (status == 0)
  {
    __try
    {
      // Resolve remote address and port
      PASOCKET remoteSocket = NULL;
      PADDRINFOA remoteAddress = NULL;
      status = UmGetAddrInfo(Argv[1], Argv[2], (ADDRESS_FAMILY)AF_INET, (UINT16)SOCK_STREAM, IPPROTO_TCP, &remoteAddress);
      if (status == 0)
      {
        // Create socket
        status = UmCreateSocket(&remoteSocket, remoteAddress);
        if (status == 0)
        {
          // Connect remote socket
          status = UmConnect(remoteSocket);
          if (status == 0)
          {
            REQUEST_TYPE requestType = UmRequestNameToType(Argv[3]);
            status = UmSendSafe(remoteSocket, &requestType, sizeof(REQUEST_TYPE), 0);
            if (status == 0)
            {
              switch (requestType)
              {
                case REQUEST_TYPE_SHUTDOWN:
                {
                  // Shutdown TCP server
                  UmShutdownRequest(remoteSocket, Argv);

                  break;
                }
                case REQUEST_TYPE_SCAN:
                {
                  // Scan process memory
                  UmScanRequest(remoteSocket, Argv);

                  break;
                }
                case REQUEST_TYPE_BREAK:
                {
                  // Manage breakpoints
                  status = UmBreakRequest(remoteSocket, Argv);

                  break;
                }
                case REQUEST_TYPE_MEMORY:
                {
                  // Read/Write kernel/process memory
                  status = UmMemoryRequest(remoteSocket, Argv);

                  break;
                }
              }
            }
      
            // Shutdown remote socket
            UmShutdownSocket(remoteSocket);
          }
      
          // Close remote socket
          UmCloseSocket(remoteSocket);
        }

        // Free remote address
        UmFreeAddrInfo(remoteAddress);
      }
    }
    __except (DEFAULT_EXCEPTION_HANDLER)
    {
      status = STATUS_UNHANDLED_EXCEPTION;
    }

    // Deinitialize winsock
    UmDeinitializeWsa();
  }

  return status;
}