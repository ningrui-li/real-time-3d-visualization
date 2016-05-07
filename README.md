Real-Time ARFI Image Visualization using VTK
============================================


Install CMake and build the VTK Libraries
-----------------------------------------

http://www.vtk.org/Wiki/VTK/Configure_and_Build

Some random notes on building this on Windows 10 with Qt 5.2.1 (OpenGL):

* The Qt version will default to 4; you need to manually change this to 5 (if you choose to use 5).

* `Qt5_DIR` had to be manually set to `C:/Qt/Qt5.2.1/5.2.1/msvc2012_64_opengl/lib/cmake/Qt5`

* Errors related to `glu32` not being found can be fixed by setting 
  `CMAKE_PREFIX_PATH=C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib` 
  on my system.  This was non-obvious since the wiki page points you to a path in the Qt 
  install.
      
This build worked "out of the box" on Fedora 23 with the system-wide VTK developer tools installed.

Compile Project
---------------
* Linux

  * Create a new directory named `build` and change to that directory.

    `mkdir build && cd build`

  * Type `cmake ..`

  * Build the project.

    `make`

* Visual Studio

  * Try to use the latest version of Visual Studio. I was able to build this on a laptop running Windows 10 using [Visual Studio Community 2015](https://www.visualstudio.com/en-us/visual-studio-homepage-vs.aspx).

  * Setup cmake using some of the config options noted above to build VTK.
  
  * Make sure to link to your build VTK directory.
  
  * Use Release and BUILD_ALL in VS.
  
  * Make sure that your PATH environmental variable points to your VTK DLLS (e.g., ``C:\VTK\bin\Release``)
  
  * If you are still having trouble getting to this work, here are [some more detailed instructions](http://www.vtk.org/Wiki/VTK/Building/Windows) for building on Windows. Note that they are probably using a different version of Visual Studio than you are, but the steps should still be about the same.

Running the ARFIVisualiation
----------------------------
Test that the script works by inputting image names through standard input (stdin).  For example:
    
    `ls -d images/JPG/dog-* | ./build/ARFIVisualization `
