#include <iostream>
#include <string>

// Generally always include these
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>

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
//#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>

int main(int argc, char* argv[])
{
  // Verify input arguments
  /*
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0]
              << " IMAGE_ONE(.png) IMAGE_TWO(.png)" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << argv[1] << " " << argv[2] << " " << argc;
  // std::cout << 
  */
  vtkSmartPointer<vtkJPEGReader> reader;
  vtkSmartPointer<vtkImageDataGeometryFilter> imageDataGeometryFilter;
  vtkSmartPointer<vtkTransform> rotationTransform;
  vtkSmartPointer<vtkTransformPolyDataFilter> rotationTransformFilter;
  vtkSmartPointer<vtkPolyData> myImageData;

  vtkSmartPointer<vtkAppendPolyData> appendFilter =
    vtkSmartPointer<vtkAppendPolyData>::New();

  double angle = 0;

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
	rotationTransform->RotateWXYZ(angle, 0, 1, 0);

	rotationTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	rotationTransformFilter->SetTransform(rotationTransform);
	rotationTransformFilter->SetInputConnection(imageDataGeometryFilter->GetOutputPort());
	rotationTransformFilter->Update();

	myImageData = vtkSmartPointer<vtkPolyData>::New();
	myImageData->ShallowCopy(rotationTransformFilter->GetOutput());

	#if VTK_MAJOR_VERSION <= 5
	  appendFilter->AddInputConnection(myImageData->GetProducerPort());
    #else
	  appendFilter->AddInputData(myImageData);
    #endif
	appendFilter->Update();

	angle += 20;
  }

  //return EXIT_SUCCESS;

  // Remove any duplicate points.
  vtkSmartPointer<vtkCleanPolyData> cleanFilter =
    vtkSmartPointer<vtkCleanPolyData>::New();
  cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
  cleanFilter->Update();

  // Create a mapper and actor
  vtkSmartPointer<vtkPolyDataMapper> mapper = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(cleanFilter->GetOutputPort());

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  // Rotate actor
  // actor->RotateY(45);

   // Visualization
  vtkSmartPointer<vtkRenderer> renderer = 
    vtkSmartPointer<vtkRenderer>::New();

  vtkSmartPointer<vtkRenderWindow> renderWindow = 
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = 
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
 
  renderer->AddActor(actor);
  renderer->SetBackground(1, 1, 1); // Background color white
 
  renderWindow->Render();

  // Start interactive window.
  renderWindowInteractor->Start();
 
  return EXIT_SUCCESS;
}
