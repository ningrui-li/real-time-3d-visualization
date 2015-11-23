Notes from Matt about a few topics...

Real-time Processing
--------------------
Real-time imaging is one of the awesome and unique aspects of
ultrasound, but it brings some unique processing challenges.  To
ensure high throughput with a real-time response, an architecture
built around ring-buffers [1] can really help. Vanilla VTK does not
have support for this, but PLUS [2] added VTK-based support for a
processing pipeline where each stage of the pipeline, running in their
own process, populates a ring-buffer.  The subsequent stages of the
pipeline grab previous previous results without synchronization
overhead or glitches in the data.  ITK also has native support for
this [3].  I know that PLUS has only recently started to add support
for 3D data, though, and I do not know if this particular situation is
supported.  If you would like, I could set up a Hangout with the PLUS
folks to discuss further.

Although the data may be natively unstructured grid data, it still may
be a good idea to scan convert it to grid image data for rendering
with VTK.  The rendering performance may be better, and there will be
better options for volume rendering.

Docker Image or Native Compile?
-------------------------------
Cool!  I have been picking up development on our prostate work
recently, and I have started to create a Docker image to support VTK
rendering here [1] (work in progress). This is a great option for
testing, and it brings new opportunities for rendering remotely on
tablets, remote workstations, etc. because it is viewed in a browser
window.  For performance reasons, though, building for each native
platform may be required because it very hard to take advantage of the
native graphics hardware.

[1] https://en.wikipedia.org/wiki/Circular_buffer

[2] https://www.assembla.com/spaces/plus/wiki

[3] http://www.itk.org/Doxygen/html/classitk_1_1RingBuffer.html

[4] https://github.com/thewtex/docker-opengl
