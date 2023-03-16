#include <ntddk.h>

#include "proc.h"
#include "undoc.h"
#include "ia32.h"

#define LOG(FMT, ...) DbgPrintEx(0, 0, FMT, __VA_ARGS__)

#define IOCTRL_SCAN_PROCESS_PHYSICAL_PAGES CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0100, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

static PDEVICE_OBJECT sDeviceHandle = NULL;

void Scan1GbPdt(PVOID Page) { LOG("[PDT] Virtual-Page:%p Physical-Page:%p\n", Page, (PVOID)MmGetPhysicalAddress(Page).QuadPart); }
void Scan2MbPt(PVOID Page) { LOG("[PT ] Virtual-Page:%p Physical-Page:%p\n", Page, (PVOID)MmGetPhysicalAddress(Page).QuadPart); }
void ScanPage(PVOID Page) { LOG("[P  ] Virtual-Page:%p Physical-Page:%p\n", Page, (PVOID)MmGetPhysicalAddress(Page).QuadPart); }

UINT32 GetPhysicalMemoryRangeCount(
  PPHYSICAL_MEMORY_RANGE Pmr)
{
  UINT32 count = 0;
  while (Pmr[count].BaseAddress.QuadPart || Pmr[count].NumberOfBytes.QuadPart) count++;
  return count;
}

BOOLEAN IsInPhysicalMemoryRange(
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

NTSTATUS
WalkProcessPhysicalPages(
  UINT32 Pid)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  __try
  {
    // Get process
    PEPROCESS process = GetProcessByProcessId((HANDLE)Pid);
    if (process)
    {
      // Get physical ranges
      PPHYSICAL_MEMORY_RANGE pmr = MmGetPhysicalMemoryRanges();
      UINT32 pmrCount = GetPhysicalMemoryRangeCount(pmr);
      PHYSICAL_ADDRESS phys;
      // Walk page map level 4 table
      phys.QuadPart = (UINT64)process->Pcb.DirectoryTableBase;
      PPML4T pml4t = (PPML4T)MmGetVirtualForPhysical(phys);
      if (pml4t && MmIsAddressValid(pml4t))
      {
        for (UINT32 pml4i = 0; pml4i < 512; pml4i++)
        {
          if (pml4t[pml4i].Flags.Present)
          {
            // Walk page directory pointer table
            phys.QuadPart = pml4t[pml4i].Flags.PageFrameNumber << 12;
            PPDPT pdpt = (PPDPT)(MmGetVirtualForPhysical(phys));
            if (pdpt && MmIsAddressValid(pdpt))
            {
              for (UINT32 pdpi = 0; pdpi < 512; pdpi++)
              {
                if (pdpt[pdpi].Flags.Present)
                {
                  // Walk page directory table
                  phys.QuadPart = pdpt[pdpi].Flags.PageFrameNumber << 12;
                  PPDT pdt = (PPDT)MmGetVirtualForPhysical(phys);
                  if (pdt && MmIsAddressValid(pdt))
                  {
                    if (pdpt[pdpi].Flags.LargePage)
                    {
                      Scan1GbPdt(pdt);
                    }
                    else
                    {
                      for (UINT32 pdi = 0; pdi < 512; pdi++)
                      {
                        if (pdt[pdi].Flags.Present)
                        {
                          // Walk page table
                          phys.QuadPart = pdt[pdi].Flags.PageFrameNumber << 12;
                          PPT pt = (PPT)MmGetVirtualForPhysical(phys);
                          if (pt && MmIsAddressValid(pt))
                          {
                            if (pdt[pdi].Flags.LargePage)
                            {
                              Scan2MbPt(pt);
                            }
                            else
                            {
                              for (UINT32 pi = 0; pi < 512; pi++)
                              {
                                if (pt[pi].Flags.Present)
                                {
                                  // Walk page
                                  phys.QuadPart = pt[pi].Flags.PageFrameNumber << 12;
                                  PPT p = (PPT)MmGetVirtualForPhysical(phys);
                                  if (p && MmIsAddressValid(p))
                                  {
                                    if (IsInPhysicalMemoryRange(pmr, pmrCount, phys))
                                    {
                                      ScanPage((PVOID)p);
                                    }
                                  }
                                  else
                                  {
                                    LOG("Invalid page\n");
                                  }
                                }
                              }
                            }
                          }
                          else
                          {
                            LOG("Invalid page table\n");
                          }
                        }
                      }
                    }
                  }
                  else
                  {
                    LOG("Invalid page directory table\n");
                  }
                }
              }
            }
            else
            {
              LOG("Invalid page directory pointer table\n");
            }
          }
        }
      }
      else
      {
        LOG("Invalid page map level 4 table\n");
      }
      ObDereferenceObject(process);
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

NTSTATUS
OnIrpDflt(
  PDEVICE_OBJECT Device,
  PIRP Irp)
{
  UNREFERENCED_PARAMETER(Device);
  Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
  Irp->IoStatus.Information = 0;
  IoCompleteRequest(Irp, IO_NO_INCREMENT);
  return Irp->IoStatus.Status;
}

NTSTATUS
OnIrpCreate(
  PDEVICE_OBJECT Device,
  PIRP Irp)
{
  UNREFERENCED_PARAMETER(Device);
  Irp->IoStatus.Status = STATUS_SUCCESS;
  Irp->IoStatus.Information = 0;
  IoCompleteRequest(Irp, IO_NO_INCREMENT);
  return Irp->IoStatus.Status;
}

NTSTATUS
OnIrpCtrl(
  PDEVICE_OBJECT Device,
  PIRP Irp)
{
  UNREFERENCED_PARAMETER(Device);
  PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
  switch (stack->Parameters.DeviceIoControl.IoControlCode)
  {
    case IOCTRL_SCAN_PROCESS_PHYSICAL_PAGES:
    {
      UINT32 pid = *(PUINT32)Irp->AssociatedIrp.SystemBuffer;
      Irp->IoStatus.Status = WalkProcessPhysicalPages(pid);
      Irp->IoStatus.Information = NT_SUCCESS(Irp->IoStatus.Status) ? 0 : 0;
      break;
    }
  }
  IoCompleteRequest(Irp, IO_NO_INCREMENT);
  return Irp->IoStatus.Status;
}

NTSTATUS
OnIrpClose(
  PDEVICE_OBJECT Device,
  PIRP Irp)
{
  UNREFERENCED_PARAMETER(Device);
  Irp->IoStatus.Status = STATUS_SUCCESS;
  Irp->IoStatus.Information = 0;
  IoCompleteRequest(Irp, IO_NO_INCREMENT);
  return Irp->IoStatus.Status;
}

VOID
DriverUnload(
  PDRIVER_OBJECT Driver)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  UNREFERENCED_PARAMETER(Driver);
  // Destroy communication device
  UNICODE_STRING symbolName = RTL_CONSTANT_STRING(L"\\DosDevices\\PageWalk");
  status = IoDeleteSymbolicLink(&symbolName);
  if (NT_SUCCESS(status))
  {
    IoDeleteDevice(sDeviceHandle);
  }
  // Check driver unload successfully
  if (NT_SUCCESS(status))
  {
    LOG("Unloaded\n");
  }
}

NTSTATUS
DriverEntry(
  PDRIVER_OBJECT Driver,
  PUNICODE_STRING RegPath)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  UNREFERENCED_PARAMETER(RegPath);
  // Setup driver unload procedure
  Driver->DriverUnload = DriverUnload;
  // Setup irp handlers
  for (UINT32 i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
  {
    Driver->MajorFunction[i] = OnIrpDflt;
  }
  Driver->MajorFunction[IRP_MJ_CREATE] = OnIrpCreate;
  Driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = OnIrpCtrl;
  Driver->MajorFunction[IRP_MJ_CLOSE] = OnIrpClose;
  // Create communication device
  UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\PageWalk");
  UNICODE_STRING symbolName = RTL_CONSTANT_STRING(L"\\DosDevices\\PageWalk");
  status = IoCreateDevice(Driver, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, 0, &sDeviceHandle);
  if (NT_SUCCESS(status))
  {
    sDeviceHandle->Flags &= ~DO_DEVICE_INITIALIZING;
    IoCreateSymbolicLink(&symbolName, &deviceName);
  }
  // Check driver load successfully
  if (NT_SUCCESS(status))
  {
    LOG("Loaded\n");
  }
  return status;
}