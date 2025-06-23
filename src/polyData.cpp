
#include "polyData.h"
#include <iostream>
#include <vector>
#include <vtkSmartPointer.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkCellData.h>
#include <vtkFieldData.h>
#include <string>


void polyReader::init(std::string filename) {
  this->filename = filename;  
  std::cout<<"Filename read.\n";
  reader = vtkSmartPointer<vtkPolyDataReader>::New();
  reader->SetFileName(filename.c_str());  
  std::cout<<"Filename set.\n";
  reader->Update();
  polyData = reader->GetOutput();
}

void polyReader::read_points(){
  std::cout<<"reading points.\n";
  vtkPoints* points = polyData->GetPoints();
  std::vector<double> pointCoords; // x0, y0, z0, x1, y1, z1, ...

  for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
    double p[3];
    points->GetPoint(i, p);
    pointCoords.push_back(p[0]);
    pointCoords.push_back(p[1]);
    pointCoords.push_back(p[2]);
  }
}

void polyReader::read_connectivity() {
  vtkCellArray* cells = polyData->GetPolys(); // or GetLines(), GetVerts(), etc.
vtkIdType npts;
vtkIdType* ptIds;

cells->InitTraversal();
while (cells->GetNextCell(npts, ptIds)) {
  for (vtkIdType j = 0; j < npts; ++j) {
    connectivity.push_back(static_cast<double>(ptIds[j]));
  }
  }
}

std::vector<double> polyReader::read_scalar(std::string scalar) {
  vtkDataArray* scalar_array = polyData->GetCellData()->GetScalars(scalar.c_str());
  std::vector<double> result;

  if (scalar_array) {
    for (vtkIdType i = 0; i < scalar_array->GetNumberOfTuples(); ++i) {
      result.push_back(scalar_array->GetComponent(i, 0));
    }
  } else {
    std::cerr << "Error: " << scalar << " not found in CELL_DATA.\n";
  }

  return result;
}

std::vector<double> polyReader::read_vector(const std::string& name, bool fromCellData) {
  vtkDataArray* vectorArray = nullptr;

  if (fromCellData) {
    vectorArray = polyData->GetCellData()->GetVectors(name.c_str());
  } else {
    vectorArray = polyData->GetPointData()->GetVectors(name.c_str());
  }

  std::vector<double> result;

  if (vectorArray) {
    for (vtkIdType i = 0; i < vectorArray->GetNumberOfTuples(); ++i) {
      double vec[3];
      vectorArray->GetTuple(i, vec);
      result.push_back(vec[0]);
      result.push_back(vec[1]);
      result.push_back(vec[2]);
    }
  } else {
    std::cerr << "Error: Vector " << name << " not found in " 
              << (fromCellData ? "CELL_DATA.\n" : "POINT_DATA.\n");
  }

  return result;
}

