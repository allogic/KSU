# Overview

The `Kernel Script Utility (KSU)` is an x64 Kernel-Mode rootkit written in C. It was designed to disect and intercept process memory flow.

# Compile the Project

Make sure the latest `WDK` is installed on your system in order to build the driver. Open the Visual Studio Solution and build for `Debug` or `Release` bitness `x64`.

# Install the Driver

There are a few ways to start this driver. Most games require `Driver Signature Enforcement (DSE)` to be enabled but windows doesn't start drivers without certificates. The solution for this is to utilize tools like `KDU` which will bypass `DSE` at runtime or `EfiGuard` which will bypass `DSE` at boot time or runtime.

 - https://github.com/hfiref0x/KDU
 - https://github.com/Mattiwatti/EfiGuard

An alternative way to disable `DSE` is holding `Shift` and reboot. Once windows is booting up again click `Troubleshoot/Advanced Options/Startup Settings/Restart/F7`. Almost every anti-cheat software doesn't start since `DSE` has been disabled permanently in this mode till the next reboot.

After `DSE` has been disabled, the driver can be started.

```ps1
sc.exe create ksu type=kernel binPath="C:\driver.sys" # Create system service
sc.exe start ksu                                      # Start the driver
```

# Features

## Information

**This API is still under construction!**

This endpoint was designed to get extended information of kernel or process images.

```
interface [Ip(Str)] [Port(Dec)] info process [ProcessId(Dec)]
```

## Memory Scanning

**This API is still under construction!**

Physically, the memory of each process may be dispersed across different areas of physical memory, or may have been paged out to secondary storage, typically to a hard-disk drive or solid-state drive. The scanners job is to iterate all physical pages of a processes memory and store all values that match a certain criteria.

```
interface [Ip(Str)] [Port(Dec)] scan reset
interface [Ip(Str)] [Port(Dec)] scan aob [ProcessId(Dec)] [Bytes(Hex)]
interface [Ip(Str)] [Port(Dec)] scan changed
interface [Ip(Str)] [Port(Dec)] scan unchanged
interface [Ip(Str)] [Port(Dec)] scan undo
```

## Debug Breakpoints

**This API is still under construction!**

Hardware breakpoints are implemented using the DRx architectural breakpoint registers described in the Intel SDM.

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

# References

 - https://nirsoft.net/
 - https://geoffchappell.com/
 - https://reactos.org/
