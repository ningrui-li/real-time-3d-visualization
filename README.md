# Real-Time ARFI Image Visualization using VTK

## Setup
1. [Install CMake and build the VTK Libraries.](http://www.vtk.org/Wiki/VTK/Configure_and_Build)

Some random notes on building this on Windows 10 with Qt 5.2.1 (OpenGL):

* The Qt version will default to 4; you need to manually change this to 5 (if you choose to use 5).

* `Qt5_DIR` had to be manuualy set to `C:/Qt/Qt5.2.1/5.2.1/msvc2012_64_opengl/lib/cmake/Qt5`

* Errors related to `glu32` not being found can be fixed by setting 
  `CMAKE_PREFIX PATH=C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib` 
  on my system.  This was non-obvious since the wiki page points you to a path in the Qt 
  install.
      
This build worked "out of the box" on Fedora 23 with the system-wide VTK developer tools installed.

2. Create a new directory named `build` and change to that directory.

    `mkdir build && cd build`

3. Type `cmake ..`

4. Build the project.

    `make`

5. Test that the script works by inputting image names through standard input (stdin).

    `cd ..`
    
    `ls -d images/JPG/dog-* | ./build/ARFIVisualization `
