# The Finals Cashout

## Overview
Inspired by the game [The Finals](https://www.reachthefinals.com) I made a cashout station in real life.

I made a 3d print of [that](https://sketchfab.com/3d-models/the-finals-cashout-and-vault-3d-model-cca6dad59c8d4516ae1370d526cc0a7a) model and painted it by myself. I also created a simple program to mimic the 'stealing'.

## Preview
You can see how it looks on Reddit: https://www.reddit.com/r/thefinals/comments/1fsuoct/i_made_a_cashout_station_in_real_life/

## Getting Started

### Download
Run the following command to clone repo and initialize all submodules: ```git clone --recursive https://github.com/Filipeak/the-finals-cashout```

If you already downloaded repo without ```--recursive``` flag, run: ```git submodule update --init --recursive```

### Environment
You have two ways to setup the project:

<ins>**1. Docker**</ins>

Build image: ```docker build -t the-finals-cashout .```

Run container: ```docker run --rm -it -v %cd%:/app the-finals-cashout```

<ins>**2. Manual**</ins>

Install required software:
1. CMake
2. ARM Toolchain
3. A build system (Ninja, etc...)
4. A compiler (GCC, Clang, MinGW, etc...)

### Build
Project is done in CMake, so create build directory and run: ```cmake ..``` in it. You can also specify your build system (e.g. Ninja) or set some variables. Then you can build your project: ```make``` (Linux) or for example ```ninja```.