#ifndef KPROCESS_H
#define KPROCESS_H

#include "../config.h"

#if defined(OS_BUILD_NUMBER_22621_1413)

/*
0x000 Header           : _DISPATCHER_HEADER
0x018 ProfileListHead : _LIST_ENTRY[0xffffb00a`dfe9a098 - 0xffffb00a`dfe9a098]
0x028 DirectoryTableBase : 0x00000001`0416d000
0x030 ThreadListHead   : _LIST_ENTRY[0xffffb00a`dcdb6378 - 0xffffb00a`dffa8378]
0x040 ProcessLock      : 0
0x044 ProcessTimerDelay : 0
0x048 DeepFreezeStartTime : 0
0x050 Affinity : _KAFFINITY_EX
0x158 ReadyListHead : _LIST_ENTRY[0xffffb00a`dfe9a1d8 - 0xffffb00a`dfe9a1d8]
0x168 SwapListEntry    : _SINGLE_LIST_ENTRY
0x170 ActiveProcessors : _KAFFINITY_EX
0x278 AutoAlignment : 0y0
0x278 DisableBoost : 0y0
0x278 DisableQuantum : 0y0
0x278 DeepFreeze : 0y0
0x278 TimerVirtualization : 0y0
0x278 CheckStackExtents : 0y1
0x278 CacheIsolationEnabled : 0y0
0x278 PpmPolicy : 0y0111
0x278 VaSpaceDeleted : 0y0
0x278 MultiGroup : 0y0
0x278 ReservedFlags : 0y0000000000000000000(0)
0x278 ProcessFlags : 0n928
0x27c ActiveGroupsMask : 1
0x280 BasePriority : 8 ''
0x281 QuantumReset : 6 ''
0x282 Visited : 0 ''
0x283 Flags : _KEXECUTE_OPTIONS
0x284 ThreadSeed : [32] 0
0x2c4 IdealProcessor : [32] 0
0x304 IdealNode : [32] 0
0x344 IdealGlobalNode : 0
0x346 Spare1 : 0
0x348 StackCount : _KSTACK_COUNT
0x350 ProcessListEntry : _LIST_ENTRY[0xffffb00a`de278410 - 0xffffb00a`df4da3d0]
0x360 CycleTime        : 0x00000001`00c3c5f3
0x368 ContextSwitches  : 0x3295
0x370 SchedulingGroup : (null)
0x378 FreezeCount : 0
0x37c KernelTime : 6
0x380 UserTime : 7
0x384 ReadyTime : 0
0x388 UserDirectoryTableBase : 0
0x390 AddressPolicy : 0 ''
0x391 Spare2 : [71] ""
0x3d8 InstrumentationCallback : (null)
0x3e0 SecureState : <unnamed - tag>
0x3e8 KernelWaitTime : 9
0x3f0 UserWaitTime : 0x2a53f
0x3f8 LastRebalanceQpc : 0x00000002`7edacfa5
0x400 PerProcessorCycleTimes : 0x00000000`00008780 Void
0x408 ExtendedFeatureDisableMask : 0
0x410 PrimaryGroup : 0
0x412 Spare3 : [3] 0
0x418 UserCetLogging : (null)
0x420 CpuPartitionList : _LIST_ENTRY[0xffffb00a`dfe9a4a0 - 0xffffb00a`dfe9a4a0]
0x430 EndPadding       : [1] 0
*/

#elif defined(OS_BUILD_NUMBER_19044_2728)

// NO DATA AVAILABLE

#endif

#endif