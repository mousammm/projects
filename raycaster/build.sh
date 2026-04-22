#!/bin/bash

set @

FILE="main.c"
BIN="raycaster.out"
WAR="-Wall -Wextra"
SDL2="`sdl2-config --cflags --libs`"

echo "compiling.."

# -lm to link math lib
gcc ${WAR} -o ${BIN} ${FILE} -lm ${SDL2}
echo "done!"

echo "running ./${BIN}.."
./${BIN}
