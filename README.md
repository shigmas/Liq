# LiQ (Lidar Qt)
First pass at using the Slamtec LiDAR SDK to draw a representation of one scan.

## Dependences
* Qt 5.12.x
* [Slamtec SDK](https://github.com/Slamtec/rplidar_sdk)

## Build Instructions
1) Build the Slamtec SDK
1) Copy the resulting libs into the destination ($$OUT_PWD) libs directory
1) Copy the headers into the destination ($$OUT_PWD) include directory
1) Normal qmake stuff

## Caveats
This barely works. Selecting the USB port will cause one scan. Selecting it again will scan again. Or, maybe crash. YMMV. It'll get better.
