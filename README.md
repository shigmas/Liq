# LiQ (Lidar Qt)
First pass at using the Slamtec LiDAR SDK to draw a representation of one scan.

## Dependences
* Qt 5.12.x
* [Slamtec SDK](https://github.com/Slamtec/rplidar_sdk)
* C++17 compiler, cmake, patch... probably other standard things
## Build Instructions
1) You'll need to clone recursively.
1) Apply the patch file to the submodule
1) create a build directory (anywhere)
1) CMAKE_PREFIX_PATH=<path to your Qt install> cmake <path to this directory>
1) cmake --build .

## Caveats
This barely works. Selecting the USB port will cause one scan. Selecting it again will scan again. Or, maybe crash. YMMV. It'll get better.
