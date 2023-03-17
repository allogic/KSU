#include "socket.h"

#define MEMORY_TAG '  sK'

typedef struct _KSOCKET_ASYNC_CONTEXT
{
  KEVENT CompletionEvent;
  PIRP Irp;
} KSOCKET_ASYNC_CONTEXT, * PKSOCKET_ASYNC_CONTEXT;

typedef struct _KSOCKET
{
  PWSK_SOCKET	WskSocket;
  union
  {
    PVOID WskDispatch;
    PWSK_PROVIDER_CONNECTION_DISPATCH WskConnectionDispatch;
    PWSK_PROVIDER_LISTEN_DISPATCH WskListenDispatch;
    PWSK_PROVIDER_DATAGRAM_DISPATCH WskDatagramDispatch;
#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)
    PWSK_PROVIDER_STREAM_DISPATCH WskStreamDispatch;
#endif
  };
  KSOCKET_ASYNC_CONTEXT AsyncContext;
} KSOCKET, * PKSOCKET;

static WSK_REGISTRATION sWskRegistration;
static WSK_PROVIDER_NPI sWskProvider;
static WSK_CLIENT_DISPATCH sWskDispatch = { MAKE_WSK_VERSION(1,0), 0, NULL };

NTSTATUS
KspAsyncContextAllocate(
  PKSOCKET_ASYNC_CONTEXT AsyncContext);

VOID
KspAsyncContextFree(
  PKSOCKET_ASYNC_CONTEXT AsyncContext);

VOID
KspAsyncContextReset(
  PKSOCKET_ASYNC_CONTEXT AsyncContext);

NTSTATUS
KspAsyncContextCompletionRoutine(
  PDEVICE_OBJECT	DeviceObject,
  PIRP Irp,
  PKEVENT CompletionEvent);

NTSTATUS
KspAsyncContextWaitForCompletion(
  PKSOCKET_ASYNC_CONTEXT AsyncContext,
  PNTSTATUS Status);


NTSTATUS
KspAsyncContextAllocate(
  PKSOCKET_ASYNC_CONTEXT AsyncContext)
{
  // Initialize the completion event
  KeInitializeEvent(
    &AsyncContext->CompletionEvent,
    SynchronizationEvent,
    FALSE);

  // Initialize the IRP
  AsyncContext->Irp = IoAllocateIrp(1, FALSE);
  if (AsyncContext->Irp == NULL)
  {
    return STATUS_INSUFFICIENT_RESOURCES;
  }
  else
  {
    // KspAsyncContextCompletionRoutine will set the CompletionEvent
    IoSetCompletionRoutine(
      AsyncContext->Irp,
      &KspAsyncContextCompletionRoutine,
      &AsyncContext->CompletionEvent,
      TRUE,
      TRUE,
      TRUE);

    return STATUS_SUCCESS;
  }
}

VOID
KspAsyncContextFree(
  PKSOCKET_ASYNC_CONTEXT AsyncContext)
{
  // Free the IRP
  IoFreeIrp(AsyncContext->Irp);
}

VOID
KspAsyncContextReset(
  PKSOCKET_ASYNC_CONTEXT AsyncContext)
{
  // Reset the completion event
  KeResetEvent(&AsyncContext->CompletionEvent);

  // Reuse the IRP
  IoReuseIrp(AsyncContext->Irp, STATUS_UNSUCCESSFUL);

  IoSetCompletionRoutine(
    AsyncContext->Irp,
    &KspAsyncContextCompletionRoutine,
    &AsyncContext->CompletionEvent,
    TRUE,
    TRUE,
    TRUE);
}

NTSTATUS
KspAsyncContextCompletionRoutine(
  PDEVICE_OBJECT DeviceObject,
  PIRP Irp,
  PKEVENT CompletionEvent)
{
  UNREFERENCED_PARAMETER(DeviceObject);
  UNREFERENCED_PARAMETER(Irp);

  KeSetEvent(CompletionEvent, IO_NO_INCREMENT, FALSE);

  return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS
KspAsyncContextWaitForCompletion(
  PKSOCKET_ASYNC_CONTEXT AsyncContext,
  PNTSTATUS Status)
{
  if (*Status == STATUS_PENDING)
  {
    KeWaitForSingleObject(
      &AsyncContext->CompletionEvent,
      Executive,
      KernelMode,
      FALSE,
      NULL);

    *Status = AsyncContext->Irp->IoStatus.Status;
  }

  return *Status;
}

NTSTATUS
WskInitialize()
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Register as a WSK client
  WSK_CLIENT_NPI wskClient;
  wskClient.ClientContext = NULL;
  wskClient.Dispatch = &sWskDispatch;

  status = WskRegister(&wskClient, &sWskRegistration);

  if (NT_SUCCESS(status))
  {
    // Capture the provider NPI
    return WskCaptureProviderNPI(
      &sWskRegistration,
      WSK_INFINITE_WAIT,
      &sWskProvider);
  }

  return status;
}

VOID
WskDestroy()
{
  // Release the provider NPI instance
  WskReleaseProviderNPI(&sWskRegistration);

  // Deregister as a WSK client
  WskDeregister(&sWskRegistration);
}

NTSTATUS
WskGetAddrInfo(
  PUNICODE_STRING NodeName,
  PUNICODE_STRING ServiceName,
  PADDRINFOEXW Hints,
  PADDRINFOEXW* Result)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Allocate async context
  KSOCKET_ASYNC_CONTEXT asyncContext;
  status = KspAsyncContextAllocate(&asyncContext);

  if (NT_SUCCESS(status))
  {
    // Call the WSK API
    status = sWskProvider.Dispatch->WskGetAddressInfo(
      sWskProvider.Client,
      NodeName,
      ServiceName,
      0,
      NULL,
      Hints,
      Result,
      NULL,
      NULL,
      asyncContext.Irp);

    KspAsyncContextWaitForCompletion(&asyncContext, &status);

    // Free the async context
    KspAsyncContextFree(&asyncContext);
  }

  return status;
}

VOID
WskFreeAddrInfo(
  PADDRINFOEXW AddrInfo)
{
  sWskProvider.Dispatch->WskFreeAddressInfo(
    sWskProvider.Client,
    AddrInfo);
}

NTSTATUS
WskCreateSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol,
  UINT32 Flags)
{
  NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;

  // Allocate memory for the socket structure
  PKSOCKET newSocket = ExAllocatePoolWithTag(PagedPool, sizeof(KSOCKET), MEMORY_TAG);
  if (newSocket)
  {
    // Allocate async context for the socket
    status = KspAsyncContextAllocate(&newSocket->AsyncContext);
    if (NT_SUCCESS(status))
    {
      // Create the WSK socket
      status = sWskProvider.Dispatch->WskSocket(
        sWskProvider.Client,
        AddressFamily,
        SocketType,
        Protocol,
        Flags,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        newSocket->AsyncContext.Irp);

      KspAsyncContextWaitForCompletion(&newSocket->AsyncContext, &status);

      // Save the socket instance and the socket dispatch table
      if (NT_SUCCESS(status))
      {
        newSocket->WskSocket = (PWSK_SOCKET)newSocket->AsyncContext.Irp->IoStatus.Information;
        newSocket->WskDispatch = (PVOID)newSocket->WskSocket->Dispatch;

        *Socket = newSocket;
      }
    }
  }

  return status;
}

NTSTATUS
WskCreateConnectionSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol)
{
  return WskCreateSocket(Socket, AddressFamily, SocketType, Protocol, WSK_FLAG_CONNECTION_SOCKET);
}

NTSTATUS
WskCreateListenSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol)
{
  return WskCreateSocket(Socket, AddressFamily, SocketType, Protocol, WSK_FLAG_LISTEN_SOCKET);
}

NTSTATUS
WskCreateDatagramSocket(
  PKSOCKET* Socket,
  ADDRESS_FAMILY AddressFamily,
  UINT16 SocketType,
  UINT32 Protocol)
{
  return WskCreateSocket(Socket, AddressFamily, SocketType, Protocol, WSK_FLAG_DATAGRAM_SOCKET);
}

NTSTATUS
WskCloseSocket(
  PKSOCKET Socket)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Reset the async context
  KspAsyncContextReset(&Socket->AsyncContext);

  // Close the WSK socket
  status = Socket->WskConnectionDispatch->WskCloseSocket(
    Socket->WskSocket,
    Socket->AsyncContext.Irp);

  KspAsyncContextWaitForCompletion(&Socket->AsyncContext, &status);

  // Free the async context
  KspAsyncContextFree(&Socket->AsyncContext);

  // Free memory for the socket structure
  ExFreePoolWithTag(Socket, MEMORY_TAG);

  return status;
}

NTSTATUS
WskBind(
  PKSOCKET Socket,
  PSOCKADDR LocalAddress)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Reset the async context
  KspAsyncContextReset(&Socket->AsyncContext);

  // Bind the socket
  status = Socket->WskListenDispatch->WskBind(
    Socket->WskSocket,
    LocalAddress,
    0,
    Socket->AsyncContext.Irp);

  KspAsyncContextWaitForCompletion(&Socket->AsyncContext, &status);

  return status;
}

NTSTATUS
WskAccept(
  PKSOCKET Socket,
  PKSOCKET* NewSocket,
  PSOCKADDR LocalAddress,
  PSOCKADDR RemoteAddress)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Reset the async context
  KspAsyncContextReset(&Socket->AsyncContext);

  // Accept the connection
  status = Socket->WskListenDispatch->WskAccept(
    Socket->WskSocket,
    0,
    NULL,
    NULL,
    LocalAddress,
    RemoteAddress,
    Socket->AsyncContext.Irp);

  KspAsyncContextWaitForCompletion(&Socket->AsyncContext, &status);

  // Save the socket instance and the socket dispatch table
  if (NT_SUCCESS(status))
  {
    PKSOCKET newSocket = ExAllocatePoolWithTag(PagedPool, sizeof(KSOCKET), MEMORY_TAG);
    if (newSocket)
    {
      newSocket->WskSocket = (PWSK_SOCKET)Socket->AsyncContext.Irp->IoStatus.Information;
      newSocket->WskDispatch = (PVOID)newSocket->WskSocket->Dispatch;

      KspAsyncContextAllocate(&newSocket->AsyncContext);

      *NewSocket = newSocket;
    }
    else
    {
      return STATUS_INSUFFICIENT_RESOURCES;
    }
  }

  return status;
}

NTSTATUS
WskConnect(
  PKSOCKET Socket,
  PSOCKADDR RemoteAddress)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Reset the async context
  KspAsyncContextReset(&Socket->AsyncContext);

  // Bind the socket to the local address
  SOCKADDR_IN localAddress;
  localAddress.sin_family = AF_INET;
  localAddress.sin_addr.s_addr = INADDR_ANY;
  localAddress.sin_port = 0;

  status = Socket->WskConnectionDispatch->WskBind(
    Socket->WskSocket,
    (PSOCKADDR)&localAddress,
    0,
    Socket->AsyncContext.Irp);

  KspAsyncContextWaitForCompletion(&Socket->AsyncContext, &status);

  if (NT_SUCCESS(status))
  {
    // Reset the async context (again)
    KspAsyncContextReset(&Socket->AsyncContext);

    // Connect to the remote host
    status = Socket->WskConnectionDispatch->WskConnect(
      Socket->WskSocket,
      RemoteAddress,
      0,
      Socket->AsyncContext.Irp);

    KspAsyncContextWaitForCompletion(&Socket->AsyncContext, &status);
  }

  return status;
}

NTSTATUS
WskSendRecv(
  PKSOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags,
  BOOLEAN Send)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Wrap the buffer into the WSK buffer
  WSK_BUF wskBuffer;
  wskBuffer.Offset = 0;
  wskBuffer.Length = *Length;
  wskBuffer.Mdl = IoAllocateMdl(Buffer, (UINT32)wskBuffer.Length, FALSE, FALSE, NULL);

  __try
  {
    MmProbeAndLockPages(wskBuffer.Mdl, KernelMode, IoWriteAccess);
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    status = STATUS_ACCESS_VIOLATION;

    goto Error;
  }

  // Reset the async context
  KspAsyncContextReset(&Socket->AsyncContext);

  // Send/Receive the data
  if (Send)
  {
    status = Socket->WskConnectionDispatch->WskSend(
      Socket->WskSocket,
      &wskBuffer,
      Flags,
      Socket->AsyncContext.Irp);
  }
  else
  {
    status = Socket->WskConnectionDispatch->WskReceive(
      Socket->WskSocket,
      &wskBuffer,
      Flags,
      Socket->AsyncContext.Irp);
  }

  KspAsyncContextWaitForCompletion(&Socket->AsyncContext, &status);

  // Set the number of bytes sent/received
  if (NT_SUCCESS(status))
  {
    *Length = (UINT32)Socket->AsyncContext.Irp->IoStatus.Information;
  }

  // Free the MDL
  MmUnlockPages(wskBuffer.Mdl);

Error:

  IoFreeMdl(wskBuffer.Mdl);

  return status;
}

NTSTATUS
WskSend(
  PKSOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags)
{
  return WskSendRecv(Socket, Buffer, Length, Flags, TRUE);
}

NTSTATUS
WskRecv(
  PKSOCKET Socket,
  PVOID Buffer,
  PUINT32 Length,
  UINT32 Flags)
{
  return WskSendRecv(Socket, Buffer, Length, Flags, FALSE);
}