# evdev-calibration
Calibration application for evdev
It's tested on Raspberry Pi, it's not absolutely accurate, but it's works.
Rotated touch-screens are not supported yet

# Installation

```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
cd /tmp
wget https://github.com/gamelaster/evdev-calibration/releases/download/0.2/evdev-calibration-armv7.tar.gz
tar xvf evdev-calibration-armv7.tar.gz
export DISPLAY=:0
./evdev-calibration
```
And then just follow the instructions

# Binaries

Check releases folder

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
