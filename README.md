# General
This is a catch-all repo for my projects. If any are particularly popular or I find them super cool, then I may move them to their own repo upon completion. All projects are licensed with the "Unlicense license" so feel free to modify/fork/whatever any of the projects to suit your needs :)

All projects will include the source code as well as a pre-compiled binary version that can be run (where applicable)
# Projects
## File share program
**Status**: incomplete

**Language**: C

**Description**: This is a simple program meant mainly to help me learn some network programming. It will be entirely self-contained with all dependencies that should be available on most *NIX machines (stdio,arpa/inet, etc...), and will provide a simple cli-based way to share files on a local network.

**Features**: 
- Server and client mode (COMPLETE)
- TCP connection to ensure reliable data transfer (COMPLETE)
- serialization so that any type of file can be transferred
- encryption/decryption when sending

**Limitations/Quirks**: 
- Must be used on a *NIX machine (feel free to fork and make it work on Windows/Mac though!)
- No directory traversal so the server must be run from the folder that has the file you want

## Read Phrack (rephr)
**Status**: incomplete

**Language**: zig

**Description** Cli-based tool to select/read/save phrack magazine (https://phrack.org/) articles

**Features**:
- Simple cli
- low memory footprint
- article saving
- vi-like navigation?
