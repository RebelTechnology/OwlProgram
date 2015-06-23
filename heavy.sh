#!/bin/bash

# make CONFIG=Release Build/heavy.elf && rm Build/HeavyProgram.o && make CONFIG=Debug Build/heavy.elf Build/heavy.syx
rm -f Build/startup.o
echo '.string "Heavy"' > Source/progname.s

make CONFIG=Release Build/heavy.elf
