#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <handleapi.h>
#include <winioctl.h>
#include <ioapiset.h>
#include <fileapi.h>

#define IOCTRL_SCAN_PROCESS_PHYSICAL_PAGES CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0100, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

static HANDLE gDriverHandle = INVALID_HANDLE_VALUE;

INT32 main(INT32 Argc, PCHAR Argv[])
{
  gDriverHandle = CreateFileA("\\\\.\\PageWalk", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
  if (gDriverHandle != INVALID_HANDLE_VALUE)
  {
    if (strcmp("scan", Argv[1]) == 0)
    {
      UINT32 pid = atoi(Argv[2]);
      DeviceIoControl(gDriverHandle, IOCTRL_SCAN_PROCESS_PHYSICAL_PAGES, &pid, sizeof(UINT32), NULL, 0, NULL, NULL);
    }
  }
  return 0;
}