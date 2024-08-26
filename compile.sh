#!/bin/sh

set -e

export JAVA_HOME=/usr/lib/jvm/default-jvm

mkdir -p build

for src in $(cd external/jnihook/src && ls *.cpp); do
	cc -o "build/$src.o" -c -fPIC "external/jnihook/src/$src" -I external/jnihook/include -I "$JAVA_HOME/include" -I "$JAVA_HOME/include/linux"
done
cc -o build/main.o -c -fPIC src/main.cpp -I external/jnihook/include -I "$JAVA_HOME/include" -I "$JAVA_HOME/include/linux"
# c++ -o build/libmchook.so -shared -fPIC build/*.o -static-libgcc -static-libstdc++ -L "$JAVA_HOME/lib/server" -ljvm -Wl,-static

mold -o build/libmchook.so -shared -fPIC build/*.o -L /usr/lib -L "/usr/lib/gcc/x86_64-alpine-linux-musl/13.2.1" -L "$JAVA_HOME/lib/server" -Bstatic -lc -lgcc -lstdc++ -Bdynamic -ljvm
