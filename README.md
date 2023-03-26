# Kernel Script Utility

An x64 Kernel-Mode rootkit written in C.

# Compiling the Project

Open the VisualStudio solution and build for `Debug` or `Release` bitness `x64`.

# Installing the Driver

There are a few ways to start this driver. Most games require `Driver Signature Enforcement` to be enabled but windows doesn't start drivers without certificates. The solution for this is to utilize tools like `KDU` which will turn off `Driver Signature Enforcement` temporarily. KDU is available here: https://github.com/hfiref0x/KDU.

```
sc.exe create ksu type=kernel binPath="C:\driver.sys" // create system service (has to be done only once)
kdu.exe -dse 0                                        // disable DSE
sc.exe start/stop ksu                                 // start or stop driver
kdu.exe -dse 6                                        // enable DSE (some AC's require DSE to be enabled)
interface.exe 127.0.0.1 9095                          // issue a variety of commands
```

An alternative way is to disable `Driver Signature Enforcement` by holding `Shift` and reboot. Once windows is booting up again click `Troubleshoot/Advanced Options/Startup Settings/Restart`. Almost every anti-cheat software doesn't start since `Driver Signature Enforcement` has been disabled permanently till the next reboot.

# API

## Memory Scanning

This API is still under construction.

A process page table iterator.

```
interface [Ip(Str)] [Port(Dec)] scan reset
interface [Ip(Str)] [Port(Dec)] scan aob [ProcessId(Dec)] [Bytes(Hex)]
interface [Ip(Str)] [Port(Dec)] scan changed
interface [Ip(Str)] [Port(Dec)] scan unchanged
interface [Ip(Str)] [Port(Dec)] scan undo
```

## Debug Breakpoints

This API is still under construction.

```
interface [Ip(Str)] [Port(Dec)] break set [Address(Hex)]
interface [Ip(Str)] [Port(Dec)] break clear [Address(Hex)]
```

## Memory Operations

```
interface [Ip(Str)] [Port(Dec)] memory kernel read [Address(Hex)] [Size(Dec)]
interface [Ip(Str)] [Port(Dec)] memory kernel write [Address(Hex)] [Bytes(Hex)]
interface [Ip(Str)] [Port(Dec)] memory process [ProcessId(Dec)] read [Address(Hex)] [Size(Dec)]
interface [Ip(Str)] [Port(Dec)] memory process [ProcessId(Dec)] write [Address(Hex)] [Bytes(Hex)]
```

## Shutdown

```
interface [Ip(Str)] [Port(Dec)] shutdown
```

# Version Differences

```
Function        | Pattern                                                     | Version
----------------|-------------------------------------------------------------|-------------------
KeSuspendThread | A8 01 0F 85 ?? ?? ?? ?? 48 8B ?? E8 ?? ?? ?? ?? 89 44 24 ?? | 7601 - 22621.1413
KeResumeThread  | 48 8B ?? E8 ?? ?? ?? ?? 65 48 8B 14 25 88 01 00 00 8B       | 17763 - 22621.1413
```
