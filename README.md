# EzLogger
EzLogger is a lean and flexible C++ logger. Currently logging to console, VisualStudio debug window and file is supported.

## Features
EzLogger aims to provide full flexibility to the user, making the addition of custom output channels, severities, and verbosities easy. Through template-based compounding any number of loggers can be combined into a single unit.

## Build
EzLogger only consists of a single header and *.cpp file. The examples can be built separately.
The "Build" folder contains pre-generated VS2019 project files, but any build system can be used in theory.

## Dependencies
EzLogger has dependencies on WinAPI and VisualStudio if the VS_OUT define is set.

## Usage
Three macros (LOG_INFO, LOG_WARNING, and LOG_ERROR) and a single VS+Console+File compound logger are pre-defined, but new types can be defined easily. See the example files and the bottom of logger.h.


## Todo's
- More flexible macros
- Single header
- Test speed of atomic based ringbuffer vs mutex
