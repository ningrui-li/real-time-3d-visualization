Real-Time ARFI Image Visualization using VTK
============================================

See [my lab meeting presentation slides](../presentations/2016_4_18_nl91_real_time_3d_visualization_prostate_imaging.pdf) for a general overview of the code structure.


Reading and Orienting Image Planes
----------------------------------
Before the image volume can be constructed, all of the image plane data must first be read into the program and oriented correctly relative to each other.

Image files names are read in through standard input (see `readImages()`) and the image data is imported using the `vtkJPEGReader` in `readAndOrientImagePlanes()`. This portion of the function should be re-written in the future so that image data is read directly from memory into the program, rather than first being written to disk.

Next, each image plane is rotated about the central axis to correctly align them with respect to each other. Each plane is also offset away from the central axis of rotation to account for the radius of the prostate probe. `vtkTransform` and `vtkTransformPolyDataFilter` classes are used to perform these transformations, and the code can be found in `readAndOrientImagePlanes()`. The rotation angles and amount of offset for each image are currently hard-coded as arbitrary values, but these can be easily changed when this visualization system is integrated into the scanner. One advantage of using the VTK libraries for visualization is that a variable amount of rotation can be applied to each image plane, and they can be still represented in the `vtkUnstructuredGrid` format.

Finally, all the correctly oriented image planes are combined into a single `vtkUnstructredGrid` object and passed to the `generateImageVolume()` function for interpolation onto a structured grid.


Interpolating onto an Image Volume
----------------------------------
The next step is to define a `vtkStructuredGrid`, which is a uniformly spaced grid of points. The values at each grid location will be interpolated from the `vtkUnstructuredGrid` constructed previously. This is advantageous because the structured grid is much easier and more efficient to work with compared to the unstructured grid data format. The `vtkStructuredGrid` bounds are set based on the bounds of the oriented image planes, and the resolution can be manually set by the user. This is done in the function `createStructuredGrid()`.

There are several ways to perform the interpolation. It is currently done using the `vtkProbeFilter`, but the `vtkPointInterpolator` or the `vtkFlyingEdgesPlaneCutter` might be faster options to consider in the future, according to Matt McCormick. More information on how to use the `vtkProbeFilter` can be found [here](http://www.vtk.org/Wiki/Demystifying_the_vtkProbeFilter). The interpolation is performed in the function `generateImageVolume()`. 



















