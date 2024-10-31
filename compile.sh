#!/bin/bash

#cd lib/MLX42/

#cmake -B build 
#cmake --build build -j4

#cd - && cd lib/bass 

#make 64bit

#cd -

g++ main.cpp -o as ./lib/bass/libbass.dylib \
		-framework Cocoa -framework IOKit ./lib/MLX42/build/libmlx42.a \
		-Iinclude -lglfw -L /Users/beddinao/.brew/opt/glfw/lib #-fsanitize=address

install_name_tool -change @rpath/libbass.dylib $PWD/lib/bass/libbass.dylib ./as
