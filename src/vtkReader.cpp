#include "vtkReader.h"
#include <vector>
#include <iostream>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <iostream>

void VTKReader::init(std::string filename) {
  this->filename = filename;  
  std::cout<<"Filename read.\n";
  reader = vtkSmartPointer<vtkUnstructuredGridReader>::New();
  reader->SetFileName(filename.c_str());  
  std::cout<<"Filename set.\n";
  reader->Update();
  grid = reader->GetOutput();
}

VTKReader::~VTKReader() = default ;

void VTKReader::read_points() {
  std::cout<<"reading points.\n";

  vtkPoints* vtk_pts = grid->GetPoints();
  
  if (!vtk_pts) return;

  num_points = vtk_pts->GetNumberOfPoints();
  points.clear();
  points.reserve(3 * num_points);

  for (int i = 0; i < num_points; ++i) {
    double p[3];
    vtk_pts->GetPoint(i, p);
    points.push_back(p[0]);
    points.push_back(p[1]);
    points.push_back(p[2]);
  }

  // for(int i = 0;i<points.size();i++) {
  //   std::cout<<"Points = "<<points[i]<<"\n";
  // }
}

int VTKReader::get_numpoints() {
  return num_points;
}

void VTKReader::read_connectivity() {

  vtkIdType numCells = grid->GetNumberOfCells();
  std::cout<<"numcells = "<<numCells<<"\n";
  for (vtkIdType i = 0; i < numCells; ++i) {
    vtkCell* cell = grid->GetCell(i);
    vtkIdList* pointIds = cell->GetPointIds();    

    // Store connectivity
    for (vtkIdType j = 0; j < pointIds->GetNumberOfIds(); ++j) {
        connectivity.push_back(static_cast<double>(pointIds->GetId(j)));
        
    }
  
    std::cout<<"**************************\n";
    // Store cell type
    cellType.push_back(static_cast<double>(grid->GetCellType(i)));
    // std::cout<<"cellType = "<<cellType[i]<<"\n";
  }
}

void VTKReader::calc_normal() {
  
}

void VTKReader::read_scalar(std::vector<double>& scalar) {

}

void VTKReader::read_vector(std::vector<std::vector<double>>& vect) {

}

