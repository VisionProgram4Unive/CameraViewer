# CameraViewer
Camera Viewer program at Jetson Board

#### Requirement

- OpenCV 2.4 +
- QT 5.8 (recommend)
- C++14

#### Howto Compile

Change `INCLUDEPATH` and `LIBS` for your computer enviroments. (Line 40~)
or just use pkg-config and vc-pkg(if you can)

#### Directory list

1. MY
   A algorithm warp based OpenCV

2. IO
   v4l2capture : v4l2 library based device class.
   CameraController : for recognizing multi-platform cam device, implemented this class (DI pattern), see the comments in header files for detail..

3. 1
   thread template.(useless)
