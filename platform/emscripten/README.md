How to build:

https://emscripten.org/docs/getting_started/downloads.html

(make sure you run npm install in $ENV{EMSDK}/upstream/emscripten)

- `$EMSDK/emsdk_env.bat` (on Windows)
- `emcmake cmake . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release`
- `cmake --build ./build`

This will generate a .wasm file to be used as side module: https://emscripten.org/docs/compiling/Dynamic-Linking.html