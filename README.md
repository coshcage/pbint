# Portable Big Integer Library

## Author

John Cage  cosh.cage#hotmail.com
Wick Dynex @github.com/wickdynex

## License

GPLv3.

## Introduction

Portable big integer library(aka: pbint) is intended to provide a set of functions to do arbitrary precision arithmetic jobs. This library is currently divided into 3 parts. These are kernel module, math module and external memory support module. Kernel module supports initializing functions for big integers and add, sub, mul, div functions for basic arithmetic operations. Big number is another type provided by this library. This type is used to conveniently convert big integers between different bases. Math module supports mathematical operations for big integers. External memory module supports to save and load big integers between main memory and files on disks.

## Before compiling checklist

|Name           |Type  |File  |Line number |Operation|
|---------------|------|------|------------|---------|
|BINT_INIT_SIZE |Macro |pbk.h |23          |Alter to a suitable value.                      |
|BINT_INIT_INCL |Macro |pbk.h |24          |Alter to a suitable value.                      |
|BNUM_INIT_SIZE |Macro |pbk.h |26          |Alter to a suitable value.                      |
|BNUM_INIT_INCL |Macro |pbk.h |27          |Alter to a suitable value.                      |
|_ib            |Type  |pbk.h |30          |Alter to signed platform integer.               |
|_ub            |Type  |pbk.h |31          |Alter to unsigned platform integer.             |
|_idb           |Type  |pbk.h |32          |Alter to signed double sized platform integer.  |
|_udb           |Type  |pbk.h |33          |Alter to unsigned double sized platform integer.|

## Compilation guide

Type cc *.c in command line.

