// Generally always include these
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>

// For reading in images
#include <vtkSmartPointer.h>
#include <vtkPNGReader.h>

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
  if (argc < 3)
  {
    std::cout << "Usage: ./PNGReader.exe"
              << " IMAGE_ONE(.png) IMAGE_TWO(.png)" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << argv[1] << " " << argv[2];

  // Read in two PNG images (maybe doesn't have to be done for ARFI images).
  vtkSmartPointer<vtkPNGReader> reader =	
    vtkSmartPointer<vtkPNGReader>::New();
  reader->SetFileName(argv[1]);

  vtkSmartPointer<vtkPNGReader> reader2 =	
    vtkSmartPointer<vtkPNGReader>::New();
  reader2->SetFileName(argv[2]);

  // Convert the PNG image to polydata
  vtkSmartPointer<vtkImageDataGeometryFilter> imageDataGeometryFilter = 
    vtkSmartPointer<vtkImageDataGeometryFilter>::New();
  imageDataGeometryFilter->SetInputConnection(reader->GetOutputPort());
  imageDataGeometryFilter->Update();

  vtkSmartPointer<vtkImageDataGeometryFilter> imageDataGeometryFilter2 = 
    vtkSmartPointer<vtkImageDataGeometryFilter>::New();
  imageDataGeometryFilter2->SetInputConnection(reader2->GetOutputPort());
  imageDataGeometryFilter2->Update();

  // Rotate the second image by 45 degrees.
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  //transform->RotateWXYZ(double angle, double x, double y, double z);
  transform->RotateWXYZ(45, 0, 1, 0);
  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = 
      vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformFilter->SetTransform(transform);
  transformFilter->SetInputConnection(imageDataGeometryFilter2->GetOutputPort());
  transformFilter->Update();

  //Append the two meshes 
  vtkSmartPointer<vtkPolyData> input1 =
    vtkSmartPointer<vtkPolyData>::New();
  input1->ShallowCopy(imageDataGeometryFilter->GetOutput());

  vtkSmartPointer<vtkPolyData> input2 =
    vtkSmartPointer<vtkPolyData>::New();
  input2->ShallowCopy(transformFilter->GetOutput());

  vtkSmartPointer<vtkAppendPolyData> appendFilter =
    vtkSmartPointer<vtkAppendPolyData>::New();
  #if VTK_MAJOR_VERSION <= 5
    appendFilter->AddInputConnection(input1->GetProducerPort());
    appendFilter->AddInputConnection(input2->GetProducerPort());
  #else
    appendFilter->AddInputData(input1);
    appendFilter->AddInputData(input2);
  #endif
  appendFilter->Update();
  
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
