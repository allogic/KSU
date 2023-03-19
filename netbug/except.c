#include "except.h"

INT32
NbExceptionHandler(
  INT32 Code,
  PEXCEPTION_POINTERS Exception)
{
  UNREFERENCED_PARAMETER(Code);

  LOG("------------------------------------------------\n");
  LOG("Exception Record:\n");
  LOG("  ExceptionCode:    %016X\n", Exception->ExceptionRecord->ExceptionCode);
  LOG("  ExceptionFlags:   %016X\n", Exception->ExceptionRecord->ExceptionFlags);
  LOG("  ExceptionAddress: %p\n", Exception->ExceptionRecord->ExceptionAddress);
  LOG("  NumberParameters: %016X\n", Exception->ExceptionRecord->NumberParameters);
  LOG("------------------------------------------------\n");
  LOG("Segment Registers:\n");
  LOG("  SegCs:  %08X\n", Exception->ContextRecord->SegCs);
  LOG("  SegDs:  %08X\n", Exception->ContextRecord->SegDs);
  LOG("  SegEs:  %08X\n", Exception->ContextRecord->SegEs);
  LOG("  SegFs:  %08X\n", Exception->ContextRecord->SegFs);
  LOG("  SegGs:  %08X\n", Exception->ContextRecord->SegGs);
  LOG("  SegSs:  %08X\n", Exception->ContextRecord->SegSs);
  LOG("  EFlags: %08X\n", Exception->ContextRecord->EFlags);
  LOG("------------------------------------------------\n");
  LOG("Debug Registers:\n");
  LOG("  DR0: %p\n", (PVOID)Exception->ContextRecord->Dr0);
  LOG("  DR1: %p\n", (PVOID)Exception->ContextRecord->Dr1);
  LOG("  DR2: %p\n", (PVOID)Exception->ContextRecord->Dr2);
  LOG("  DR3: %p\n", (PVOID)Exception->ContextRecord->Dr3);
  LOG("  DR6: %p\n", (PVOID)Exception->ContextRecord->Dr6);
  LOG("  DR7: %p\n", (PVOID)Exception->ContextRecord->Dr7);
  LOG("------------------------------------------------\n");
  LOG("Integer Registers:\n");
  LOG("  RAX: %p\n", (PVOID)Exception->ContextRecord->Rax);
  LOG("  RCX: %p\n", (PVOID)Exception->ContextRecord->Rcx);
  LOG("  RDX: %p\n", (PVOID)Exception->ContextRecord->Rdx);
  LOG("  RBX: %p\n", (PVOID)Exception->ContextRecord->Rbx);
  LOG("  RSP: %p\n", (PVOID)Exception->ContextRecord->Rsp);
  LOG("  RBP: %p\n", (PVOID)Exception->ContextRecord->Rbp);
  LOG("  RSI: %p\n", (PVOID)Exception->ContextRecord->Rsi);
  LOG("  RDI: %p\n", (PVOID)Exception->ContextRecord->Rdi);
  LOG("  R8 : %p\n", (PVOID)Exception->ContextRecord->R8);
  LOG("  R9 : %p\n", (PVOID)Exception->ContextRecord->R9);
  LOG("  R10: %p\n", (PVOID)Exception->ContextRecord->R10);
  LOG("  R11: %p\n", (PVOID)Exception->ContextRecord->R11);
  LOG("  R12: %p\n", (PVOID)Exception->ContextRecord->R12);
  LOG("  R13: %p\n", (PVOID)Exception->ContextRecord->R13);
  LOG("  R14: %p\n", (PVOID)Exception->ContextRecord->R14);
  LOG("  R15: %p\n", (PVOID)Exception->ContextRecord->R15);
  LOG("------------------------------------------------\n");
  LOG("Program Counter:\n");
  LOG("  RIP: %p\n", (PVOID)Exception->ContextRecord->Rip);
  LOG("------------------------------------------------\n");
  LOG("Special Debug Control Registers:\n");
  LOG("  DebugControl:         %p\n", (PVOID)Exception->ContextRecord->DebugControl);
  LOG("  LastBranchToRip:      %p\n", (PVOID)Exception->ContextRecord->LastBranchToRip);
  LOG("  LastBranchFromRip:    %p\n", (PVOID)Exception->ContextRecord->LastBranchFromRip);
  LOG("  LastExceptionToRip:   %p\n", (PVOID)Exception->ContextRecord->LastExceptionToRip);
  LOG("  LastExceptionFromRip: %p\n", (PVOID)Exception->ContextRecord->LastExceptionFromRip);
  LOG("------------------------------------------------\n");

  return EXCEPTION_EXECUTE_HANDLER;
}