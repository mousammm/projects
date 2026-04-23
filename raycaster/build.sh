#!/usr/bin/env bash
set @

mkdir -p bin

WAR="-Wall -Wextra -lm"
BIN="bin/raycaster"
SDL2="`sdl2-config --cflags --libs`"

echo "Compiling and runnning..."
gcc $WAR -o $BIN main.c $SDL2

echo "Installing to system.."
sudo cp $BIN /usr/local/bin/raycc

# ./$BIN
