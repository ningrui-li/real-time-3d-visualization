#include <iostream>
#include <string>

// Generally always include these
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkProperty.h>

// For reading in images
#include <vtkSmartPointer.h>
#include <vtkJPEGReader.h>

#include <vtkImageDataGeometryFilter.h>

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include <vtkCleanPolyData.h>
#include <vtkAppendPolyData.h>

// Plane widget interactor
#include <vtkImagePlaneWidget.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>

// Define interaction style
class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
{
  public:
    static KeyPressInteractorStyle* New();
    vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);
 
    virtual void OnKeyPress() 
    {
      // Get the keypress
      vtkRenderWindowInteractor *rwi = this->Interactor;
      std::string key = rwi->GetKeySym();
 
      // Output the key that was pressed
      std::cout << "Pressed " << key << std::endl;
 
      // Handle an arrow key
      if(key == "Up")
        {
        std::cout << "The up arrow was pressed." << std::endl;
        }
 
      // Handle a "normal" key
      if(key == "a")
        {
        std::cout << "The a key was pressed." << std::endl;
        }
 
      // Forward events
      vtkInteractorStyleTrackballCamera::OnKeyPress();
    }
 
};
vtkStandardNewMacro(KeyPressInteractorStyle);

int main(int argc, char* argv[])
{
  // Verify input arguments
  /*
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0]
              << " IMAGE_ONE(.png) IMAGE_TWO(.png)" << std::endl;
    return EXIT_FAILURE;
  }
  */

  vtkSmartPointer<vtkJPEGReader> reader;
  vtkSmartPointer<vtkImageDataGeometryFilter> imageDataGeometryFilter;
  vtkSmartPointer<vtkTransform> rotationTransform;
  vtkSmartPointer<vtkTransformPolyDataFilter> rotationTransformFilter;
  vtkSmartPointer<vtkPolyData> myImageData;

  vtkSmartPointer<vtkAppendPolyData> appendFilter =
    vtkSmartPointer<vtkAppendPolyData>::New();

  double angle = 0;

  // Read in images and append them to a single PolyData
  for (std::string line; std::getline(std::cin, line);) {
    std::cout << line << std::endl;

	// Read in image data.
	reader = vtkSmartPointer<vtkJPEGReader>::New();
    reader->SetFileName(line.c_str());

	// Convert image data to PolyData
	imageDataGeometryFilter = vtkSmartPointer<vtkImageDataGeometryFilter>::New();
    imageDataGeometryFilter->SetInputConnection(reader->GetOutputPort());
    imageDataGeometryFilter->Update();

	// Rotate the image.
	//transform->RotateWXYZ(double angle, double x, double y, double z);
	rotationTransform = vtkSmartPointer<vtkTransform>::New();
	rotationTransform->RotateWXYZ(angle, 1, 0, 0);

	rotationTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	rotationTransformFilter->SetTransform(rotationTransform);
	rotationTransformFilter->SetInputConnection(imageDataGeometryFilter->GetOutputPort());
	rotationTransformFilter->Update();

	myImageData = vtkSmartPointer<vtkPolyData>::New();
	myImageData->ShallowCopy(rotationTransformFilter->GetOutput());

	// Append image to the rest of the read images.
	#if VTK_MAJOR_VERSION <= 5
	  appendFilter->AddInputConnection(myImageData->GetProducerPort());
    #else
	  appendFilter->AddInputData(myImageData);
    #endif
	appendFilter->Update();

	// Update angle offset between each image.
	angle += 1;
  }

  // Remove any duplicate points.
  vtkSmartPointer<vtkCleanPolyData> cleanFilter =
    vtkSmartPointer<vtkCleanPolyData>::New();
  cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
  //cleanFilter->SetTolerance(0.1);
  cleanFilter->Update();

  // Visualization
  vtkSmartPointer<vtkPolyDataMapper> map = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  map->SetInputConnection(cleanFilter->GetOutputPort());
  //map->ScalarVisibilityOff();
 
  vtkSmartPointer<vtkActor> surfaceActor = 
    vtkSmartPointer<vtkActor>::New();
  surfaceActor->SetMapper(map);
 
  // Create the renderer
  vtkSmartPointer<vtkRenderer> ren = 
    vtkSmartPointer<vtkRenderer>::New();

  // Add the actors to the renderer, set the background and size
  ren->AddActor(surfaceActor);
  ren->SetBackground(.2, .3, .4);
  
  // Create renderer window.
  vtkSmartPointer<vtkRenderWindow> renWin = 
    vtkSmartPointer<vtkRenderWindow>::New();
  renWin->AddRenderer(ren);

  // Create renderer window interactor and set interactor style.
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = 
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renWin);

  vtkSmartPointer<KeyPressInteractorStyle> style = 
    vtkSmartPointer<KeyPressInteractorStyle>::New();
  renderWindowInteractor->SetInteractorStyle(style);
  style->SetCurrentRenderer(ren);

  // Create plane widget and initialize position of widget.
  vtkSmartPointer<vtkImagePlaneWidget> planeWidget = 
    vtkSmartPointer<vtkImagePlaneWidget>::New();
  planeWidget->SetInteractor(renderWindowInteractor);

  // Adjust initial camera position
  vtkSmartPointer<vtkCamera> camera = 
    vtkSmartPointer<vtkCamera>::New();
  camera->SetPosition(1000, 500, 500);
  camera->SetFocalPoint(0, 0, 0);
  camera->SetViewUp(.3, .8, .3);
  ren->SetActiveCamera(camera);
 
  renWin->Render();

  renderWindowInteractor->Initialize();
  renWin->Render();  

  double* firstPoint = planeWidget->GetPoint1();
  double* secondPoint = planeWidget->GetPoint2();

  std::cout << "First Point: " << firstPoint[0] << " " << firstPoint[1] 
            << " " << firstPoint[2] << std::endl;
  std::cout << "Second Point: " << secondPoint[0] << " " << secondPoint[1] 
            << " " << secondPoint[2] << std::endl;


  planeWidget->SetPoint1(200, 0, 0);
  planeWidget->SetPoint2(0, 300, 0);
  planeWidget->UpdatePlacement();
  planeWidget->On();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
