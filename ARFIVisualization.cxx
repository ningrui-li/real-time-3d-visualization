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
    
    //
    vtkSmartPointer<vtkDataSetTriangleFilter> triangleFilter =
        vtkSmartPointer<vtkDataSetTriangleFilter>::New();
    triangleFilter->SetInputConnection(appendFilter->GetOutputPort());    
    triangleFilter->Update();

    // Cast output of appendFilter to vtkUnstructuredGrid
    vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid =
        vtkSmartPointer<vtkUnstructuredGrid>::New();
    unstructuredGrid->ShallowCopy(triangleFilter->GetOutput());
    
    // Create a mapper.
    vtkSmartPointer<vtkDataSetMapper> mapper = 
        vtkSmartPointer<vtkDataSetMapper>::New();
    #if VTK_MAJOR_VERSION <= 5
        mapper->SetInputConnection(unstructuredGrid->GetProducerPort());
    #else
        mapper->SetInputData(unstructuredGrid);
    #endif

    // Create an actor.
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

    renderer->AddActor(actor);
    renderer->SetBackground(1, 1, 1); // Background color white

    renderWindow->Render();

    // Start interactive window.
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}