#include "polyData.h"

polyReader::polyReader(const std::string& filename) {
  init(filename);
}

void polyReader::init(const std::string& filename) {
  this->filename = filename;
  std::cout << "Filename read.\n";

  reader = vtkSmartPointer<vtkPolyDataReader>::New();
  reader->SetFileName(filename.c_str());
  std::cout << "Filename set.\n";

  reader->Update();
  polyData = reader->GetOutput();
}

void polyReader::read_points() {
  std::cout << "Reading points.\n";
  vtkPoints* vtk_pts = polyData->GetPoints();
  if (!vtk_pts) return;

  points.clear();
  points.reserve(3 * vtk_pts->GetNumberOfPoints());

  for (vtkIdType i = 0; i < vtk_pts->GetNumberOfPoints(); ++i) {
    double p[3];
    vtk_pts->GetPoint(i, p);
    points.push_back(p[0]);
    points.push_back(p[1]);
    points.push_back(p[2]);
  }
}

void polyReader::read_connectivity() {
  vtkCellArray* cells = polyData->GetPolys(); // Can also use GetLines/GetVerts if needed
  vtkIdType npts;
  vtkIdType* ptIds;

  connectivity.clear();

  cells->InitTraversal();
  while (cells->GetNextCell(npts, ptIds)) {
    for (vtkIdType j = 0; j < npts; ++j) {
      connectivity.push_back(static_cast<double>(ptIds[j]));
    }
  }
}

std::vector<double> polyReader::read_scalar(const std::string& name) {
  vtkDataArray* scalar_array = polyData->GetCellData()->GetScalars(name.c_str());
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

std::vector<std::vector<double>> polyReader::read_vector(const std::string& name, bool fromCellData) {
  vtkDataArray* vectorArray = nullptr;

  if (fromCellData) {
    vectorArray = polyData->GetCellData()->GetVectors(name.c_str());
  } else {
    vectorArray = polyData->GetPointData()->GetVectors(name.c_str());
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

int polyReader::get_numpoints() const {
  return points.size() / 3;
}

int polyReader::get_numcells() const {
  return get_numpoints()/3;
}



std::vector<std::string> polyReader::get_scalar_names(bool fromCellData) {
vtkDataSetAttributes* dataAttrs = fromCellData
                                   ? static_cast<vtkDataSetAttributes*>(polyData->GetCellData())
                                   : static_cast<vtkDataSetAttributes*>(polyData->GetPointData());


  std::vector<std::string> names;
  int numArrays = dataAttrs->GetNumberOfArrays();
  for (int i = 0; i < numArrays; ++i) {
    const char* name = dataAttrs->GetArrayName(i);
    if (name) names.push_back(std::string(name));
  }
  return names;
}

std::vector<std::string> polyReader::get_vector_names(bool fromCellData) {
  vtkDataSetAttributes* dataAttrs = fromCellData
                                       ? static_cast<vtkDataSetAttributes*>(polyData->GetCellData())
                                       : static_cast<vtkDataSetAttributes*>(polyData->GetPointData());

  std::vector<std::string> vectorNames;
  int numArrays = dataAttrs->GetNumberOfArrays();
  for (int i = 0; i < numArrays; ++i) {
    vtkDataArray* arr = dataAttrs->GetArray(i);
    if (arr && arr->GetNumberOfComponents() == 3) {
      const char* name = arr->GetName();
      if (name) vectorNames.push_back(std::string(name));
    }
  }
  return vectorNames;
}


