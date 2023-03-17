#include <ntddk.h>
#include <handleapi.h>

#include "ioctl.h"
#include "proc.h"
#include "undoc.h"
#include "ia32.h"
#include "socket.h"

#define LOG(FMT, ...) DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, FMT, __VA_ARGS__)

static HANDLE sTcpServerThreadHandle = INVALID_HANDLE_VALUE;
static PDEVICE_OBJECT sDeviceHandle = NULL;
static PPHYSICAL_MEMORY_RANGE sPhysicalMemoryRange = NULL;
static UINT32 sPhysicalMemoryRangeCount = 0;

VOID Scan1GbPageDirectoryTable(PVOID Page) { LOG("[PDT] Virtual-Page:%p Physical-Page:%p\n", Page, (PVOID)MmGetPhysicalAddress(Page).QuadPart); }
VOID Scan2MbPageTable(PVOID Page) { LOG("[PT ] Virtual-Page:%p Physical-Page:%p\n", Page, (PVOID)MmGetPhysicalAddress(Page).QuadPart); }
VOID ScanPage(PVOID Page) { LOG("[P  ] Virtual-Page:%p Physical-Page:%p\n", Page, (PVOID)MmGetPhysicalAddress(Page).QuadPart); }

UINT32
GetPhysicalMemoryRangeCount(
  PPHYSICAL_MEMORY_RANGE Pmr)
{
  UINT32 count = 0;

  while (Pmr[count].BaseAddress.QuadPart || Pmr[count].NumberOfBytes.QuadPart) count++;

  return count;
}

BOOLEAN
IsInPhysicalMemoryRange(
  PPHYSICAL_MEMORY_RANGE Pmr,
  UINT32 PmrCount,
  PHYSICAL_ADDRESS Address)
{
  for (UINT64 i = 0; i < PmrCount; i++)
  {
    if ((Address.QuadPart >= Pmr[i].BaseAddress.QuadPart) && (Address.QuadPart <= Pmr[i].BaseAddress.QuadPart + Pmr[i].NumberOfBytes.QuadPart))
    {
      return TRUE;
    }
  }

  return FALSE;
}

VOID
IteratePage(
  PHYSICAL_ADDRESS Address)
{
  PVOID page = MmGetVirtualForPhysical(Address);
  if (page && MmIsAddressValid(page))
  {
    if (IsInPhysicalMemoryRange(sPhysicalMemoryRange, sPhysicalMemoryRangeCount, Address))
    {
      ScanPage(page);
    }
  }
  else
  {
    LOG("Invalid page\n");
  }
}

VOID
IteratePageTable(
  PHYSICAL_ADDRESS Address,
  BOOLEAN LargePage)
{
  PPT pageTable = (PPT)MmGetVirtualForPhysical(Address);
  if (pageTable && MmIsAddressValid(pageTable))
  {
    if (LargePage)
    {
      Scan2MbPageTable(pageTable);
    }
    else
    {
      for (UINT32 i = 0; i < 512; i++)
      {
        if (pageTable[i].Flags.Present)
        {
          PHYSICAL_ADDRESS address = { .QuadPart = pageTable[i].Flags.PageFrameNumber << 12 };
          IteratePage(address);
        }
      }
    }
  }
  else
  {
    LOG("Invalid page table\n");
  }
}

VOID
IteratePageDirectoryTable(
  PHYSICAL_ADDRESS Address,
  BOOLEAN LargePage)
{
  PPDT pageDirectoryTable = (PPDT)MmGetVirtualForPhysical(Address);
  if (pageDirectoryTable && MmIsAddressValid(pageDirectoryTable))
  {
    if (LargePage)
    {
      Scan1GbPageDirectoryTable(pageDirectoryTable);
    }
    else
    {
      for (UINT32 i = 0; i < 512; i++)
      {
        if (pageDirectoryTable[i].Flags.Present)
        {
          PHYSICAL_ADDRESS address = { .QuadPart = pageDirectoryTable[i].Flags.PageFrameNumber << 12 };
          IteratePageTable(address, (BOOLEAN)pageDirectoryTable[i].Flags.LargePage);
        }
      }
    }
  }
  else
  {
    LOG("Invalid page directory table\n");
  }
}

VOID
IteratePageDirectoryPointerTable(
  PHYSICAL_ADDRESS Address)
{
  PPDPT pageDirectoryPointerTable = (PPDPT)MmGetVirtualForPhysical(Address);
  if (pageDirectoryPointerTable && MmIsAddressValid(pageDirectoryPointerTable))
  {
    for (UINT32 i = 0; i < 512; i++)
    {
      if (pageDirectoryPointerTable[i].Flags.Present)
      {
        PHYSICAL_ADDRESS address = { .QuadPart = pageDirectoryPointerTable[i].Flags.PageFrameNumber << 12 };
        IteratePageDirectoryTable(address, (BOOLEAN)pageDirectoryPointerTable[i].Flags.LargePage);
      }
    }
  }
  else
  {
    LOG("Invalid page directory pointer table\n");
  }
}

VOID
IteratePageMapLevel4Table(
  PHYSICAL_ADDRESS Address)
{
  PPML4T pageMapLevel4Table = (PPML4T)MmGetVirtualForPhysical(Address);
  if (pageMapLevel4Table && MmIsAddressValid(pageMapLevel4Table))
  {
    for (UINT32 i = 0; i < 512; i++)
    {
      if (pageMapLevel4Table[i].Flags.Present)
      {
        PHYSICAL_ADDRESS address = { .QuadPart = pageMapLevel4Table[i].Flags.PageFrameNumber << 12 };
        IteratePageDirectoryPointerTable(address);
      }
    }
  }
  else
  {
    LOG("Invalid page map level 4 table\n");
  }
}

NTSTATUS
ScanProcessPhysicalPages(
  UINT32 Pid)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;

  __try
  {
    // Open process
    PEPROCESS process = GetProcessByProcessId((HANDLE)Pid);
    if (process)
    {
      // Start scanning pages
      PHYSICAL_ADDRESS address = { .QuadPart = process->Pcb.DirectoryTableBase };
      IteratePageMapLevel4Table(address);

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
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    LOG("Something went wrong\n");
  }

  return status;
}

VOID
TcpServerThread(
  PVOID Context)
{
  UNREFERENCED_PARAMETER(Context);

  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Initialize winsock
  status = WskInitialize();
  if (NT_SUCCESS(status))
  {
    __try
    {
      CHAR sendBuffer[] = "Hello from page walker";
      CHAR recvBuffer[1024] = { 0 };

      PKSOCKET serverSocket = NULL;
      status = WskCreateListenSocket(&serverSocket, (ADDRESS_FAMILY)AF_INET, (UINT16)SOCK_STREAM, IPPROTO_TCP);

      SOCKADDR_IN address;
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = INADDR_ANY;
      address.sin_port = HTONS(9095);
      status = WskBind(serverSocket, (PSOCKADDR)&address);

      PKSOCKET clientSocket = NULL;
      status = WskAccept(serverSocket, &clientSocket, NULL, (PSOCKADDR)&address);

      UINT32 recvBufferSize = sizeof(recvBuffer) - 1;
      status = WskRecv(clientSocket, recvBuffer, &recvBufferSize, 0);

      LOG("Received: %s\n", recvBuffer);

      UINT32 sendBufferSize = sizeof(sendBuffer);
      status = WskSend(clientSocket, sendBuffer, &sendBufferSize, 0);

      status = WskCloseSocket(clientSocket);
      status = WskCloseSocket(serverSocket);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
      LOG("Something went wrong\n");
    }

    // Deinitialize winsock
    WskDestroy();
  }
}

VOID
DriverUnload(
  PDRIVER_OBJECT Driver)
{
  UNREFERENCED_PARAMETER(Driver);

  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Destroy communication device
  UNICODE_STRING symbolName = RTL_CONSTANT_STRING(L"\\DosDevices\\PageWalk");
  status = IoDeleteSymbolicLink(&symbolName);
  if (NT_SUCCESS(status))
  {
    IoDeleteDevice(sDeviceHandle);
  }
}

NTSTATUS
DriverEntry(
  PDRIVER_OBJECT Driver,
  PUNICODE_STRING RegPath)
{
  UNREFERENCED_PARAMETER(RegPath);

  NTSTATUS status = STATUS_UNSUCCESSFUL;

  // Setup driver unload procedure
  Driver->DriverUnload = DriverUnload;

  // Setup irp handlers
  for (UINT32 i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
  {
    Driver->MajorFunction[i] = OnIrpDflt;
  }
  Driver->MajorFunction[IRP_MJ_CREATE] = OnIrpCreate;
  Driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = OnIrpCtrl;
  Driver->MajorFunction[IRP_MJ_CLOSE] = OnIrpClose;

  // Get physical memory ranges
  sPhysicalMemoryRange = MmGetPhysicalMemoryRanges();
  sPhysicalMemoryRangeCount = GetPhysicalMemoryRangeCount(sPhysicalMemoryRange);

  // Create communication device
  UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\PageWalk");
  UNICODE_STRING symbolName = RTL_CONSTANT_STRING(L"\\DosDevices\\PageWalk");
  status = IoCreateDevice(Driver, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, 0, &sDeviceHandle);
  if (NT_SUCCESS(status))
  {
    sDeviceHandle->Flags &= ~DO_DEVICE_INITIALIZING;
    IoCreateSymbolicLink(&symbolName, &deviceName);
  }

  // Create TCP listen thread
  return PsCreateSystemThread(&sTcpServerThreadHandle, THREAD_ALL_ACCESS, 0, 0, 0, TcpServerThread, 0);
}