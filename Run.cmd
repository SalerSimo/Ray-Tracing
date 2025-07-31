copy "external\lib\SDL3.dll" "run\SDL3.dll"
gcc src/*.c -Iinclude -lSDL3 -o run/RayTracing
.\run\RayTracing.exe