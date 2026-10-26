# README

```Build Command

// TLDR: Lets you write cuda kernels and render it to the screen 

// On Windows Visual Studio Is Only Supported Environment for CUDA
// If you are using another environment make sure Visual Studio Enviromental Variables are Set if you use ninja
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64

// Build Debug
cmake --preset ninja-debug -DFETCHCONTENT_QUIET=OFF
cmake --build --preset ninja-debug --target SceneRayTracer

cmake --preset vs2022 -DFETCHCONTENT_QUIET=OFF
cmake --build --preset vs-debug --target SceneRayTracer

// Build Debug Color Test
cmake -S examples/debug-colors -B build/debug-colors
cmake --build build/debug-colors --config Release
.\build\debug-colors\Release\debug-colors.exe

// Notes on coding style, been writing more c99 and loving that method

// This is where I will show my test scenes