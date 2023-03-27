#ifndef EPROCESS_H
#define EPROCESS_H

#include "../config.h"

#if defined(OS_BUILD_NUMBER_22621_1413)

/*
0x000 Pcb              : _KPROCESS
0x438 ProcessLock : _EX_PUSH_LOCK
0x440 UniqueProcessId : 0x00000000`00000df0 Void
0x448 ActiveProcessLinks : _LIST_ENTRY[0xffffb00a`de278508 - 0xffffb00a`df4da4c8]
0x458 RundownProtect   : _EX_RUNDOWN_REF
0x460 Flags2 : 0xd080
0x460 JobNotReallyActive : 0y0
0x460 AccountingFolded : 0y0
0x460 NewProcessReported : 0y0
0x460 ExitProcessReported : 0y0
0x460 ReportCommitChanges : 0y0
0x460 LastReportMemory : 0y0
0x460 ForceWakeCharge : 0y0
0x460 CrossSessionCreate : 0y1
0x460 NeedsHandleRundown : 0y0
0x460 RefTraceEnabled : 0y0
0x460 PicoCreated : 0y0
0x460 EmptyJobEvaluated : 0y0
0x460 DefaultPagePriority : 0y101
0x460 PrimaryTokenFrozen : 0y1
0x460 ProcessVerifierTarget : 0y0
0x460 RestrictSetThreadContext : 0y0
0x460 AffinityPermanent : 0y0
0x460 AffinityUpdateEnable : 0y0
0x460 PropagateNode : 0y0
0x460 ExplicitAffinity : 0y0
0x460 ProcessExecutionState : 0y00
0x460 EnableReadVmLogging : 0y0
0x460 EnableWriteVmLogging : 0y0
0x460 FatalAccessTerminationRequested : 0y0
0x460 DisableSystemAllowedCpuSet : 0y0
0x460 ProcessStateChangeRequest : 0y00
0x460 ProcessStateChangeInProgress : 0y0
0x460 InPrivate : 0y0
0x464 Flags : 0x144d0c01
0x464 CreateReported : 0y1
0x464 NoDebugInherit : 0y0
0x464 ProcessExiting : 0y0
0x464 ProcessDelete : 0y0
0x464 ManageExecutableMemoryWrites : 0y0
0x464 VmDeleted : 0y0
0x464 OutswapEnabled : 0y0
0x464 Outswapped : 0y0
0x464 FailFastOnCommitFail : 0y0
0x464 Wow64VaSpace4Gb : 0y0
0x464 AddressSpaceInitialized : 0y11
0x464 SetTimerResolution : 0y0
0x464 BreakOnTermination : 0y0
0x464 DeprioritizeViews : 0y0
0x464 WriteWatch : 0y0
0x464 ProcessInSession : 0y1
0x464 OverrideAddressSpace : 0y0
0x464 HasAddressSpace : 0y1
0x464 LaunchPrefetched : 0y1
0x464 Reserved : 0y0
0x464 VmTopDown : 0y0
0x464 ImageNotifyDone : 0y1
0x464 PdeUpdateNeeded : 0y0
0x464 VdmAllowed : 0y0
0x464 ProcessRundown : 0y0
0x464 ProcessInserted : 0y1
0x464 DefaultIoPriority : 0y010
0x464 ProcessSelfDelete : 0y0
0x464 SetTimerResolutionLink : 0y0
0x468 CreateTime : _LARGE_INTEGER 0x01d960fd`2356ce07
0x470 ProcessQuotaUsage : [2] 0xa518
0x480 ProcessQuotaPeak : [2] 0xfe70
0x490 PeakVirtualSize : 0x00000201`1c8ec000
0x498 VirtualSize      : 0x00000201`1ada9000
0x4a0 SessionProcessLinks : _LIST_ENTRY[0xffffb00a`de278560 - 0xffffb00a`dea95560]
0x4b0 ExceptionPortData : 0xffffb00a`dbfd1d30 Void
0x4b0 ExceptionPortValue : 0xffffb00a`dbfd1d30
0x4b0 ExceptionPortState : 0y000
0x4b8 Token : _EX_FAST_REF
0x4c0 MmReserved : 0
0x4c8 AddressCreationLock : _EX_PUSH_LOCK
0x4d0 PageTableCommitmentLock : _EX_PUSH_LOCK
0x4d8 RotateInProgress : (null)
0x4e0 ForkInProgress : (null)
0x4e8 CommitChargeJob : (null)
0x4f0 CloneRoot : _RTL_AVL_TREE
0x4f8 NumberOfPrivatePages : 0x24f4
0x500 NumberOfLockedPages : 0
0x508 Win32Process : 0xffffe505`9958d010 Void
0x510 Job              : (null)
0x518 SectionObject : 0xffffe505`96b38d10 Void
0x520 SectionBaseAddress : 0x00007ff6`51df0000 Void
0x528 Cookie           : 0xae6bedcc
0x530 WorkingSetWatch : (null)
0x538 Win32WindowStation : 0x00000000`00000130 Void
0x540 InheritedFromUniqueProcessId : 0x00000000`00001a5c Void
0x548 OwnerProcessId   : 0x1b8e
0x550 Peb : 0x0000002f`facc6000 _PEB
0x558 Session          : 0xffffb00a`dbeedcb0 _MM_SESSION_SPACE
0x560 Spare1           : (null)
0x568 QuotaBlock : 0xffffb00a`dc115cc0 _EPROCESS_QUOTA_BLOCK
0x570 ObjectTable      : 0xffffe505`99915c40 _HANDLE_TABLE
0x578 DebugPort        : (null)
0x580 WoW64Process : (null)
0x588 DeviceMap : _EX_FAST_REF
0x590 EtwDataSource : 0xffffb00a`db4a3a10 Void
0x598 PageDirectoryPte : 0
0x5a0 ImageFilePointer : 0xffffb00a`d8f5b300 _FILE_OBJECT
0x5a8 ImageFileName    : [15] "Taskmgr.exe"
0x5b7 PriorityClass : 0x2 ''
0x5b8 SecurityPort : (null)
0x5c0 SeAuditProcessCreationInfo : _SE_AUDIT_PROCESS_CREATION_INFO
0x5c8 JobLinks : _LIST_ENTRY[0x00000000`00000000 - 0x00000000`00000000]
0x5d8 HighestUserAddress : 0x00007fff`ffff0000 Void
0x5e0 ThreadListHead   : _LIST_ENTRY[0xffffb00a`dcdb65b8 - 0xffffb00a`dffa85b8]
0x5f0 ActiveThreads    : 0x14
0x5f4 ImagePathHash : 0x4c8500ba
0x5f8 DefaultHardErrorProcessing : 0x8000
0x5fc LastThreadExitStatus : 0n0
0x600 PrefetchTrace : _EX_FAST_REF
0x608 LockedPagesList : (null)
0x610 ReadOperationCount : _LARGE_INTEGER 0x42
0x618 WriteOperationCount : _LARGE_INTEGER 0x0
0x620 OtherOperationCount : _LARGE_INTEGER 0x70e
0x628 ReadTransferCount : _LARGE_INTEGER 0x3c1f
0x630 WriteTransferCount : _LARGE_INTEGER 0x0
0x638 OtherTransferCount : _LARGE_INTEGER 0x5d72
0x640 CommitChargeLimit : 0
0x648 CommitCharge : 0x2c69
0x650 CommitChargePeak : 0x3d3d
0x680 Vm : _MMSUPPORT_FULL
0x7c0 MmProcessLinks : _LIST_ENTRY[0xffffb00a`de278880 - 0xffffb00a`df4da840]
0x7d0 ModifiedPageCount : 0xdb3
0x7d4 ExitStatus : 0n259
0x7d8 VadRoot : _RTL_AVL_TREE
0x7e0 VadHint : 0xffffb00a`dfbc37c0 Void
0x7e8 VadCount         : 0x130
0x7f0 VadPhysicalPages : 0
0x7f8 VadPhysicalPagesLimit : 0
0x800 AlpcContext : _ALPC_PROCESS_CONTEXT
0x820 TimerResolutionLink : _LIST_ENTRY[0x00000000`00000000 - 0x00000000`00000000]
0x830 TimerResolutionStackRecord : (null)
0x838 RequestedTimerResolution : 0
0x83c SmallestTimerResolution : 0
0x840 ExitTime : _LARGE_INTEGER 0x0
0x848 InvertedFunctionTable : 0xffffe505`998a8da0 _INVERTED_FUNCTION_TABLE_KERNEL_MODE
0x850 InvertedFunctionTableLock : _EX_PUSH_LOCK
0x858 ActiveThreadsHighWatermark : 0x20
0x85c LargePrivateVadCount : 0
0x860 ThreadListLock : _EX_PUSH_LOCK
0x868 WnfContext : 0xffffe505`9705f120 Void
0x870 ServerSilo       : (null)
0x878 SignatureLevel : 0 ''
0x879 SectionSignatureLevel : 0 ''
0x87a Protection : _PS_PROTECTION
0x87b HangCount : 0y000
0x87b GhostCount : 0y000
0x87b PrefilterException : 0y0
0x87c Flags3 : 0x3040c000
0x87c Minimal : 0y0
0x87c ReplacingPageRoot : 0y0
0x87c Crashed : 0y0
0x87c JobVadsAreTracked : 0y0
0x87c VadTrackingDisabled : 0y0
0x87c AuxiliaryProcess : 0y0
0x87c SubsystemProcess : 0y0
0x87c IndirectCpuSets : 0y0
0x87c RelinquishedCommit : 0y0
0x87c HighGraphicsPriority : 0y0
0x87c CommitFailLogged : 0y0
0x87c ReserveFailLogged : 0y0
0x87c SystemProcess : 0y0
0x87c HideImageBaseAddresses : 0y0
0x87c AddressPolicyFrozen : 0y1
0x87c ProcessFirstResume : 0y1
0x87c ForegroundExternal : 0y0
0x87c ForegroundSystem : 0y0
0x87c HighMemoryPriority : 0y0
0x87c EnableProcessSuspendResumeLogging : 0y0
0x87c EnableThreadSuspendResumeLogging : 0y0
0x87c SecurityDomainChanged : 0y0
0x87c SecurityFreezeComplete : 0y1
0x87c VmProcessorHost : 0y0
0x87c VmProcessorHostTransition : 0y0
0x87c AltSyscall : 0y0
0x87c TimerResolutionIgnore : 0y0
0x87c DisallowUserTerminate : 0y0
0x87c EnableProcessRemoteExecProtectVmLogging : 0y1
0x87c EnableProcessLocalExecProtectVmLogging : 0y1
0x87c MemoryCompressionProcess : 0y0
0x880 DeviceAsid : 0n0
0x888 SvmData : (null)
0x890 SvmProcessLock : _EX_PUSH_LOCK
0x898 SvmLock : 0
0x8a0 SvmProcessDeviceListHead : _LIST_ENTRY[0xffffb00a`dfe9a920 - 0xffffb00a`dfe9a920]
0x8b0 LastFreezeInterruptTime : 0
0x8b8 DiskCounters : 0xffffb00a`dfe9ac00 _PROCESS_DISK_COUNTERS
0x8c0 PicoContext      : (null)
0x8c8 EnclaveTable : (null)
0x8d0 EnclaveNumber : 0
0x8d8 EnclaveLock : _EX_PUSH_LOCK
0x8e0 HighPriorityFaultsAllowed : 0
0x8e8 EnergyContext : 0xffffb00a`dfe9ac28 _PO_PROCESS_ENERGY_CONTEXT
0x8f0 VmContext        : (null)
0x8f8 SequenceNumber : 0x10a
0x900 CreateInterruptTime : 0x4ccb08c9
0x908 CreateUnbiasedInterruptTime : 0x4ccb08c9
0x910 TotalUnbiasedFrozenTime : 0
0x918 LastAppStateUpdateTime : 0x4ccb08c9
0x920 LastAppStateUptime : 0y0000000000000000000000000000000000000000000000000000000000000(0)
0x920 LastAppState : 0y000
0x928 SharedCommitCharge : 0xfed
0x930 SharedCommitLock : _EX_PUSH_LOCK
0x938 SharedCommitLinks : _LIST_ENTRY[0xffffe505`9834e818 - 0xffffe505`9a6c4fa8]
0x948 AllowedCpuSets   : 0
0x950 DefaultCpuSets : 0
0x948 AllowedCpuSetsIndirect : (null)
0x950 DefaultCpuSetsIndirect : (null)
0x958 DiskIoAttribution : (null)
0x960 DxgProcess : 0xffffe505`97da5d60 Void
0x968 Win32KFilterSet  : 0
0x96c Machine : 0x8664
0x96e Spare0 : 0
0x970 ProcessTimerDelay : _PS_INTERLOCKED_TIMER_DELAY_VALUES
0x978 KTimerSets : 0
0x97c KTimer2Sets : 0
0x980 ThreadTimerSets : 0x1821
0x988 VirtualTimerListLock : 0
0x990 VirtualTimerListHead : _LIST_ENTRY[0xffffb00a`dfe9aa10 - 0xffffb00a`dfe9aa10]
0x9a0 WakeChannel      : _WNF_STATE_NAME
0x9a0 WakeInfo : _PS_PROCESS_WAKE_INFORMATION
0x9d0 MitigationFlags : 0x1c000021
0x9d0 MitigationFlagsValues : <unnamed - tag>
0x9d4 MitigationFlags2 : 0x40000000
0x9d4 MitigationFlags2Values : <unnamed - tag>
0x9d8 PartitionObject : 0xffffb00a`d7ca9280 Void
0x9e0 SecurityDomain   : 0
0x9e8 ParentSecurityDomain : 0
0x9f0 CoverageSamplerContext : (null)
0x9f8 MmHotPatchContext : (null)
0xa00 IdealProcessorAssignmentBlock : _KE_IDEAL_PROCESSOR_ASSIGNMENT_BLOCK
0xb18 DynamicEHContinuationTargetsTree : _RTL_AVL_TREE
0xb20 DynamicEHContinuationTargetsLock : _EX_PUSH_LOCK
0xb28 DynamicEnforcedCetCompatibleRanges : _PS_DYNAMIC_ENFORCED_ADDRESS_RANGES
0xb38 DisabledComponentFlags : 0
0xb3c PageCombineSequence : 0n1
0xb40 EnableOptionalXStateFeaturesLock : _EX_PUSH_LOCK
0xb48 PathRedirectionHashes : (null)
0xb50 SyscallProvider : (null)
0xb58 SyscallProviderProcessLinks : _LIST_ENTRY[0x00000000`00000000 - 0x00000000`00000000]
0xb68 SyscallProviderDispatchContext : _PSP_SYSCALL_PROVIDER_DISPATCH_CONTEXT
0xb70 MitigationFlags3 : 0
0xb70 MitigationFlags3Values : <unnamed - tag>
*/

#elif defined(OS_BUILD_NUMBER_19044_2728)

// NO DATA AVAILABLE

#endif

#endif