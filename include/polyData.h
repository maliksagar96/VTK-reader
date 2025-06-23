#ifndef POLYDATA_H
#define POLYDATA_H


#include <iostream>
#include <string>
#include <vector>
#include <vtkSmartPointer.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <iostream>
#include <vtkCellData.h>
#include <vtkFieldData.h>

class polyReader {

public:
  polyReader(const std::string& filename) {
    init(filename);
  }

  ~polyReader() = default; 

  void init(const std::string filename); // declare it properly
  void read_points();
  void read_connectivity();
  std::vector<double> read_scalar(std::string scalar);

private:
  std::string filename;
  vtkSmartPointer<vtkPolyDataReader> reader;
  vtkPolyData* polyData;

  std::vector<double> points, connectivity, normal, area, cellType; 
  
};

#endif
