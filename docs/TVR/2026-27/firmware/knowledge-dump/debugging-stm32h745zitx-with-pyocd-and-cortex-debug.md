# Debugging STM32H734ZITx with pyOCD and Cortex-Debug

## Audience

If you are trying to find an existing flash configuration, this document is not meant for you.

If you are creating your own flash configuration, you may find this helpful.

However, this is mainly just a knowledge dump and debugging story for those that are interested.

## Overview

Hardware

 - Board: `FC Rev 2.0`
 - MCU: `STM32H745ZITx`
 - Debug probe: `STLINK-V3MINIE `
 - GDB server: `pyOCD 0.45.1`
 - GDB frontend: `Cortex-Debug 1.13.0-pre10`

The `STM32H745ZITx` MCU we use on the FC is composed of 2 asymmetrical cores (asymmetrical as in being not the same), combining a Cortex-M7 and a Cortex-M4 core. Now, this configuration does introduce some issues in terms of the FW. For shared resources like system and peripheral clock configuration, we don't want both cores to try to configure it at the same time (e.g. data races may happen, double initialization may not be allowed). Taking a look at the default CubeMX generated firmware, it solves this by introducing a `DUAL_CORE_BOOT_SYNC_SEQUENCE` define that enables a synchronization step between the CM7 and CM4 during boot. It treats the CM7 as a primary core, handling configuration of shared resources, and the CM4 as a secondary core, waiting until the shared resource configuration is done.

Since we are dealing with concurrent code, we can describe the overall synchronization steps as the following possibilities.

| **CM7 boots first**                                                   | **CM4 boots first**                                                   |
| --------------------------------------------------------------------- | --------------------------------------------------------------------- |
| CM7 enables HSEM notifications                                        | CM4 enables HSEM notifications                                        |
| CM7 waits until CM4 enters STOP mode                                  | CM4 enters STOP mode                                                  |
| CM4 boots                                                             | CM7 boots                                                             |
| CM4 enters STOP mode                                                  | CM7 waits for CM4 to enter STOP mode and exit immediately since it is |
| CM7 configures shared resources (HAL, system clock, peripheral clock) | CM7 configures shared resources (HAL, system clock, peripheral clock) |
| CM7 takes and releases HSEM to notify CM4 that it can wake up         | CM7 takes and releases HSEM to notify CM4 that it can wake up         |
| CM7 waits until CM4 wakes up                                          | CM7 waits until CM4 wakes up                                          |
| User firmware runs as normal                                          | User firmware runs as normal                                          |

It appears that this synchronization process is straightforward, however, there are 2 important things to note:

1. CM7 waiting for CM4 has a timeout
2. All of D2 is inaccessible after entering STOP mode

So, there are a few problems that arise when debugging firmware on the MCU

- Our debugger start CM7 first and CM4 second
	- If starting the CM4 takes too long, CM7 will timeout and go into error handler
- A previous launch caused CM4 to be stuck in STOP mode
	- Debugger cannot interact with CM4 while in STOP mode (cannot halt, cannot continue)

The correct sequence of steps to debug would be to

1. Debug one of our cores as the primary target, and attach to the other core later
	1. We would likely prefer CM7 as the primary target, since it what ST regards as the primary core
2. Hold the device under reset
	1. To fix the STOP mode issue
3. Perform default debugger configuration
	1. Configuration that is not done by us
4. Flash the firmware
5. Continue the CM4 core
6. Continue the CM7 core
	1. Ensures the CM4 is in STOP mode

However, again, there are more problems that arise due to bugs, unimplemented features, and hardware limitations within our tech stack:

1. pyOCD always chooses the CM4 flash algorithm for CM7 when parsing CMSIS pack
2. pyOCD core discovery doesn't find CM7 when device is held under reset
3. pyOCD `monitor reset halt` only places a reset catch on the selected core

## pyOCD flash algorithm choice

So there are several ways of flashing firmware into the MCU flash. One way would be to use the memory mapped flash peripheral control registers to write data externally from the debug probe itself (debug probe could write data into those control register). The way that pyOCD uses it much simpler in terms of implementation.

Open-CMSIS-Pack is a standard for Arm Cortex MCUs that allows vendors to provide metadata about their MCUs in the form of a Device Family Pack (DFP). The DFP defines a bunch of useful information like what memory regions there are, debug sequences, and flash algorithms that a debugging tool can use. This flash algorithm is delivered in the form of FLM files, which are normal ELF executables that have a standard structure and ABI. What a debug tool like pyOCD would do is load that FLM file into the memory of the MCU, and execute the code in memory. It can then use that running program to erase and program the flash. Part of the description of the flash algorithm metadata is the flash range it supports writing to as well as a memory location for where the FLM must be loaded. For a 2 core MCU with 2 flash banks, that of course means 2 flash algorithm. For our MCU it is defined in the DFP as

```xml
<algorithm Pname="CM7" name="CMSIS/Flash/STM32H7x_2048.FLM" start="0x08000000" size="0x00200000" RAMstart="0x20000000" RAMsize="0x8000" default="1" />
<algorithm Pname="CM4" name="CMSIS/Flash/STM32H7x_2048.FLM" start="0x08000000" size="0x00200000" RAMstart="0x10000000" RAMsize="0x8000" default="1" />
```

Problem is, when pyOCD extracts the flash algorithms out of the CMSIS pack, it only cares about the flash region that it can write to. Since both algorithms for our MCU can write to the same flash start and size, pyOCD discards the algorithm for CM7 and only uses the CM4 one. Issue with this is that the RAM region (start and size) for both algorithms are different. So even when we are flashing from CM7, it uses the RAM region for CM4. If you look at RM0399 for where the CM4 RAM region is, it is in SRAM1. SRAM1 is located in D2. If you remember the synchronization sequence during boot, there is a time where we place D2 in STOP mode. So if D2 is stuck in STOP mode (for example if someone broke the synchronization code) and we try to flash on a new firmware, then CM7 attempts to execute code off of a peripheral in STOP mode which causes a fault, meaning that we have broken flashing.

To solve this, it is fairly simple. Either we upstream a change to pyOCD to distinguish between flash algorithms of different RAM regions and select that based on a core (which would be a nice open source contribution that we should do in the future). Or, as a temporary fix, we can use the user scripts functionality of pyOCD to patch the flash algorithm RAM region to RAM that stays powered along with CM7, say AXI SRAM in D1.

## CM7 core discovery failing when held under reset

Not sure whether the cause of this issue is with pyOCD or the actual MCU. But, we can see that when we set the connection mode to be under reset (that is we first reset the MCU, then hold it in reset during initialization), pyOCD fails to discover the CM7 core.

In the pyOCD logs, you may see

```
0000957 D Running debug sequence 'DebugCoreStart' (CM7) [pack_target]
0000959 E Error attempting to create component SCS: Memory transfer fault (read) @ 0xe000ed78-0xe000ed7b [discovery]
0000963 D Creating SCS component [discovery]
0000964 D Running debug sequence 'DebugCoreStart' (CM4) [pack_target]
0000966 I CPU core #1: Cortex-M4 r0p1, v7.0-M architecture [cortex_m]
0000966 I   Extensions: [DSP, FPU, FPU_V4] [cortex_m]
0000967 I   FPU present: FPv4-SP-D16-M [cortex_m]
```

which shows that we only discovered the CM4 core, and a fault occured when we tried to start the CM7.

Still unsure about the cause, but maybe its due to the fact that holding the MCU under reset doesn't clock the CM7 for memory reads? But that doesn't explain why CM4 is discovered fine. Maybe pyOCD queries for certain values from the current debug target, so it doesn't do anything to CM4?

Ignoring speculations about the cause, a simple fix would be to ensure the MCU is not held under reset just before the discovery task.

Some pseudocode for this may be:

```python
def fix_cm7_discovery():
	assert is_held_under_reset()
	
	set_reset_catch(cm7)
	set_reset_catch(cm4)
	
	release_reset()
	
	wait_until_halt(cm7)
	wait_until_halt(cm4)
	
	clear_reset_catch(cm7)
	clear_reset_catch(cm4)
```

We must ensure that both cores are at least halted so that the firmware doesn't change the current state of the MCU, such as suddenly entering STOP mode on one of the domains.

## Reset catch only halting one core after reset

For the definitive source of information, visit the Cortex-Debug wiki.

As a brief overview, the final launch steps for our debug setup is to perform the following commands

```
monitor reset halt
load
monitor reset halt
```

It reset the MCU and halts it on the reset vector, flashes on our firmware, and then does another reset and halt to ensure that the registers are not clobbered during the flashing. Problem with this is that the `monitor reset halt` commands are really only intended for a 1 core MCU setup. In pyOCD, it will set the vector catch for the reset vector and halt the **currently selected core** which is the CM7 for us, since it is the core that handles the launch. Given this information, you may see that we do nothing really to the CM4, so after every reset, the CM4 is allowed to run freely. However, this may cause issues if we want to keep D2 available or break early in the MCU, since the firmware would likely be able to run after each reset halt.

So, a solution would be to use the `preLaunchCommands` configuration provided by Cortex-Debug. In there we can simply do

```
monitor core 1
monitor halt
monitor set vector-catch hr
monitor core 0
```

to select CM4, halt it, place a reset catch, and swap the context back to CM7 for the rest of the launch sequence.

In the `postLaunchCommands`, we can restore the reset catch option

```
monitor core 1
monitor set vector-catch h
monitor core 0
```
