# Real-Time ARFI Image Visualization using VTK

## Setup
1. [Install CMake and build the VTK Libraries.](http://www.vtk.org/Wiki/VTK/Configure_and_Build)

2. Create a new directory named `build` and change to that directory.

    `mkdir build && cd build`

3. Type `cmake ..`

4. Build the project.

    `make`

5. Test that the script works by inputting image names through standard input (stdin).

    `cd ..`
    
    `ls -d images/JPG/dog-* | ./build/ARFIVisualization `