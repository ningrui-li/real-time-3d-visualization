#pragma once
// Generally always include these
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkObjectFactory.h>
#include <vtkProperty.h>

// For reading in images
#include <vtkSmartPointer.h>
#include <vtkJPEGReader.h>

#include <vtkImageDataToPointSet.h>
#include <vtkImageDataGeometryFilter.h>

#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include <vtkUnstructuredGrid.h>
#include <vtkCleanPolyData.h>
#include <vtkAppendPolyData.h>
#include <vtkStructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>

#include <vtkAppendFilter.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkClipDataSet.h>
#include <vtkPolyDataMapper.h>
#include <vtkPoints.h>
#include <vtkStructuredGrid.h>
#include <vtkProbeFilter.h>
#include <vtkDelaunay3D.h>

#include <vtkPlaneSource.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkClipDataSet.h>
#include <vtkPlane.h>
#include <vtkSampleFunction.h>
#include <vtkContourFilter.h>
#include <vtkOutlineFilter.h>
#include <vtkPlaneWidget.h>

#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>

double center[3];
double* bounds;
double width;
double height;
double length;

// Define image plane widget
vtkSmartPointer<vtkPlaneWidget> sagittalClipPlaneWidget = vtkSmartPointer<vtkPlaneWidget>::New();
vtkSmartPointer<vtkPlaneWidget> axialClipPlaneWidget = vtkSmartPointer<vtkPlaneWidget>::New();
vtkSmartPointer<vtkPlaneWidget> coronalClipPlaneWidget = vtkSmartPointer<vtkPlaneWidget>::New();

bool updateSagittal = false;
bool updateAxial = false;
bool updateCoronal = false;

vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();

// Define clipping filter
vtkSmartPointer<vtkPlane> sagittalClipPlane = vtkSmartPointer<vtkPlane>::New();
vtkSmartPointer<vtkClipDataSet> sagittalClipDataSet = vtkSmartPointer<vtkClipDataSet>::New();

vtkSmartPointer<vtkPlane> axialClipPlane = vtkSmartPointer<vtkPlane>::New();
vtkSmartPointer<vtkClipDataSet> axialClipDataSet = vtkSmartPointer<vtkClipDataSet>::New();

vtkSmartPointer<vtkPlane> coronalClipPlane = vtkSmartPointer<vtkPlane>::New();
vtkSmartPointer<vtkClipDataSet> coronalClipDataSet = vtkSmartPointer<vtkClipDataSet>::New();


vtkSmartPointer<vtkCleanPolyData> readAndOrientImagePlanes();
vtkSmartPointer<vtkProbeFilter> generateImageVolume(vtkSmartPointer<vtkCleanPolyData> cleanFilter, int numPoints[3]);
std::vector<std::string> readImages();