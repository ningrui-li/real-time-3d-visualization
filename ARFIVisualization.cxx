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
#include <vtkProbeFilter.h>
#include <vtkDelaunay2D.h>
#include <vtkPlaneSource.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkClipDataSet.h>
#include <vtkPlane.h>

#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
int main(int argc, char* argv[])
{
    vtkSmartPointer<vtkJPEGReader> reader;
    vtkSmartPointer<vtkImageDataGeometryFilter> imageDataGeometryFilter;
    vtkSmartPointer<vtkTransform> rotationTransform;
    vtkSmartPointer<vtkTransformPolyDataFilter> rotationTransformFilter;
    vtkSmartPointer<vtkPolyData> myImageData;

    vtkSmartPointer<vtkAppendPolyData> appendPolyDataFilter =
        vtkSmartPointer<vtkAppendPolyData>::New();

    double angle = 0.0;
    double angleOffset = 5; // Rotate each image by 20 degrees.

    /* 
    Read in each image, convert it to a vtkStructuredGrid, then rotate it by
    angleOffset degrees and append it to the rest of the read images.
    */
    for (std::string imageFileName; std::getline(std::cin, imageFileName);) {
        std::cout << imageFileName << std::endl;

        // Read in image data as vtkImageData.
        reader = vtkSmartPointer<vtkJPEGReader>::New();
        reader->SetFileName(imageFileName.c_str());
    
        /*
        // Code for converting from 
        // vtkImageData -> vtkStructuredGrid -> vtkUnstructuredGrid.
        
        // Convert vtkImageData to vtkStructuredGrid.
        vtkSmartPointer<vtkImageDataToPointSet> imageDataToPointSet =
            vtkSmartPointer<vtkImageDataToPointSet>::New();
        #if VTK_MAJOR_VERSION <= 5
            imageDataToPointSet->SetInputConnection(reader->GetProducerPort());
        #else
            imageDataToPointSet->SetInputConnection(reader->GetOutputPort());
        #endif
        imageDataToPointSet->Update();

        // Convert vtkStructuredGrid to vtkUnstructuredGrid with 
        // vtkDataSetTriangleFilter.
        */

        // Code for converting from 
        // vtkImageData -> vtkPolyData -> vtkUnstructuredGrid.
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
        
        // Consider skipping this step and just adding data to the appendFilter
        // to more directly convert to vtkUnstructuredGrid data.
        #if VTK_MAJOR_VERSION <= 5
            appendPolyDataFilter->AddInputConnection(myImageData->GetProducerPort());
        #else
            appendPolyDataFilter->AddInputData(myImageData);
        #endif
        appendPolyDataFilter->Update();

        angle += angleOffset;
    }

    // Remove any duplicate points.
    vtkSmartPointer<vtkCleanPolyData> cleanFilter =
        vtkSmartPointer<vtkCleanPolyData>::New();
    cleanFilter->SetInputConnection(appendPolyDataFilter->GetOutputPort());
    cleanFilter->Update();

    // Convert vtkPolyData to vtkUnstructuredGrid using vtkAppendFilter.
    vtkSmartPointer<vtkAppendFilter> appendFilter =
        vtkSmartPointer<vtkAppendFilter>::New();
    #if VTK_MAJOR_VERSION <= 5
        appendFilter->AddInput(sphereSource->GetOutput());
    #else
        appendFilter->AddInputData(cleanFilter->GetOutput());
    #endif
    appendFilter->Update();
    
    // Triangulate the image data.
    vtkSmartPointer<vtkDataSetTriangleFilter> triangleFilter =
        vtkSmartPointer<vtkDataSetTriangleFilter>::New();
    triangleFilter->SetInputConnection(appendFilter->GetOutputPort());    
    triangleFilter->Update();


    // Cast output of appendFilter to vtkUnstructuredGrid
    vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid =
        vtkSmartPointer<vtkUnstructuredGrid>::New();
    unstructuredGrid->ShallowCopy(triangleFilter->GetOutput());

    std::cout << unstructuredGrid->GetScalarRange()[0] << " " << unstructuredGrid->GetScalarRange()[1] << std::endl;

    // Apply vtkClipDataSet filter for interpolation.    
    // Create a vtkPlane (implicit function) to interpolate over
    vtkSmartPointer<vtkPlane> clipPlane =
        vtkSmartPointer<vtkPlane>::New();
    clipPlane->SetOrigin(0.0, 0.0, 10.0);
    clipPlane->SetNormal(1.0, 0.0, 1.0);

    // Perform the interpolation
    vtkSmartPointer<vtkClipDataSet> clipDataSet =
        vtkSmartPointer<vtkClipDataSet>::New();
    clipDataSet->SetClipFunction(clipPlane);
    clipDataSet->InsideOutOn();

    clipDataSet->SetInputConnection(triangleFilter->GetOutputPort());
#if VTK_MAJOR_VERSION <= 5
    probeFilter->SetInput(gridPolyDat)a; // 
    // Interpolate 'Source' at these points
#else
    clipDataSet->SetInputData(triangleFilter->GetOutput());
    // Interpolate 'Source' at these points
#endif
    clipDataSet->Update();
    
    std::cout << clipDataSet->GetInsideOut() << std::endl;

    // Create mapper and actor for interpolated points.
    vtkSmartPointer<vtkDataSetMapper> gridMapper =
        vtkSmartPointer<vtkDataSetMapper>::New();
    gridMapper->SetInputConnection(clipDataSet->GetOutputPort());
    //gridMapper->ScalarVisibilityOff();

    vtkSmartPointer<vtkActor> gridActor =
        vtkSmartPointer<vtkActor>::New();
    gridActor->SetMapper(gridMapper);
    //gridActor->GetProperty()->SetColor(0.0, 0.0, 1.0); //(R,G,B)
    //gridActor->GetProperty()->SetPointSize(3);
 

    // Create a mapper and actor for image data.
    vtkSmartPointer<vtkDataSetMapper> mapper = 
        vtkSmartPointer<vtkDataSetMapper>::New();
    #if VTK_MAJOR_VERSION <= 5
        mapper->SetInputConnection(unstructuredGrid->GetProducerPort());
    #else
        mapper->SetInputData(clipDataSet->GetOutput());
    #endif
    vtkSmartPointer<vtkActor> actor = 
        vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    // Visualization
    vtkSmartPointer<vtkRenderer> renderer = 
        vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> renderWindow = 
        vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = 
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add orientation axes
    vtkSmartPointer<vtkAxesActor> axes = 
        vtkSmartPointer<vtkAxesActor>::New();

    vtkSmartPointer<vtkOrientationMarkerWidget> widget = 
        vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    widget->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
    widget->SetOrientationMarker( axes );
    widget->SetInteractor( renderWindowInteractor );
    //widget->SetViewport( 0.0, 0.0, 0.4, 0.4 );
    widget->SetEnabled( 1 );
    widget->InteractiveOn();

    renderer->AddActor(actor);
    renderer->AddActor(gridActor);
    renderer->SetBackground(.1, .2, .3); // Background color white

    renderWindow->Render();

    // Start interactive window.
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
