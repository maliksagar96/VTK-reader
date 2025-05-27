#ifndef VTK_READER_H
#define VTK_READER_H

#include <iostream>
#include <vector>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>


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
  void read_scalar(std::vector<double>&);
  void read_vector(std::vector<std::vector<double>>&);
  int get_numpoints();

  

  private:
  std::string filename;
  vtkSmartPointer<vtkUnstructuredGridReader> reader;
  int num_points;

  std::vector<double> points, connectivity, normal, area; 
  vtkUnstructuredGrid* grid;




};

#endif