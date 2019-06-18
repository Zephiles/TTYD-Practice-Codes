## Credits
* **PistonMiner** for their work on the TTYD scripting disassembly, the initial code for the rel framework, and the elf2rel, gcipack, and rellink utilities. (GitHub repo for these tools [here](https://github.com/PistonMiner/ttyd-tools).)
* **Jdaster64** for their work on the Shufflizer. (GitHub repo for the Shufflizer [here](https://github.com/jdaster64/shufflizer).)

## Overview
These are practice codes that I have made for Paper Mario: The Thousand-Year Door. 

## Download
The latest release can be found on the [releases page](https://github.com/Zephiles/TTYD-Practice-Codes/releases).

## Usage
See the [user manual](https://github.com/Zephiles/TTYD-Practice-Codes/blob/master/USER_MANUAL.md).

## Prerequisites
To build this, you need to have devkitPPC installed. Instructions for doing so can be found [here](https://devkitpro.org/wiki/Getting_Started). You also need to place **elf2rel.exe** in the **bin** folder. This file can either be compiled manually, or downloaded from [here](https://github.com/PistonMiner/ttyd-tools/releases/download/v2.5/elf2rel.exe).

## Building
To build, navigate to the root directory of the repository (the folder with the makefile in it) and run `make`. Any combination of rules can be applied to this.
`make us`
`make jp eu`
`make us eu`

## Special Thanks
* **PistonMiner** for helping me with many C++ questions, and overall providing a ton of in-depth technical assistance.
* **Jdaster64** for helping me with various formulas, providing logic about guard/superguard timing, and other C++ related help.
* **Muz** for drawing the banners and icons used in the save files.
* **Gibstack** for testing out the menu, and for confirming many battle actor names.