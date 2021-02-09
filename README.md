# ConwaysLife
Conway's life game in 3d with gui for custom generating settings.


Screenshots:
![Image 1](res/screen1.png)


<h3>Compilation</h3>
Dependencies:

```bash
libboost
libglew
libglm
sdl2
sdl2-ttf
sdl2-image
sdl2-mixer
```

Compilation with cmake: <br>

```cmake
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. 
make -j<n>
```

