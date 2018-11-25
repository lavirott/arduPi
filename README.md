# arduPi
This projet merge all th known project from Cooking Hacks for the "Raspberry Pi to Arduino Shields Connection Bridge"

![Raspberry Pi to Arduino Shields Connection Bridge](https://www.cooking-hacks.com/media/catalog/product/cache/1/thumbnail/9df78eab33525d08d6e5fb8d27136e95/r/a/raspberry_arduino_shield_600px.1471337569.png)

The included release of the different packages are:
  - arduPi v2.4 (release for Raspberry Pi 2 and 3)
  - arduPi-api V1.8
  - 4G library and examples V1.3
  - CANbus library and examples V0.2
  - LoRa library and exemples V1.4
  - LoRaWAN library and examples V1.3
  - Sigfox library and examples V1.2
  - XBee examples V0.1

# Note
To activate the shiled's socket even if you don't use these examples, you need to activate GPIO 4 in OUT mode with the following commands:
```shell
sudo apt-get install wiringpi
gpio mode 4 OUT
gpio write 4 1
gpio readall
```
Then you can verify than GPIO 4 in configured as 1 and OUT.

# Usage
To compile and test the samples, you have to go to examples folder and in any subfolder, you just have:
```shell
$ ./cook.sh sample.cpp
$ sudo ./sample.cpp_exe
```
