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

#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkActor2D.h>
#include <vtkImageSlice.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGWriter.h>
#include <vtkSmartPointer.h>


#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkPointData.h>
vtkSmartPointer<vtkImagePlaneWidget> planeWidget;

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

		// Display current image widget plane data
		if(key == "space"){
			std::cout << "Displaying image plane data." << std::endl;
			planeWidget->RestrictPlaneToVolumeOn(); 
			planeWidget->SetResliceInterpolateToCubic();
			planeWidget->DisplayTextOn();
			planeWidget->UseContinuousCursorOn();
			planeWidget->UpdatePlacement();
			vtkSmartPointer<vtkImageData> myImageData = planeWidget->GetResliceOutput();

			int* dims = myImageData->GetDimensions();
			std::cout << "Dims: " << " x: " << dims[0] << " y: " << dims[1] << " z: " << dims[2] << std::endl;


			// Create image mapper to render sliced plane
			vtkSmartPointer<vtkImageMapper> imageMapper = vtkSmartPointer<vtkImageMapper>::New();
			imageMapper->SetInputData(myImageData);

			imageMapper->SetColorWindow(255);
			imageMapper->SetColorLevel(127.5);

			vtkSmartPointer<vtkActor2D> imageActor = vtkSmartPointer<vtkActor2D>::New();
			imageActor->SetMapper(imageMapper);
			//imageActor->SetPosition(20, 20);
			// Setup renderers
			vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

			// Setup render window
			vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();

			renderWindow->AddRenderer(renderer);

			// Setup render window interactor
			vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

			vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New();

			renderWindowInteractor->SetInteractorStyle(style);

			// Render and start interaction
			renderWindowInteractor->SetRenderWindow(renderWindow);

			//renderer->AddViewProp(imageActor);
			renderer->AddActor2D(imageActor);

			renderWindow->Render();
			renderWindowInteractor->Start();

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
	
	vtkSmartPointer<vtkImageData> whiteImage = 
		vtkSmartPointer<vtkImageData>::New();    
	double bounds[6];
	cleanFilter->GetOutput()->GetBounds(bounds);
	double spacing[3]; // desired volume spacing
	spacing[0] = 0.5;
	spacing[1] = 0.5;
	spacing[2] = 0.5;
	whiteImage->SetSpacing(spacing);
	// compute dimensions
	int dim[3];
	for (int i = 0; i < 3; i++)
	{
		dim[i] = static_cast<int>(ceil((bounds[i * 2 + 1] - bounds[i * 2]) / spacing[i]));
	}
	whiteImage->SetDimensions(dim);
	whiteImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

	double origin[3];
	origin[0] = bounds[0] + spacing[0] / 2;
	origin[1] = bounds[2] + spacing[1] / 2;
	origin[2] = bounds[4] + spacing[2] / 2;
	whiteImage->SetOrigin(origin);

	#if VTK_MAJOR_VERSION <= 5
		whiteImage->SetScalarTypeToUnsignedChar();
		whiteImage->AllocateScalars();
	#else
		whiteImage->AllocateScalars(VTK_UNSIGNED_CHAR,1);
	#endif
	// fill the image with foreground voxels:
	unsigned char inval = 255;
	unsigned char outval = 0;
	vtkIdType count = whiteImage->GetNumberOfPoints();
	for (vtkIdType i = 0; i < count; ++i)
	{
		whiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
	}

	// polygonal data --> image stencil:
	vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc = 
		vtkSmartPointer<vtkPolyDataToImageStencil>::New();
	#if VTK_MAJOR_VERSION <= 5
		pol2stenc->SetInput(pd);
	#else
		pol2stenc->SetInputData(cleanFilter->GetOutput());
	#endif
	pol2stenc->SetOutputOrigin(origin);
	pol2stenc->SetOutputSpacing(spacing);
	pol2stenc->SetOutputWholeExtent(whiteImage->GetExtent());
	pol2stenc->Update();

	// cut the corresponding white image and set the background:
	vtkSmartPointer<vtkImageStencil> imgstenc = 
		vtkSmartPointer<vtkImageStencil>::New();
	#if VTK_MAJOR_VERSION <= 5
		imgstenc->SetInput(whiteImage);
		imgstenc->SetStencil(pol2stenc->GetOutput());
	#else
		imgstenc->SetInputData(whiteImage);
		imgstenc->SetStencilConnection(pol2stenc->GetOutputPort());
	#endif
	imgstenc->ReverseStencilOff();
	imgstenc->SetBackgroundValue(outval);
	imgstenc->Update();
	
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
	planeWidget = vtkSmartPointer<vtkImagePlaneWidget>::New();
	planeWidget->SetInteractor(renderWindowInteractor);

	planeWidget->SetInputData(imgstenc->GetOutput());

	planeWidget->GetPlaneProperty()->SetColor(0,0,1);
	planeWidget->SetResliceInterpolateToCubic();
	// Adjust initial camera position
	vtkSmartPointer<vtkCamera> camera = 
		vtkSmartPointer<vtkCamera>::New();
	camera->SetPosition(1000, 500, 500);
	camera->SetFocalPoint(0, 0, 0);
	camera->SetViewUp(.3, .8, .3);
	ren->SetActiveCamera(camera);

	renWin->SetSize(1280, 720);

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
