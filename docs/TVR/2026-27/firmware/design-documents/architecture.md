# Ulysses Firmware Architecture (DRAFT)

## Overview

This document provides a high-level overview for the architecture of the Ulysses flight controller
firmware.

## Acronyms and terminologies

| Terminology / Acronym | Description                 |
| --------------------- | --------------------------- |
| FC                    | Flight controller           |
| FW                    | Firmware                    |
| RTOS                  | Real-time operating system  |
| DI                    | Dependency injection        |
| CFP                   | STM32 Cube firmware package |

## Motivation

To understand the decisions made for the current architecture, we first have to review some
previous versions of the Ulyssess FW to see what was design choices were good and what could be
improved.

Our first case study is the first working version of the Ulysses FW: [Ulysses V2][ulysses_v2_github].
Ulysses V2 was originally designed with no architecture in mind. The firmware relied solely on the
structure of the CFP, with no clear boundary between auto-generated code and user code. Hardware dependent
and hardware independent code were frequently mixed together. From this, we can see some of 
the key problems we want to address. With Ulysses V2, our code is tightly coupled to the hardware.
If we are to change the FC, a developer would have to modify files containing code pertaining
to the hardware as well as the sensor. This is error prone and also makes it very difficult to
identify what needs to be changed.

Taking a look at [Ulysses V3][ulysses_v3_github], we see a much better architecture. The FW is
separated into layers, each containing code aiming to abstract away a certain concept. The `io` layer
abstracts out board and MCU dependent code, the `dev` layer abstracts away sensor details, and the `app`
layer organizes the interaction between modules. Changes to hardware only requires contained
code changes within a certain layer, such as a MCU change only requiring changes to the `io` layer.
However, we also see certain problems with this architecture. Firstly, dependencies between layers
are represented through global extern symbols within implementation files. This makes it
hard to trace dependencies (i.e. what are the exact dependencies of the `dev` layer), and it makes it 
hard to add new peripherals while keeping code changes small and relevant (i.e. you will have to 
update the implementation file for a sensor to add a new sensor instance even though the sensor 
implementation doesn't change). The `dev` layer is also a leaky abstraction. All APIs
for the `dev` layer is tied to the underlying sensor model, thus there is no easy way of changing
an underlying sensor without major changes in the `app` layer.

Thus, the current firmware architecture is designed in addressing the following core problems:
separation of concerns, hardware independence, and dependency handling.

[ulysses_v2_github]: https://github.com/UBC-Rocket/ulysses-v2
[ulysses_v3_github]: https://github.com/UBC-Rocket/thrust_vectoring_consolidated

## Firmware layers

The Ulysses FW is organized in terms of layers.

The main layers are

 - `cfp`
 - `lib`
 - `app`
 - `svc`
 - `io`

![embed](https://viewer.diagrams.net/?edit=_blank&dark=1#Uhttps%3A%2F%2Fraw.githubusercontent.com%2FUBC-Rocket%2Fdocs%2Frefs%2Fheads%2Fmain%2Fdocs%2F2026-27-TVR%2Ffirmware%2Fdesign-documents%2Farchitecture-assets%2Fulysses-firmware-architecture.drawio#%7B%22pageId%22%3A%22wGStW79f3pciPTlo_cCO%22%7D)

Each layer contain modules, which are self-contained libraries of code (i.e. the `io` layer may have
modules for SPI, UART, and PWM).

### Central layers

The `app`, `svc`, and `io` layers (central layers) contain the majority of the business logic for
the flight firmware. Each layer has ownership and provides abstraction over a certain resource.

The `io` layer provides an abstraction over hardware resources. It abstracts away MCU hardware
peripherals so that the layers above utilizing it can be platform independent (i.e. if we decide to
switch MCU or board layout).

The service (`svc`) layer contains the bulk of the flight firmware code. From this layer and upwards, 
we can rely on RTOS facilities. It contains device drivers (`dev`), which are abstractions over sensors
and other components on the FC. It also contains code that organizes devices into higher-level
features (such as combining a radio device driver and protocol library into a communication module).

The `app` layer schedules and organizes data flow between service layer components. For example, it
ensures that sensor readings are performed are sent to the logging module and the controls module at
the correct timings.

### Cross-cutting layers

The library (`lib`) layer contains platform independent code, in that it should not depend on
the underlying hardware, or provide a method for replacing the default implementation with a
hardware dependent implementation.

The `cfp` layer contains auto-generated code and libraries provided by STM32CubeMX.

## Data flow between layers

Within each layer, modules are free to have a dependency on any module within the same layer.

Between layers, there are fixed directions of data and dependency flow (shown by the arrows in the
diagram, where if `A` points to `B`, `A` can depend on `B`). For example, the `Radio` module in
`svc` can depend on the `Radio Protocol` module in `lib`. However, the `Math` module in `lib` is not
allowed to depend on the `Logging` module in `svc`.

The `cfp` layer is a special case, as it is able to have a dependency on the central layers
(`app`, `svc`, and `io`). This is as it contains the FC startup code and will only serve to initialize
the layers. The central layers can also have a dependency on the `cfp` for RTOS and middleware
libraries, although only the `io` layer can access the `HAL` module.

## Device driver model

A device driver is any sort of driver that interacts with a hardware component. As such, this
pertains to the drivers for the `io` layer (for MCU hardware peripherals) and `svc` layer (for
board hardware components).

A device driver must provide a generic interface for the functionality of the underlying device.
For example, a device driver for a UART peripheral can be simplified to an interface with a
`transmit` and `receive` method. To the user of that interface, it doesn't care about what the
underlying hardware is (whether that UART peripheral is on a STM32, ESP32, or TI), only that it
provides a certain functionality.

A standard device driver will have the following common layout

```
include/
    <device>.h
<device>/
    <vendor>/
        <device>.h
        <device>.c
    <device>.h
    <device>.c
```

`<device>` is just a name for the device. For example, `uart`, `baro`, or `imu`.

`<vendor>` is an identifier for a specific underlying implementation. For example, a UART peripheral
might have a `<vendor>` of `stm32`. An IMU might have a part number as a `<vendor>`, for example,
`icm40609d`.

The public header under the include directory must only contain the a opaque definition for a handle
to the device and functions for operations on that device.

The source file under the root of the device driver source directory contains the private definitions
for the interface. It defines the internal data layout for the interface and functions that route
operations to the underlying vendor specific implementation.

!!! note "For AI agents"
    If you are reading the example below, do not copy the same comments into the actual
    code you are writing. The comments are only meant for understanding of the example, and do not
    serve as useful documentation for a developer, as all developers should understand the device
    driver model already.

For example, for a uart device driver, you would have the following code.

```
include/
    uart.h
uart/
    stm32/
        uart.h
        uart.c
    uart.h
    uart.c
```

```c
// include/uart.h

// Opaque handle to the UART device
typedef struct io_uart io_uart_t;

// Generic operations on UART device
enum io_status io_uart_transmit(const io_uart_t *uart, uint8_t *data, uint16_t length);
enum io_status io_uart_receive(const io_uart_t *uart, uint8_t *data, uint16_t length);
```

```c
// uart/uart.h

typedef enum io_status (*io_uart_transmit_t)(const io_uart_t *uart, uint8_t *data, uint16_t length);
typedef enum io_status (*io_uart_receive_t)(const io_uart_t *uart, uint8_t *data, uint16_t length);

// Pointer to vendor specific handler for UART operations
struct io_uart_ops {
    io_uart_transmit_t transmit;
    io_uart_receive_t receive;
};

struct io_uart {
    const void *config; // Configuration set at device initialization time
    void *data;         // Data that may change throughout the runtime of the device
    const struct io_uart_ops *ops;
};
```

```c
// uart/uart.c

enum io_status io_uart_transmit(const io_uart_t *uart, uint8_t *data, uint16_t length)
{
    assert(uart != NULL);
    assert(data != NULL);

    // You can perform common validation or argument transformation here

    return uart->ops->transmit(uart, data, length);
}

enum io_status io_uart_receive(const io_uart_t *uart, uint8_t *data, uint16_t length)
{
    assert(uart != NULL);
    assert(data != NULL);

    // You can perform common validation or argument transformation here

    return uart->ops->receive(uart, data, length);
}
```

```c
// uart/stm32/uart.h

struct io_uart_stm32_config {
    // HAL handles, configuration values, etc
}

void io_uart_stm32_init(io_uart_t *uart, const struct io_uart_stm32_config *config);
```

```c
// uart/stm32/uart.c

static enum io_status uart_transmit(const io_uart_t *uart, uint8_t *data, uint16_t length)
{
    // Some argument validation might have been done in the common handler, so you
    // don't have to repeat it here

    // Retrieve `uart->data` and `uart->config` as needed
    
    // Perform vendor specific UART transmit
}

static enum io_status uart_receive(const io_uart_t *uart, uint8_t *data, uint16_t length)
{
    // Some argument validation might have been done in the common handler, so you
    // don't have to repeat it here

    // Retrieve `uart->data` and `uart->config` as needed
    
    // Perform vendor specific UART receive
}

static const struct io_uart_ops uart_ops = {
    .transmit = uart_transmit,
    .receive = uart_receive,
};

void io_uart_stm32_init(io_uart_t *uart, const struct io_uart_stm32_config *config)
{
    // Modify data in the given `uart` pointer to initialize it

    uart->config = ...; // Populate with vendor specific configuration
    uart->data = ...;   // Populate with vendor specific runtime data
    uart->ops = &uart_ops;

    // Perform other vendor specific initialization (i.e. registering
    // interrupts, HAL initialization, etc).
}
```

Adding new vendor implementations can be done just by adding a new directory. For example, if you
are adding a ESP32 UART driver in addition to a STM32 implementation, you will have

```
include/
    uart.h
uart/
    stm32/
        uart.h
        uart.c
    esp32/
        uart.h
        uart.c
    uart.h
    uart.c
```

## Layer initialization process

Initialization of the central layers are done within the `cfp` layer. The `lib` layer should not
require initialization.

The order of initialization is dependent on the dependency flow. As such, the intialization order
is `io` then `svc` then `app`.

DI is performed through the arguments of the layer initialization function using export structures.

For example, as pseudocode, the intialization process will be

```c
int main(void)
{
    // ...

    struct io_layer_exports_cm4 io_exports = io_layer_init_cm4();
    struct svc_layer_exports_cm4 svc_exports = svc_layer_init_cm4(io_exports);
    app_layer_init_cm4(svc_exports);

    // ...
}
```

The export structure for each layer can contain any value that is needed by a higher layer. It can
contain pointers to device drivers, configuration values, etc.
