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
- `--frames N` writes raw RGB frames to stdout every N rows

Rendering to a video:

```
./build/raytracer --size 1920x1080 --frames 4 | ffmpeg -f rawvideo -pix_fmt rgb24 -s 1920x1080 -r 30 -i - -c:v libx264 -pix_fmt yuv420p out/render.mp4
```

## Tests

```
ctest --test-dir build
```
