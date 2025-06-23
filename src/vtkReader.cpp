#include "vtkReader.h"
#include <iostream>
#include <vector>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>


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
  for (vtkIdType i = 0; i < numCells; ++i) {
    vtkCell* cell = grid->GetCell(i);
    vtkIdList* pointIds = cell->GetPointIds();    

    // Store connectivity
    for (vtkIdType j = 0; j < pointIds->GetNumberOfIds(); ++j) {
        connectivity.push_back(static_cast<double>(pointIds->GetId(j)));
    }
    // Store cell type
    cellType.push_back(static_cast<double>(grid->GetCellType(i)));
    
  }
}

void VTKReader::calc_normal() {
  
}

std::vector<double> VTKReader::read_scalar(const std::string& name) {
  vtkDataArray* scalar_array = grid->GetCellData()->GetScalars(name.c_str());
  std::vector<double> result;

  if (scalar_array) {
    for (vtkIdType i = 0; i < scalar_array->GetNumberOfTuples(); ++i) {
      result.push_back(scalar_array->GetComponent(i, 0));
    }
  } else {
    std::cerr << "Error: " << name << " not found in CELL_DATA.\n";
  }

  return result;
}


std::vector<std::vector<double>> VTKReader::read_vector(const std::string& name, bool fromCellData) {
  vtkDataArray* vectorArray = nullptr;

  if (fromCellData) {
    vectorArray = grid->GetCellData()->GetVectors(name.c_str());
  } else {
    vectorArray = grid->GetPointData()->GetVectors(name.c_str());
  }

  std::vector<std::vector<double>> result;

  if (vectorArray) {
    for (vtkIdType i = 0; i < vectorArray->GetNumberOfTuples(); ++i) {
      double vec[3];
      vectorArray->GetTuple(i, vec);
      result.push_back({vec[0], vec[1], vec[2]});      
    }
  } else {
    std::cerr << "Error: Vector " << name << " not found in "
              << (fromCellData ? "CELL_DATA.\n" : "POINT_DATA.\n");
  }

  return result;
}

std::vector<std::string> VTKReader::get_scalar_names(bool fromCellData) {
  vtkDataSetAttributes* dataAttrs =fromCellData
  ? static_cast<vtkDataSetAttributes*>(grid->GetCellData())
  : static_cast<vtkDataSetAttributes*>(grid->GetPointData());

  std::vector<std::string> names;
  int numArrays = dataAttrs->GetNumberOfArrays();
  for (int i = 0; i < numArrays; ++i) {
    const char* name = dataAttrs->GetArrayName(i);
    if (name) names.push_back(std::string(name));
  }
  return names;
}
