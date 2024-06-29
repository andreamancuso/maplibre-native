How to build:

https://emscripten.org/docs/getting_started/downloads.html

(make sure you run npm install in $ENV{EMSDK}/upstream/emscripten)

- `cd platform/escripten`
- `cmake . -B build -GNinja`
- `cmake --build ./build --target mbgl-wasm`
