#!/bin/bash

GAME_CMDLINE="java" # TODO: Find better match
LIB_PATH="$(pwd)/build/libmchook.so"
LOAD_MODE=1 # RTLD_LAZY

if [ $EUID -ne 0 ]; then
	echo "[!] Please run as root"
	exit -1
fi

if [ ! -f $LIB_PATH ]; then
	echo "[!] Library does not exist"
	exit -1
fi

if [ "$1" != "-d" ]; then
	echo "[INJECT MODE]"
	gdb -n -q -batch \
		-ex "attach $(pgrep -f "$GAME_CMDLINE")" \
		-ex "set \$dlopen = (void *(*)(char *, int))dlopen" \
		-ex "call \$dlopen(\"$LIB_PATH\", $LOAD_MODE)" \
		-ex "detach" \
		-ex "quit"
else
	echo "[DEBUG MODE]"
	gdb -n -q \
		-ex "attach $(pgrep -f "$GAME_CMDLINE")" \
		-ex "set \$dlopen = (void *(*)(char *, int))dlopen" \
		-ex "call \$dlopen(\"$LIB_PATH\", $LOAD_MODE)"
fi
