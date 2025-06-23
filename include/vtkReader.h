#ifndef VTK_READER_H
#define VTK_READER_H

#include <iostream>
#include <vector>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <string>

class VTKReader {
  public:

   VTKReader(std::string filename) {
    init(filename);
  };

  ~VTKReader();

  void init(std::string);
  void read_points();
  void read_connectivity();
  void calc_normal();
  std::vector<double> read_scalar(const std::string& name);
  std::vector<std::vector<double>> read_vector(const std::string& name, bool fromCellData=true);
  std::vector<std::string> get_scalar_names(bool fromCellData = true);
  int get_numpoints();
  
  private:
  std::string filename;
  vtkSmartPointer<vtkUnstructuredGridReader> reader;
  int num_points;

  std::vector<double> points, connectivity, normal, area, cellType; 
  vtkUnstructuredGrid* grid;
  
};

#endif