#include "ioctl.h"

NTSTATUS
ScanProcessPhysicalPages(
  UINT32 Pid);

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

  // Process io requests
  PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
  switch (stack->Parameters.DeviceIoControl.IoControlCode)
  {
  case IOCTRL_SCAN_PROCESS_PHYSICAL_PAGES:
  {
    UINT32 pid = *(PUINT32)Irp->AssociatedIrp.SystemBuffer;
    Irp->IoStatus.Status = ScanProcessPhysicalPages(pid);
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