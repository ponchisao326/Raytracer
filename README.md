# Raytracer

Raytracer in C++20 from scratch, no dependencies (the PNG/PPM writers are handwritten too).

## Build

```
cmake -S . -B build
cmake --build build
./build/raytracer
```

Output goes to `out/image.png` and `out/image.ppm`.

## Options

- `--size WxH` image size, default 640x360
- `--flat` no lighting, saves to `out/flat.png`
- `--trace X Y` prints every value computed for a single pixel

## Tests

```
ctest --test-dir build
```
