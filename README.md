# Kernel Script Utility

# Compiling the Project

Open the VisualStudio solution and build for `Debug` or `Release` bitness `x64`.

# Installing the Driver

There are a few ways to start this driver. Most games require `Driver Signature Enforcement` to be enabled but windows doesn't start drivers without certificates. The solution for this is to utilize tools like `KDU` which will turn off `Driver Signature Enforcement` temporarily. KDU is available here: https://github.com/hfiref0x/KDU.

```
sc.exe create ksu type=kernel binPath="C:\driver.sys" // create system service (has to be done only once)
kdu.exe -dse 0                                        // disable DSE
sc.exe start/stop ksu                                 // start or stop driver
kdu.exe -dse 6                                        // enable DSE (some AC's require DSE to be enabled)
interface.exe                                         // issue a variety of commands
```

An alternative way is to disable `Driver Signature Enforcement` by holding `Shift` and clicking reboot. Once windows is booting up again click `Troubleshoot/Advanced Options/Startup Settings/Restart`. Almost every anti-cheat software doesn't start since `Driver Signature Enforcement` has been disabled permanently till the next reboot.

# API

## Memory Scanning

This API is still under construction.

A process page table iterator.

```
interface scan reset
interface scan aob [ProcessId(Dec)] [Bytes(Hex)]
interface scan changed
interface scan unchanged
interface scan undo
```

## Debug Breakpoints

This API is still under construction.

```
interface break set [Address(Hex)]
interface break clear [Address(Hex)]
```

## Memory Operations

```
interface memory kernel read [Address(Hex)] [Size(Dec)]
interface memory kernel write [Address(Hex)] [Bytes(Hex)]
interface memory process [ProcessId(Dec)] read [Address(Hex)] [Size(Dec)]
interface memory process [ProcessId(Dec)] write [Address(Hex)] [Bytes(Hex)]
```

## Shutdown

```
interface shutdown
```
