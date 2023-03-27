#ifndef UNDOC_H
#define UNDOC_H

#include "core.h"

///////////////////////////////////////////////////////////////
// Public Types
///////////////////////////////////////////////////////////////

typedef struct _LDR_DATA_TABLE_ENTRY
{
  LIST_ENTRY InLoadOrderLinks;
  LIST_ENTRY InMemoryOrderLinks;
  LIST_ENTRY InInitializationOrderLinks;
  PVOID DllBase;
  PVOID EntryPoint;
  ULONG SizeOfImage;
  UNICODE_STRING FullDllName;
  UNICODE_STRING BaseDllName;
  ULONG Flags;
  WORD LoadCount;
  WORD TlsIndex;
  union
  {
    LIST_ENTRY HashLinks;
    struct
    {
      PVOID SectionPointer;
      ULONG CheckSum;
    };
  };
  union
  {
    ULONG TimeDateStamp;
    PVOID LoadedImports;
  };
  PVOID EntryPointActivationContext;
  PVOID PatchInformation;
  LIST_ENTRY ForwarderLinks;
  LIST_ENTRY ServiceTagLinks;
  LIST_ENTRY StaticLinks;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct _PEB_LDR_DATA
{
  DWORD Length;
  BOOLEAN Initialized;
  PVOID SsHandler;
  LIST_ENTRY InLoadOrderModuleList;
  LIST_ENTRY InMemoryOrderModuleList;
  LIST_ENTRY InInitializationOrderModuleList;
  PVOID EntryInProgress;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _PEB
{
  union
  {
    struct
    {
      BYTE InheritedAddressSpace;
      BYTE ReadImageFileExecOptions;
      BYTE BeingDebugged;
      BYTE _SYSTEM_DEPENDENT_01;
    } flags;
    PVOID dummyalign;
  } dword0;
  PVOID Mutant;
  PVOID ImageBaseAddress;
  PPEB_LDR_DATA Ldr;
  PVOID ProcessParameters;
  PVOID SubSystemData;
  PVOID ProcessHeap;
  PVOID FastPebLock;
  PVOID _SYSTEM_DEPENDENT_02;
  PVOID _SYSTEM_DEPENDENT_03;
  PVOID _SYSTEM_DEPENDENT_04;
  union
  {
    PVOID KernelCallbackTable;
    PVOID UserSharedInfoPtr;
  } dummy00;
  DWORD SystemReserved;
  DWORD _SYSTEM_DEPENDENT_05;
  PVOID _SYSTEM_DEPENDENT_06;
  PVOID TlsExpansionCounter;
  PVOID TlsBitmap;
  DWORD TlsBitmapBits[2];
  PVOID ReadOnlySharedMemoryBase;
  PVOID _SYSTEM_DEPENDENT_07;
  PVOID ReadOnlyStaticServerData;
  PVOID AnsiCodePageData;
  PVOID OemCodePageData;
  PVOID UnicodeCaseTableData;
  DWORD NumberOfProcessors;
  union
  {
    DWORD NtGlobalFlag;
    DWORD dummy02;
  } dummy01;
  LARGE_INTEGER CriticalSectionTimeout;
  PVOID HeapSegmentReserve;
  PVOID HeapSegmentCommit;
  PVOID HeapDeCommitTotalFreeThreshold;
  PVOID HeapDeCommitFreeBlockThreshold;
  DWORD NumberOfHeaps;
  DWORD MaximumNumberOfHeaps;
  PVOID ProcessHeaps;
  PVOID GdiSharedHandleTable;
  PVOID ProcessStarterHelper;
  PVOID GdiDCAttributeList;
  PVOID LoaderLock;
  DWORD OSMajorVersion;
  DWORD OSMinorVersion;
  WORD OSBuildNumber;
  WORD OSCSDVersion;
  DWORD OSPlatformId;
  DWORD ImageSubsystem;
  DWORD ImageSubsystemMajorVersion;
  PVOID ImageSubsystemMinorVersion;
  union
  {
    PVOID ImageProcessAffinityMask;
    PVOID ActiveProcessAffinityMask;
  } dummy02;
  PVOID GdiHandleBuffer[30];
  PVOID PostProcessInitRoutine;
  PVOID TlsExpansionBitmap;
  DWORD TlsExpansionBitmapBits[32];
  PVOID SessionId;
  ULARGE_INTEGER AppCompatFlags;
  ULARGE_INTEGER AppCompatFlagsUser;
  PVOID pShimData;
  PVOID AppCompatInfo;
  UNICODE_STRING64 CSDVersion;
  PVOID ActivationContextData;
  PVOID ProcessAssemblyStorageMap;
  PVOID SystemDefaultActivationContextData;
  PVOID SystemAssemblyStorageMap;
  PVOID MinimumStackCommit;
} PEB, * PPEB;

typedef struct _KGDTENTRY
{
  WORD LimitLow;
  WORD BaseLow;
  ULONG HighWord;
} KGDTENTRY, * PKGDTENTRY;

typedef struct _KIDTENTRY
{
  WORD Offset;
  WORD Selector;
  WORD Access;
  WORD ExtendedOffset;
} KIDTENTRY, * PKIDTENTRY;

typedef struct _KEXECUTE_OPTIONS
{
  ULONG ExecuteDisable : 1;
  ULONG ExecuteEnable : 1;
  ULONG DisableThunkEmulation : 1;
  ULONG Permanent : 1;
  ULONG ExecuteDispatchEnable : 1;
  ULONG ImageDispatchEnable : 1;
  ULONG Spare : 2;
} KEXECUTE_OPTIONS, * PKEXECUTE_OPTIONS;

typedef struct _KPROCESS
{
  DISPATCHER_HEADER Header;
  LIST_ENTRY ProfileListHead;
  ULONGLONG DirectoryTableBase;
  KGDTENTRY LdtDescriptor;
  KIDTENTRY Int21Descriptor;
  WORD IopmOffset;
  UCHAR Iopl;
  UCHAR Unused;
  ULONG ActiveProcessors;
  ULONG KernelTime;
  ULONG UserTime;
  LIST_ENTRY ReadyListHead;
  SINGLE_LIST_ENTRY SwapListEntry;
  PVOID VdmTrapcHandler;
  LIST_ENTRY ThreadListHead;
  ULONG ProcessLock;
  ULONG Affinity;
  union
  {
    ULONG AutoAlignment : 1;
    ULONG DisableBoost : 1;
    ULONG DisableQuantum : 1;
    ULONG ReservedFlags : 29;
    LONG ProcessFlags;
  } Dummy1;
  CHAR BasePriority;
  CHAR QuantumReset;
  UCHAR State;
  UCHAR ThreadSeed;
  UCHAR PowerState;
  UCHAR IdealNode;
  UCHAR Visited;
  union
  {
    KEXECUTE_OPTIONS Flags;
    UCHAR ExecuteOptions;
  } Dummy2;
  ULONG StackCount;
  LIST_ENTRY ProcessListEntry;
  UINT64 CycleTime;
} KPROCESS, * PKPROCESS;

typedef struct _HANDLE_TRACE_DB_ENTRY
{
  CLIENT_ID ClientId;
  PVOID Handle;
  ULONG Type;
  VOID* StackTrace[16];
} HANDLE_TRACE_DB_ENTRY, * PHANDLE_TRACE_DB_ENTRY;

typedef struct _HANDLE_TRACE_DEBUG_INFO
{
  LONG RefCount;
  ULONG TableSize;
  ULONG BitMaskFlags;
  FAST_MUTEX CloseCompactionLock;
  ULONG CurrentStackIndex;
  HANDLE_TRACE_DB_ENTRY TraceDb[1];
} HANDLE_TRACE_DEBUG_INFO, * PHANDLE_TRACE_DEBUG_INFO;

typedef struct _HANDLE_TABLE_ENTRY_INFO
{
  ULONG AuditMask;
} HANDLE_TABLE_ENTRY_INFO, * PHANDLE_TABLE_ENTRY_INFO;

typedef struct _HANDLE_TABLE_ENTRY
{
  union
  {
    PVOID Object;
    ULONG ObAttributes;
    PHANDLE_TABLE_ENTRY_INFO InfoTable;
    ULONG Value;
  } Dummy1;
  union
  {
    ULONG GrantedAccess;
    struct
    {
      WORD GrantedAccessIndex;
      WORD CreatorBackTraceIndex;
    } Dummy3;
    LONG NextFreeTableEntry;
  } Dummy2;
} HANDLE_TABLE_ENTRY, * PHANDLE_TABLE_ENTRY;

typedef struct _HANDLE_TABLE
{
  ULONG TableCode;
  PEPROCESS QuotaProcess;
  PVOID UniqueProcessId;
  EX_PUSH_LOCK HandleLock;
  LIST_ENTRY HandleTableList;
  EX_PUSH_LOCK HandleContentionEvent;
  PHANDLE_TRACE_DEBUG_INFO DebugInfo;
  LONG ExtraInfoPages;
  ULONG Flags;
  ULONG StrictFIFO : 1;
  LONG FirstFreeHandle;
  PHANDLE_TABLE_ENTRY LastFreeHandleEntry;
  LONG HandleCount;
  ULONG NextHandleNeedingPool;
} HANDLE_TABLE, * PHANDLE_TABLE;

typedef struct _EX_FAST_REF
{
  union
  {
    PVOID Object;
    ULONG RefCnt : 3;
    ULONG Value;
  } Dummy1;
} EX_FAST_REF, * PEX_FAST_REF;

typedef struct _MMADDRESS_NODE
{
  ULONG u1;
  VOID* LeftChild;
  VOID* RightChild;
  ULONG StartingVpn;
  ULONG EndingVpn;
} MMADDRESS_NODE, * PMMADDRESS_NODE;

typedef struct _MM_AVL_TABLE
{
  MMADDRESS_NODE BalancedRoot;
  ULONG DepthOfTree : 5;
  ULONG Unused : 3;
  ULONG NumberGenericTableElements : 24;
  PVOID NodeHint;
  PVOID NodeFreeHint;
} MM_AVL_TABLE, * PMM_AVL_TABLE;

typedef struct _HARDWARE_PTE
{
  union
  {
    ULONG Valid : 1;
    ULONG Write : 1;
    ULONG Owner : 1;
    ULONG WriteThrough : 1;
    ULONG CacheDisable : 1;
    ULONG Accessed : 1;
    ULONG Dirty : 1;
    ULONG LargePage : 1;
    ULONG Global : 1;
    ULONG CopyOnWrite : 1;
    ULONG Prototype : 1;
    ULONG reserved0 : 1;
    ULONG PageFrameNumber : 26;
    ULONG reserved1 : 26;
    ULONG LowPart;
  } Dummy1;
  ULONG HighPart;
} HARDWARE_PTE, * PHARDWARE_PTE;

typedef struct _SE_AUDIT_PROCESS_CREATION_INFO
{
  POBJECT_NAME_INFORMATION ImageFileName;
} SE_AUDIT_PROCESS_CREATION_INFO, * PSE_AUDIT_PROCESS_CREATION_INFO;

typedef struct _MMSUPPORT_FLAGS
{
  ULONG SessionSpace : 1;
  ULONG ModwriterAttached : 1;
  ULONG TrimHard : 1;
  ULONG MaximumWorkingSetHard : 1;
  ULONG ForceTrim : 1;
  ULONG MinimumWorkingSetHard : 1;
  ULONG SessionMaster : 1;
  ULONG TrimmerAttached : 1;
  ULONG TrimmerDetaching : 1;
  ULONG Reserved : 7;
  ULONG MemoryPriority : 8;
  ULONG WsleDeleted : 1;
  ULONG VmExiting : 1;
  ULONG Available : 6;
} MMSUPPORT_FLAGS, * PMMSUPPORT_FLAGS;

typedef struct _MMWSLE
{
  ULONG u1;
} MMWSLE, * PMMWSLE;

typedef struct _MMWSLE_NONDIRECT_HASH
{
  PVOID Key;
  ULONG Index;
} MMWSLE_NONDIRECT_HASH, * PMMWSLE_NONDIRECT_HASH;

typedef struct _MMWSLE_HASH
{
  ULONG Index;
} MMWSLE_HASH, * PMMWSLE_HASH;

typedef struct _MMWSL
{
  ULONG FirstFree;
  ULONG FirstDynamic;
  ULONG LastEntry;
  ULONG NextSlot;
  PMMWSLE Wsle;
  PVOID LowestPagableAddress;
  ULONG LastInitializedWsle;
  ULONG NextEstimationSlot;
  ULONG NextAgingSlot;
  ULONG EstimatedAvailable;
  ULONG GrowthSinceLastEstimate;
  ULONG NumberOfCommittedPageTables;
  ULONG VadBitMapHint;
  ULONG NonDirectCount;
  PMMWSLE_NONDIRECT_HASH NonDirectHash;
  PMMWSLE_HASH HashTableStart;
  PMMWSLE_HASH HighestPermittedHashAddress;
  PVOID HighestUserAddress;
  WORD UsedPageTableEntries[1536];
  ULONG CommittedPageTables[48];
} MMWSL, * PMMWSL;

typedef struct _MMSUPPORT
{
  LIST_ENTRY WorkingSetExpansionLinks;
  WORD LastTrimStamp;
  WORD NextPageColor;
  MMSUPPORT_FLAGS Flags;
  ULONG PageFaultCount;
  ULONG PeakWorkingSetSize;
  ULONG Spare0;
  ULONG MinimumWorkingSetSize;
  ULONG MaximumWorkingSetSize;
  PMMWSL VmWorkingSetList;
  ULONG Claim;
  ULONG Spare[1];
  ULONG WorkingSetPrivateSize;
  ULONG WorkingSetSizeOverhead;
  ULONG WorkingSetSize;
  PKEVENT ExitEvent;
  EX_PUSH_LOCK WorkingSetMutex;
  PVOID AccessLog;
} MMSUPPORT, * PMMSUPPORT;

typedef struct _ALPC_PROCESS_CONTEXT
{
  EX_PUSH_LOCK Lock;
  LIST_ENTRY ViewListHead;
  ULONG PagedPoolQuotaCache;
} ALPC_PROCESS_CONTEXT, * PALPC_PROCESS_CONTEXT;

typedef struct _EPROCESS
{
  KPROCESS Pcb;
  EX_PUSH_LOCK ProcessLock;
  LARGE_INTEGER CreateTime;
  LARGE_INTEGER ExitTime;
  EX_RUNDOWN_REF RundownProtect;
  PVOID UniqueProcessId;
  LIST_ENTRY ActiveProcessLinks;
  ULONG QuotaUsage[3];
  ULONG QuotaPeak[3];
  ULONG CommitCharge;
  ULONG PeakVirtualSize;
  ULONG VirtualSize;
  LIST_ENTRY SessionProcessLinks;
  PVOID DebugPort;
  union
  {
    PVOID ExceptionPortData;
    ULONG ExceptionPortValue;
    ULONG ExceptionPortState : 3;
  } Dummy1;
  PHANDLE_TABLE ObjectTable;
  EX_FAST_REF Token;
  ULONG WorkingSetPage;
  EX_PUSH_LOCK AddressCreationLock;
  PETHREAD RotateInProgress;
  PETHREAD ForkInProgress;
  ULONG HardwareTrigger;
  PMM_AVL_TABLE PhysicalVadRoot;
  PVOID CloneRoot;
  ULONG NumberOfPrivatePages;
  ULONG NumberOfLockedPages;
  PVOID Win32Process;
  PEJOB Job;
  PVOID SectionObject;
  PVOID SectionBaseAddress;
  VOID* QuotaBlock;
  VOID* WorkingSetWatch;
  PVOID Win32WindowStation;
  PVOID InheritedFromUniqueProcessId;
  PVOID LdtInformation;
  PVOID VadFreeHint;
  PVOID VdmObjects;
  PVOID DeviceMap;
  PVOID EtwDataSource;
  PVOID FreeTebHint;
  union
  {
    HARDWARE_PTE PageDirectoryPte;
    UINT64 Filler;
  } Dummy2;
  PVOID Session;
  UCHAR ImageFileName[16];
  LIST_ENTRY JobLinks;
  PVOID LockedPagesList;
  LIST_ENTRY ThreadListHead;
  PVOID SecurityPort;
  PVOID PaeTop;
  ULONG ActiveThreads;
  ULONG ImagePathHash;
  ULONG DefaultHardErrorProcessing;
  LONG LastThreadExitStatus;
  PPEB Peb;
  EX_FAST_REF PrefetchTrace;
  LARGE_INTEGER ReadOperationCount;
  LARGE_INTEGER WriteOperationCount;
  LARGE_INTEGER OtherOperationCount;
  LARGE_INTEGER ReadTransferCount;
  LARGE_INTEGER WriteTransferCount;
  LARGE_INTEGER OtherTransferCount;
  ULONG CommitChargeLimit;
  ULONG CommitChargePeak;
  PVOID AweInfo;
  SE_AUDIT_PROCESS_CREATION_INFO SeAuditProcessCreationInfo;
  MMSUPPORT Vm;
  LIST_ENTRY MmProcessLinks;
  ULONG ModifiedPageCount;
  ULONG Flags2;
  ULONG JobNotReallyActive : 1;
  ULONG AccountingFolded : 1;
  ULONG NewProcessReported : 1;
  ULONG ExitProcessReported : 1;
  ULONG ReportCommitChanges : 1;
  ULONG LastReportMemory : 1;
  ULONG ReportPhysicalPageChanges : 1;
  ULONG HandleTableRundown : 1;
  ULONG NeedsHandleRundown : 1;
  ULONG RefTraceEnabled : 1;
  ULONG NumaAware : 1;
  ULONG ProtectedProcess : 1;
  ULONG DefaultPagePriority : 3;
  ULONG PrimaryTokenFrozen : 1;
  ULONG ProcessVerifierTarget : 1;
  ULONG StackRandomizationDisabled : 1;
  ULONG Flags;
  ULONG CreateReported : 1;
  ULONG NoDebugInherit : 1;
  ULONG ProcessExiting : 1;
  ULONG ProcessDelete : 1;
  ULONG Wow64SplitPages : 1;
  ULONG VmDeleted : 1;
  ULONG OutswapEnabled : 1;
  ULONG Outswapped : 1;
  ULONG ForkFailed : 1;
  ULONG Wow64VaSpace4Gb : 1;
  ULONG AddressSpaceInitialized : 2;
  ULONG SetTimerResolution : 1;
  ULONG BreakOnTermination : 1;
  ULONG DeprioritizeViews : 1;
  ULONG WriteWatch : 1;
  ULONG ProcessInSession : 1;
  ULONG OverrideAddressSpace : 1;
  ULONG HasAddressSpace : 1;
  ULONG LaunchPrefetched : 1;
  ULONG InjectInpageErrors : 1;
  ULONG VmTopDown : 1;
  ULONG ImageNotifyDone : 1;
  ULONG PdeUpdateNeeded : 1;
  ULONG VdmAllowed : 1;
  ULONG SmapAllowed : 1;
  ULONG ProcessInserted : 1;
  ULONG DefaultIoPriority : 3;
  ULONG SparePsFlags1 : 2;
  LONG ExitStatus;
  WORD Spare7;
  union
  {
    struct
    {
      UCHAR SubSystemMinorVersion;
      UCHAR SubSystemMajorVersion;
    } Dummy4;
    WORD SubSystemVersion;
  } Dummy3;
  UCHAR PriorityClass;
  MM_AVL_TABLE VadRoot;
  ULONG Cookie;
  ALPC_PROCESS_CONTEXT AlpcContext;
} EPROCESS;

typedef struct _DESCRIPTOR
{
  WORD Pad;
  WORD Limit;
  ULONG Base;
} DESCRIPTOR, * PDESCRIPTOR;

typedef struct _KSPECIAL_REGISTERS
{
  ULONG Cr0;
  ULONG Cr2;
  ULONG Cr3;
  ULONG Cr4;
  ULONG KernelDr0;
  ULONG KernelDr1;
  ULONG KernelDr2;
  ULONG KernelDr3;
  ULONG KernelDr6;
  ULONG KernelDr7;
  DESCRIPTOR Gdtr;
  DESCRIPTOR Idtr;
  WORD Tr;
  WORD Ldtr;
  ULONG Reserved[6];
} KSPECIAL_REGISTERS, * PKSPECIAL_REGISTERS;

typedef struct _KPROCESSOR_STATE
{
  CONTEXT ContextFrame;
  KSPECIAL_REGISTERS SpecialRegisters;
} KPROCESSOR_STATE, * PKPROCESSOR_STATE;

typedef struct _CACHED_KSTACK_LIST
{
  SLIST_HEADER SListHead;
  LONG MinimumFree;
  ULONG Misses;
  ULONG MissesLast;
} CACHED_KSTACK_LIST, * PCACHED_KSTACK_LIST;

typedef struct _KNODE
{
  SLIST_HEADER PagedPoolSListHead;
  SLIST_HEADER NonPagedPoolSListHead[3];
  SLIST_HEADER PfnDereferenceSListHead;
  ULONG ProcessorMask;
  UCHAR Color;
  UCHAR Seed;
  UCHAR NodeNumber;
  UCHAR Flags;
  ULONG MmShiftedColor;
  ULONG FreeCount[2];
  PSINGLE_LIST_ENTRY PfnDeferredList;
  CACHED_KSTACK_LIST CachedKernelStacks;
} KNODE, * PKNODE;

typedef struct _PP_LOOKASIDE_LIST
{
  PGENERAL_LOOKASIDE P;
  PGENERAL_LOOKASIDE L;
} PP_LOOKASIDE_LIST, * PPP_LOOKASIDE_LIST;

typedef struct _KDPC_DATA
{
  LIST_ENTRY DpcListHead;
  ULONG DpcLock;
  LONG DpcQueueDepth;
  ULONG DpcCount;
} KDPC_DATA, * PKDPC_DATA;

typedef struct _FX_SAVE_AREA
{
  BYTE U[520];
  ULONG NpxSavedCpu;
  ULONG Cr0NpxState;
} FX_SAVE_AREA, * PFX_SAVE_AREA;

typedef struct _PPM_IDLE_STATE
{
  LONG* IdleHandler;
  ULONG Context;
  ULONG Latency;
  ULONG Power;
  ULONG TimeCheck;
  ULONG StateFlags;
  UCHAR PromotePercent;
  UCHAR DemotePercent;
  UCHAR PromotePercentBase;
  UCHAR DemotePercentBase;
  UCHAR StateType;
} PPM_IDLE_STATE, * PPPM_IDLE_STATE;

typedef struct _PPM_IDLE_STATES
{
  ULONG Type;
  ULONG Count;
  ULONG Flags;
  ULONG TargetState;
  ULONG ActualState;
  ULONG OldState;
  ULONG TargetProcessors;
  PPM_IDLE_STATE State[1];
} PPM_IDLE_STATES, * PPPM_IDLE_STATES;

typedef struct _PROCESSOR_IDLE_TIMES
{
  UINT64 StartTime;
  UINT64 EndTime;
  ULONG Reserved[4];
} PROCESSOR_IDLE_TIMES, * PPROCESSOR_IDLE_TIMES;

typedef struct _PPM_IDLE_STATE_ACCOUNTING
{
  ULONG IdleTransitions;
  ULONG FailedTransitions;
  ULONG InvalidBucketIndex;
  UINT64 TotalTime;
  ULONG IdleTimeBuckets[6];
} PPM_IDLE_STATE_ACCOUNTING, * PPPM_IDLE_STATE_ACCOUNTING;

typedef struct _PPM_IDLE_ACCOUNTING
{
  ULONG StateCount;
  ULONG TotalTransitions;
  ULONG ResetCount;
  UINT64 StartTime;
  PPM_IDLE_STATE_ACCOUNTING State[1];
} PPM_IDLE_ACCOUNTING, * PPPM_IDLE_ACCOUNTING;

typedef struct _PPM_PERF_STATE
{
  ULONG Frequency;
  ULONG Power;
  UCHAR PercentFrequency;
  UCHAR IncreaseLevel;
  UCHAR DecreaseLevel;
  UCHAR Type;
  UINT64 Control;
  UINT64 Status;
  ULONG TotalHitCount;
  ULONG DesiredCount;
} PPM_PERF_STATE, * PPPM_PERF_STATE;

typedef struct _PPM_PERF_STATES
{
  ULONG Count;
  ULONG MaxFrequency;
  ULONG MaxPerfState;
  ULONG MinPerfState;
  ULONG LowestPState;
  ULONG IncreaseTime;
  ULONG DecreaseTime;
  UCHAR BusyAdjThreshold;
  UCHAR Reserved;
  UCHAR ThrottleStatesOnly;
  UCHAR PolicyType;
  ULONG TimerInterval;
  ULONG Flags;
  ULONG TargetProcessors;
  LONG* PStateHandler;
  ULONG PStateContext;
  LONG* TStateHandler;
  ULONG TStateContext;
  PVOID FeedbackHandler;
  PPM_PERF_STATE State[1];
} PPM_PERF_STATES, * PPPM_PERF_STATES;

typedef struct _PROCESSOR_POWER_STATE
{
  PVOID IdleFunction;
  PPPM_IDLE_STATES IdleStates;
  UINT64 LastTimeCheck;
  UINT64 LastIdleTime;
  PROCESSOR_IDLE_TIMES IdleTimes;
  PPPM_IDLE_ACCOUNTING IdleAccounting;
  PPPM_PERF_STATES PerfStates;
  ULONG LastKernelUserTime;
  ULONG LastIdleThreadKTime;
  UINT64 LastGlobalTimeHv;
  UINT64 LastProcessorTimeHv;
  UCHAR ThermalConstraint;
  UCHAR LastBusyPercentage;
  BYTE Flags[6];
  KTIMER PerfTimer;
  KDPC PerfDpc;
  ULONG LastSysTime;
  PVOID PStateMaster;
  ULONG PStateSet;
  ULONG CurrentPState;
  ULONG Reserved0;
  ULONG DesiredPState;
  ULONG Reserved1;
  ULONG PStateIdleStartTime;
  ULONG PStateIdleTime;
  ULONG LastPStateIdleTime;
  ULONG PStateStartTime;
  ULONG WmiDispatchPtr;
  LONG WmiInterfaceEnabled;
} PROCESSOR_POWER_STATE, * PPROCESSOR_POWER_STATE;

typedef struct _KPRCB
{
  WORD MinorVersion;
  WORD MajorVersion;
  PKTHREAD CurrentThread;
  PKTHREAD NextThread;
  PKTHREAD IdleThread;
  UCHAR Number;
  UCHAR NestingLevel;
  WORD BuildType;
  ULONG SetMember;
  CHAR CpuType;
  CHAR CpuID;
  union
  {
    WORD CpuStep;
    struct
    {
      UCHAR CpuStepping;
      UCHAR CpuModel;
    };
  };
  KPROCESSOR_STATE ProcessorState;
  ULONG KernelReserved[16];
  ULONG HalReserved[16];
  ULONG CFlushSize;
  UCHAR PrcbPad0[88];
  KSPIN_LOCK_QUEUE LockQueue[33];
  PKTHREAD NpxThread;
  ULONG InterruptCount;
  ULONG KernelTime;
  ULONG UserTime;
  ULONG DpcTime;
  ULONG DpcTimeCount;
  ULONG InterruptTime;
  ULONG AdjustDpcThreshold;
  ULONG PageColor;
  UCHAR SkipTick;
  UCHAR DebuggerSavedIRQL;
  UCHAR NodeColor;
  UCHAR PollSlot;
  ULONG NodeShiftedColor;
  PKNODE ParentNode;
  ULONG MultiThreadProcessorSet;
  PVOID MultiThreadSetMaster;
  ULONG SecondaryColorMask;
  ULONG DpcTimeLimit;
  ULONG CcFastReadNoWait;
  ULONG CcFastReadWait;
  ULONG CcFastReadNotPossible;
  ULONG CcCopyReadNoWait;
  ULONG CcCopyReadWait;
  ULONG CcCopyReadNoWaitMiss;
  LONG MmSpinLockOrdering;
  LONG IoReadOperationCount;
  LONG IoWriteOperationCount;
  LONG IoOtherOperationCount;
  LARGE_INTEGER IoReadTransferCount;
  LARGE_INTEGER IoWriteTransferCount;
  LARGE_INTEGER IoOtherTransferCount;
  ULONG CcFastMdlReadNoWait;
  ULONG CcFastMdlReadWait;
  ULONG CcFastMdlReadNotPossible;
  ULONG CcMapDataNoWait;
  ULONG CcMapDataWait;
  ULONG CcPinMappedDataCount;
  ULONG CcPinReadNoWait;
  ULONG CcPinReadWait;
  ULONG CcMdlReadNoWait;
  ULONG CcMdlReadWait;
  ULONG CcLazyWriteHotSpots;
  ULONG CcLazyWriteIos;
  ULONG CcLazyWritePages;
  ULONG CcDataFlushes;
  ULONG CcDataPages;
  ULONG CcLostDelayedWrites;
  ULONG CcFastReadResourceMiss;
  ULONG CcCopyReadWaitMiss;
  ULONG CcFastMdlReadResourceMiss;
  ULONG CcMapDataNoWaitMiss;
  ULONG CcMapDataWaitMiss;
  ULONG CcPinReadNoWaitMiss;
  ULONG CcPinReadWaitMiss;
  ULONG CcMdlReadNoWaitMiss;
  ULONG CcMdlReadWaitMiss;
  ULONG CcReadAheadIos;
  ULONG KeAlignmentFixupCount;
  ULONG KeExceptionDispatchCount;
  ULONG KeSystemCalls;
  ULONG PrcbPad1[3];
  PP_LOOKASIDE_LIST PPLookasideList[16];
  GENERAL_LOOKASIDE_POOL PPNPagedLookasideList[32];
  GENERAL_LOOKASIDE_POOL PPPagedLookasideList[32];
  ULONG PacketBarrier;
  LONG ReverseStall;
  PVOID IpiFrame;
  UCHAR PrcbPad2[52];
  VOID* CurrentPacket[3];
  ULONG TargetSet;
  PVOID WorkerRoutine;
  ULONG IpiFrozen;
  UCHAR PrcbPad3[40];
  ULONG RequestSummary;
  PVOID SignalDone;
  UCHAR PrcbPad4[56];
  KDPC_DATA DpcData[2];
  PVOID DpcStack;
  LONG MaximumDpcQueueDepth;
  ULONG DpcRequestRate;
  ULONG MinimumDpcRate;
  UCHAR DpcInterruptRequested;
  UCHAR DpcThreadRequested;
  UCHAR DpcRoutineActive;
  UCHAR DpcThreadActive;
  ULONG PrcbLock;
  ULONG DpcLastCount;
  ULONG TimerHand;
  ULONG TimerRequest;
  PVOID PrcbPad41;
  KEVENT DpcEvent;
  UCHAR ThreadDpcEnable;
  UCHAR QuantumEnd;
  UCHAR PrcbPad50;
  UCHAR IdleSchedule;
  LONG DpcSetEventRequest;
  LONG Sleeping;
  ULONG PeriodicCount;
  ULONG PeriodicBias;
  UCHAR PrcbPad5[6];
  LONG TickOffset;
  KDPC CallDpc;
  LONG ClockKeepAlive;
  UCHAR ClockCheckSlot;
  UCHAR ClockPollCycle;
  UCHAR PrcbPad6[2];
  LONG DpcWatchdogPeriod;
  LONG DpcWatchdogCount;
  LONG ThreadWatchdogPeriod;
  LONG ThreadWatchdogCount;
  ULONG PrcbPad70[2];
  LIST_ENTRY WaitListHead;
  ULONG WaitLock;
  ULONG ReadySummary;
  ULONG QueueIndex;
  SINGLE_LIST_ENTRY DeferredReadyListHead;
  UINT64 StartCycles;
  UINT64 CycleTime;
  UINT64 PrcbPad71[3];
  LIST_ENTRY DispatcherReadyListHead[32];
  PVOID ChainedInterruptList;
  LONG LookasideIrpFloat;
  LONG MmPageFaultCount;
  LONG MmCopyOnWriteCount;
  LONG MmTransitionCount;
  LONG MmCacheTransitionCount;
  LONG MmDemandZeroCount;
  LONG MmPageReadCount;
  LONG MmPageReadIoCount;
  LONG MmCacheReadCount;
  LONG MmCacheIoCount;
  LONG MmDirtyPagesWriteCount;
  LONG MmDirtyWriteIoCount;
  LONG MmMappedPagesWriteCount;
  LONG MmMappedWriteIoCount;
  ULONG CachedCommit;
  ULONG CachedResidentAvailable;
  PVOID HyperPte;
  UCHAR CpuVendor;
  UCHAR PrcbPad9[3];
  UCHAR VendorString[13];
  UCHAR InitialApicId;
  UCHAR CoresPerPhysicalProcessor;
  UCHAR LogicalProcessorsPerPhysicalProcessor;
  ULONG MHz;
  ULONG FeatureBits;
  LARGE_INTEGER UpdateSignature;
  UINT64 IsrTime;
  UINT64 SpareField1;
  FX_SAVE_AREA NpxSaveArea;
  PROCESSOR_POWER_STATE PowerState;
  KDPC DpcWatchdogDpc;
  KTIMER DpcWatchdogTimer;
  PVOID WheaInfo;
  PVOID EtwSupport;
  SLIST_HEADER InterruptObjectPool;
  LARGE_INTEGER HypercallPagePhysical;
  PVOID HypercallPageVirtual;
  PVOID RateControl;
  CACHE_DESCRIPTOR Cache[5];
  ULONG CacheCount;
  ULONG CacheProcessorMask[5];
  UCHAR LogicalProcessorsPerCore;
  UCHAR PrcbPad8[3];
  ULONG PackageProcessorSet;
  ULONG CoreProcessorSet;
} KPRCB, * PKPRCB;

typedef struct _KTHREAD
{
  DISPATCHER_HEADER Header;
  UINT64 CycleTime;
  ULONG HighCycleTime;
  UINT64 QuantumTarget;
  PVOID InitialStack;
  PVOID StackLimit;
  PVOID KernelStack;
  ULONG ThreadLock;
  union
  {
    KAPC_STATE ApcState;
    UCHAR ApcStateFill[23];
  };
  CHAR Priority;
  WORD NextProcessor;
  WORD DeferredProcessor;
  ULONG ApcQueueLock;
  ULONG ContextSwitches;
  UCHAR State;
  UCHAR NpxState;
  UCHAR WaitIrql;
  CHAR WaitMode;
  LONG WaitStatus;
  union
  {
    PKWAIT_BLOCK WaitBlockList;
    PKGATE GateObject;
  };
  union
  {
    ULONG KernelStackResident : 1;
    ULONG ReadyTransition : 1;
    ULONG ProcessReadyQueue : 1;
    ULONG WaitNext : 1;
    ULONG SystemAffinityActive : 1;
    ULONG Alertable : 1;
    ULONG GdiFlushActive : 1;
    ULONG Reserved : 25;
    LONG MiscFlags;
  };
  UCHAR WaitReason;
  UCHAR SwapBusy;
  UCHAR Alerted[2];
  union
  {
    LIST_ENTRY WaitListEntry;
    SINGLE_LIST_ENTRY SwapListEntry;
  };
  PKQUEUE Queue;
  ULONG WaitTime;
  union
  {
    struct
    {
      SHORT KernelApcDisable;
      SHORT SpecialApcDisable;
    };
    ULONG CombinedApcDisable;
  };
  PVOID Teb;
  union
  {
    KTIMER Timer;
    UCHAR TimerFill[40];
  };
  union
  {
    ULONG AutoAlignment : 1;
    ULONG DisableBoost : 1;
    ULONG EtwStackTraceApc1Inserted : 1;
    ULONG EtwStackTraceApc2Inserted : 1;
    ULONG CycleChargePending : 1;
    ULONG CalloutActive : 1;
    ULONG ApcQueueable : 1;
    ULONG EnableStackSwap : 1;
    ULONG GuiThread : 1;
    ULONG ReservedFlags : 23;
    LONG ThreadFlags;
  };
  union
  {
    KWAIT_BLOCK WaitBlock[4];
    struct
    {
      UCHAR WaitBlockFill0[23];
      UCHAR IdealProcessor;
    };
    struct
    {
      UCHAR WaitBlockFill1[47];
      CHAR PreviousMode;
    };
    struct
    {
      UCHAR WaitBlockFill2[71];
      UCHAR ResourceIndex;
    };
    UCHAR WaitBlockFill3[95];
  };
  UCHAR LargeStack;
  LIST_ENTRY QueueListEntry;
  PKTRAP_FRAME TrapFrame;
  PVOID FirstArgument;
  union
  {
    PVOID CallbackStack;
    ULONG CallbackDepth;
  };
  PVOID ServiceTable;
  UCHAR ApcStateIndex;
  CHAR BasePriority;
  CHAR PriorityDecrement;
  UCHAR Preempted;
  UCHAR AdjustReason;
  CHAR AdjustIncrement;
  UCHAR Spare01;
  CHAR Saturation;
  ULONG SystemCallNumber;
  ULONG Spare02;
  ULONG UserAffinity;
  PKPROCESS Process;
  ULONG Affinity;
  PKAPC_STATE ApcStatePointer[2];
  union
  {
    KAPC_STATE SavedApcState;
    UCHAR SavedApcStateFill[23];
  };
  CHAR FreezeCount;
  CHAR SuspendCount;
  UCHAR UserIdealProcessor;
  UCHAR Spare03;
  UCHAR Iopl;
  PVOID Win32Thread;
  PVOID StackBase;
  union
  {
    KAPC SuspendApc;
    struct
    {
      UCHAR SuspendApcFill0[1];
      CHAR Spare04;
    };
    struct
    {
      UCHAR SuspendApcFill1[3];
      UCHAR QuantumReset;
    };
    struct
    {
      UCHAR SuspendApcFill2[4];
      ULONG KernelTime;
    };
    struct
    {
      UCHAR SuspendApcFill3[36];
      PKPRCB WaitPrcb;
    };
    struct
    {
      UCHAR SuspendApcFill4[40];
      PVOID LegoData;
    };
    UCHAR SuspendApcFill5[47];
  };
  UCHAR PowerState;
  ULONG UserTime;
  union
  {
    KSEMAPHORE SuspendSemaphore;
    UCHAR SuspendSemaphorefill[20];
  };
  ULONG SListFaultCount;
  LIST_ENTRY ThreadListEntry;
  LIST_ENTRY MutantListHead;
  PVOID SListFaultAddress;
  PVOID MdlForLockedTeb;
} KTHREAD, * PKTHREAD;

typedef struct _TERMINATION_PORT
{
  PVOID Next;
  PVOID Port;
} TERMINATION_PORT, * PTERMINATION_PORT;

typedef struct _PS_CLIENT_SECURITY_CONTEXT
{
  union
  {
    ULONG ImpersonationData;
    PVOID ImpersonationToken;
    ULONG ImpersonationLevel : 2;
    ULONG EffectiveOnly : 1;
  };
} PS_CLIENT_SECURITY_CONTEXT, * PPS_CLIENT_SECURITY_CONTEXT;

typedef struct _ETHREAD
{
  KTHREAD Tcb;
  LARGE_INTEGER CreateTime;
  union
  {
    LARGE_INTEGER ExitTime;
    LIST_ENTRY KeyedWaitChain;
  };
  union
  {
    LONG ExitStatus;
    PVOID OfsChain;
  };
  union
  {
    LIST_ENTRY PostBlockList;
    struct
    {
      PVOID ForwardLinkShadow;
      PVOID StartAddress;
    };
  };
  union
  {
    PTERMINATION_PORT TerminationPort;
    PETHREAD ReaperLink;
    PVOID KeyedWaitValue;
    PVOID Win32StartParameter;
  };
  ULONG ActiveTimerListLock;
  LIST_ENTRY ActiveTimerListHead;
  CLIENT_ID Cid;
  union
  {
    KSEMAPHORE KeyedWaitSemaphore;
    KSEMAPHORE AlpcWaitSemaphore;
  };
  PS_CLIENT_SECURITY_CONTEXT ClientSecurity;
  LIST_ENTRY IrpList;
  ULONG TopLevelIrp;
  PDEVICE_OBJECT DeviceToVerify;
  PVOID RateControlApc;
  PVOID Win32StartAddress;
  PVOID SparePtr0;
  LIST_ENTRY ThreadListEntry;
  EX_RUNDOWN_REF RundownProtect;
  EX_PUSH_LOCK ThreadLock;
  ULONG ReadClusterSize;
  LONG MmLockOrdering;
  ULONG CrossThreadFlags;
  ULONG Terminated : 1;
  ULONG ThreadInserted : 1;
  ULONG HideFromDebugger : 1;
  ULONG ActiveImpersonationInfo : 1;
  ULONG SystemThread : 1;
  ULONG HardErrorsAreDisabled : 1;
  ULONG BreakOnTermination : 1;
  ULONG SkipCreationMsg : 1;
  ULONG SkipTerminationMsg : 1;
  ULONG CopyTokenOnOpen : 1;
  ULONG ThreadIoPriority : 3;
  ULONG ThreadPagePriority : 3;
  ULONG RundownFail : 1;
  ULONG SameThreadPassiveFlags;
  ULONG ActiveExWorker : 1;
  ULONG ExWorkerCanWaitUser : 1;
  ULONG MemoryMaker : 1;
  ULONG ClonedThread : 1;
  ULONG KeyedEventInUse : 1;
  ULONG RateApcState : 2;
  ULONG SelfTerminate : 1;
  ULONG SameThreadApcFlags;
  ULONG Spare : 1;
  ULONG StartAddressInvalid : 1;
  ULONG EtwPageFaultCalloutActive : 1;
  ULONG OwnsProcessWorkingSetExclusive : 1;
  ULONG OwnsProcessWorkingSetShared : 1;
  ULONG OwnsSystemWorkingSetExclusive : 1;
  ULONG OwnsSystemWorkingSetShared : 1;
  ULONG OwnsSessionWorkingSetExclusive : 1;
  ULONG OwnsSessionWorkingSetShared : 1;
  ULONG OwnsProcessAddressSpaceExclusive : 1;
  ULONG OwnsProcessAddressSpaceShared : 1;
  ULONG SuppressSymbolLoad : 1;
  ULONG Prefetching : 1;
  ULONG OwnsDynamicMemoryShared : 1;
  ULONG OwnsChangeControlAreaExclusive : 1;
  ULONG OwnsChangeControlAreaShared : 1;
  ULONG PriorityRegionActive : 4;
  UCHAR CacheManagerActive;
  UCHAR DisablePageFaultClustering;
  UCHAR ActiveFaultCount;
  ULONG AlpcMessageId;
  union
  {
    PVOID AlpcMessage;
    ULONG AlpcReceiveAttributeSet;
  };
  LIST_ENTRY AlpcWaitListEntry;
  ULONG CacheManagerCount;
} ETHREAD;

///////////////////////////////////////////////////////////////
// Public API
///////////////////////////////////////////////////////////////

PVOID
PsGetProcessSectionBaseAddress(
  PEPROCESS Process);

#endif