#include "vtkReader.h"
#include <vector>
#include <iostream>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>

void VTKReader::init(std::string filename) {
  this->filename = filename;
  vtkSmartPointer<vtkUnstructuredGridReader> reader = vtkSmartPointer<vtkUnstructuredGridReader>::New();
  reader->SetFileName(filename.c_str());
  reader->Update();
  grid = reader->GetOutput();
}

void VTKReader::read_points() {
  
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

  for(int i = 0;i<points.size();i++) {
    std::cout<<"Points = "<<points[i]<<"\n";
  }
}

int VTKReader::get_numpoints() {
  return num_points;
}

void VTKReader::read_connectivity() {

  // vtkCellArray* cells = grid->GetCells();
  // connectivity.clear();

  // vtkIdType npts;
  // const vtkIdType* pts;

  // cells->InitTraversal();
  // while (cells->GetNextCell(npts, pts)) {
  //   for (vtkIdType i = 0; i < npts; ++i) {
  //     connectivity.push_back(static_cast<double>(pts[i]));
  //     std::cout<<"Connectivity = "<<connectivity[i]<<"\n";
  //   }
  // }
}

void VTKReader::calc_normal() {
  
}

void VTKReader::read_scalar(std::vector<double>& scalar) {

}

void VTKReader::read_vector(std::vector<std::vector<double>>& vect) {

}

