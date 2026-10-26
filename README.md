# README

```Build Command

// On Windows Visual Studio Is Only Supported Environment for CUDA
// If you are using another environment make sure Visual Studio Enviromental Variables are Set if you use ninja
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64

// Build Debug
cmake --preset ninja-debug -DFETCHCONTENT_QUIET=OFF
cmake --build --preset ninja-debug --target SceneRayTracer

cmake --preset vs2022 -DFETCHCONTENT_QUIET=OFF
cmake --build --preset vs-debug --target SceneRayTracer

// Build Debug Color Test
cmake -S examples/debug-colors -B build/debug-colors-vs -G "Visual Studio 17 2022" -A x64
cmake --build build/debug-colors-vs --config Release --target debug-colors --parallel

.\build\debug-colors-vs\Release\debug-colors.exe

// Notes on coding style, been writing more c99 and loving that method
// Lets you write cuda kernels and render it to the screen 

// This is where I will show my test scenes
