#!/usr/bin/env bash

mkdir -p bin

cc -Wall -Wextra main.c -o bin/xtty
./bin/xtty
