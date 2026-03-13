# Introduction

Planck is a lightweight interpreted scripting language with a minimal instruction set, inspired by the RISC-V ISA.

Planck is originally created to act as a scripting medium for BASED (Basic Automated Shell for Embedded Devices), a userland for microcontrollers which I am working on, but is able to run in any environment with the Planck Virtual Machine.

Planck aims to be as lightweight as possible, while still being functional enough for small scripts. The minimalism of the language is originally intended for memory-efficient operations on the ESP32-C3 Supermini, which has 400KB of SRAM, and a 160Hz RISC-V CPU.

# Architecture

## Variables

Planck can have a maximum of 32 variables, all of which are objects with fields `id` and `val`. `id` is the unique identifier of the variable, and `val` is the value it holds. As of writing this, Planck only supports 32-bit integer variables. Planck variable names can not be more than 16 characters.

Variables do not need to be declared before use, and will be initialized when they are first used in an operation. By default, every variable is set to zero.

A special variable `zero` always holds the value `0` and can not be altered.

## String Literals

Though Planck does not support storage and manipulation of any data type except 32-bit integers, Planck does support printing of string literals. 

Since Planck handles string literals differently than other variables, strings don't need to be inside quotes.

## Instructions

All instructions in Planck have a general format: `OP DEST SRC1 SRC2`. This format is heavily inspired by the RISC-V ISA. Some instructions may omit `SRC2`.

Since every instruction has the same format, every operation in Planck is an operation from a source to a destination.

## Streams

Planck supports moving data to and from standard streams i.e. `stdout`, `stdin` and `stderr`. 

## Operations

Planck has the following operations (as of writing this):

`dest`, `src1`, `src2` are names of variables. `str` is a string literal. Contents inside `[]` are optional.

| Operation | Usage | Effect |
| --------- | ----- | ------ |
| add | add dest src1 src2 | dest = src1 + src2 |
| mul | mul dest src1 src2 | dest = src1 * src2 |
| div | div dest src1 src2 | dest = src1 / src2 |
| mov | mov dest src1 | contents of src1 moved into dest |
| movstr | movstr dest str [br] | string literal str moved to stream dest; adding br adds a line break |

Variables and string literals can be printed by moving them into `stdout` or `stderr`. 

Variables can be read by moving `stdin` into a variable.

# Usage

## Installing

Download the file `planck` from this repository. This is the Planck Virtual Machine/Interpreter.

## Compiling

To compile the Planck Virtual Machine from source:

1. Clone the repository
2. After moving into the directory, run `gcc -O3 main.c -o planck`
3. The compiled binary, `planck` will be in the repository folder.

## Running

To run a Planck program `prog.plk`:

`path/to/planckvm prog.plk`

## Examples

```
mov x 5
mov y 6
add z x y
mov stdout z

>> 11
```

```
movstr stdout Number?
mov n_apples stdin
movstr stdout Number: br
mov stdout n_apples

>> Number?20
>> Number:
>> 20
```

```
movstr stdout Player1Score: br
mov p1_score stdin
movstr stdout Player2Score: br
mov p2_score stdin
add total p1_score p2_score
movstr stdout Total: br
mov stdout total

>> Player1Score:
>> 20
>> Player2Score:
>> 30
>> Total:
>> 50
```

# Planned Features and Limitations

Planck is in very early stages as of writing this, so naturally, it has a lot of limitations:

1. The biggest is probably the absence of conditionals and looping constructs. This prevents Planck from being Turing complete. Implementing this would take some time, since a labelling system would have to be added first.

2. String literals cannot have spaces in between.

3. Only supports 32-bit integer variables.

4. The memory constraints of the language are a limitation in some sense, but are also necessary for efficient working on microcontrollers (which Planck is originally intended for).

5. Potential bugs in the VM.





