# evdev-calibration
Calibration application for evdev
It's tested on Raspberry Pi, it's not absolutely accurate, but it's works.
Rotated touch-screens are not supported yet

# Binaries

Check releases folder

# Installation

(not sure if this is required)
```
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
```

# Compiling

```
mkdir build
cd build
cmake ../
make
```

# TODO
- Rotated touch-screens
- Better calculation
- Automatically generate 90-touchscreen file
