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
#include <vtkCellData.h>
#include <vtkFieldData.h>
#include <vtkPointData.h>

class polyReader {

public:
  polyReader(const std::string& filename) {
    init(filename);
  }

  ~polyReader() = default;

  void init(const std::string& filename);
  void read_points();
  void read_connectivity();

  std::vector<double> read_scalar(const std::string& name);
  std::vector<std::vector<double>> read_vector(const std::string& name, bool fromCellData = true);
  std::vector<std::string> get_scalar_names(bool fromCellData = true);

  int get_numpoints() const;

private:
  std::string filename;
  vtkSmartPointer<vtkPolyDataReader> reader;
  vtkPolyData* polyData = nullptr;

  std::vector<double> points, connectivity;
};

#endif
