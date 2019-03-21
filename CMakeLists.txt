cmake_minimum_required(VERSION 3.13)
project(x86emu)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(device)
include_directories(kazuminlib)

add_executable(x86emu
        device/Device.cpp
        device/Device.h
        device/Display.cpp
        device/Display.h
        device/PIC.cpp
        device/PIC.h
        kazuminlib/BMP.cpp
        kazuminlib/BMP.h
        kazuminlib/Image.cpp
        kazuminlib/Image.h
        kazuminlib/PPM.cpp
        kazuminlib/PPM.h
        Emulator.cpp
        Emulator.h
        GUI.cpp
        GUI.h
        Instruction16.cpp
        Instruction32.cpp
        main.cpp
        ModRM.cpp
        ModRM.h
        test03.c
        test04.c)
