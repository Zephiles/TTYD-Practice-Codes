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
To build this, you need to have **devkitPPC** and **Python 3** installed. Instructions for installing **devkitPPC** can be found [here](https://devkitpro.org/wiki/Getting_Started), and **Python 3** can be downloaded from [here](https://www.python.org/downloads/). You also need to install **pyelf2rel** in Python, which can be found [here](https://pypi.org/project/pyelf2rel/).

## Building
To build, you must first add **DEVKITPPC** to your environment, in which it should be set to the `devkitPPC` folder. You can then run `make` to build the GCI files. Any combination of rules can be applied to this. You should also run `make clean` after making any changes, as files built with a previous build could cause issues.  

Examples of `make` being used:  
`make us`  
`make jp eu`  
`make us eu`  

## Special Thanks
* **PistonMiner** for helping me with many C++ questions, and overall providing a ton of in-depth technical assistance.
* **Jdaster64** for helping me with various formulas, providing logic about guard/superguard timing, and other C++ related help.
* **Muz** for drawing the banners and icons used in the save files.
* **Gibstack** for testing out the menu, and for confirming many battle actor names.
