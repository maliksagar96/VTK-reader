#include <VTKReader.h>
#include <iostream>
#include <vector>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkNew.h>    

VTKReader::VTKReader(const std::string& filename) {
  loadFile(filename);
}

void VTKReader::loadFile(const std::string& filename) {
  this->filename = filename;  
  std::cout << "Filename read.\n";
  reader = vtkSmartPointer<vtkUnstructuredGridReader>::New();
  reader->SetFileName(filename.c_str());  
  std::cout << "Filename set.\n";
  reader->Update();
  grid = reader->GetOutput();
}

VTKReader::~VTKReader() = default;

void VTKReader::loadPoints() {
  std::cout << "Reading points.\n";

  vtkPoints* vtk_pts = grid->GetPoints();
  if (!vtk_pts) return;

  numPoints = vtk_pts->GetNumberOfPoints();
  points.clear();
  points.reserve(3 * numPoints);

  for (int i = 0; i < numPoints; ++i) {
    double p[3];
    vtk_pts->GetPoint(i, p);
    points.push_back(p[0]);
    points.push_back(p[1]);
    points.push_back(p[2]);
  }
}

void VTKReader::loadConnectivity() {

  vtkIdType numCells = grid->GetNumberOfCells();
  numTriangles = 0;

  for (vtkIdType i = 0; i < numCells; ++i) {
    vtkCell* cell = grid->GetCell(i);
    vtkIdList* pointIds = cell->GetPointIds();    

    int type = grid->GetCellType(i);
    if (type != VTK_TRIANGLE) continue;

    numTriangles++;

    for (vtkIdType j = 0; j < pointIds->GetNumberOfIds(); ++j)
      connectivity.push_back(static_cast<int>(pointIds->GetId(j)));

    cellTypes.push_back(type);
  }
}

void VTKReader::loadOffsets() {
  std::cout << "Reading offsets.\n";

  vtkCellArray* cells = grid->GetCells();
  if (!cells) {
    std::cerr << "Error: No cell connectivity found.\n";
    return;
  }

#if VTK_MAJOR_VERSION >= 9
  vtkNew<vtkIdTypeArray> legacy;
  cells->ExportLegacyFormat(legacy);

  offsets.clear();
  connectivity.clear();

  vtkIdType nTuples = legacy->GetNumberOfTuples();
  vtkIdType i = 0;
  vtkIdType offset = 0;

  while (i < nTuples) {
    vtkIdType npts = legacy->GetValue(i++);

    if (npts == 3) {
      offsets.push_back(offset);

      for (vtkIdType j = 0; j < 3; ++j)
        connectivity.push_back(static_cast<int>(legacy->GetValue(i++)));

      offset = connectivity.size();
    } else {
      i += npts;
    }
  }
#endif

  std::cout << "Triangle offsets read: " << offsets.size() << "\n";
}

void VTKReader::computeCentroids() {
  centroids.clear();
  std::vector<int> newConn;
  std::vector<int> newOff;

  int conn_index = 0;
  for (size_t i = 1; i < offsets.size(); ++i) {
    int cell_size = offsets[i] - offsets[i - 1];

    if (cell_size == 3) {
      int id1 = connectivity[conn_index];
      int id2 = connectivity[conn_index + 1];
      int id3 = connectivity[conn_index + 2];

      newConn.insert(newConn.end(), {id1, id2, id3});
      newOff.push_back(static_cast<int>(newConn.size()));

      double cx = (points[id1*3] + points[id2*3] + points[id3*3]) / 3.0;
      double cy = (points[id1*3 + 1] + points[id2*3 + 1] + points[id3*3 + 1]) / 3.0;
      double cz = (points[id1*3 + 2] + points[id2*3 + 2] + points[id3*3 + 2]) / 3.0;

      centroids.push_back(cx);
      centroids.push_back(cy);
      centroids.push_back(cz);
    }  
    conn_index += cell_size;
  }

  connectivity.swap(newConn);
  offsets.swap(newOff);

  std::cout << "Triangle centroids calculated: " 
            << centroids.size() / 3 << " cells.\n";
}

void VTKReader::computeNormals() {
  if (connectivity.empty() || points.empty()) {
    std::cerr << "Error: Connectivity or points empty.\n";
    return;
  }

  normals.clear();
  normals.reserve(connectivity.size());

  for (size_t i = 0; i < connectivity.size(); i += 3) {
    int id1 = connectivity[i];
    int id2 = connectivity[i + 1];
    int id3 = connectivity[i + 2];

    double x1 = points[id1*3], y1 = points[id1*3 + 1], z1 = points[id1*3 + 2];
    double x2 = points[id2*3], y2 = points[id2*3 + 1], z2 = points[id2*3 + 2];
    double x3 = points[id3*3], y3 = points[id3*3 + 1], z3 = points[id3*3 + 2];

    double ux = x2 - x1, uy = y2 - y1, uz = z2 - z1;
    double vx = x3 - x1, vy = y3 - y1, vz = z3 - z1;

    double nx = uy * vz - uz * vy;
    double ny = uz * vx - ux * vz;
    double nz = ux * vy - uy * vx;

    double area = 0.5 * std::sqrt(nx*nx + ny*ny + nz*nz);
    areas.push_back(area);

    double len = std::sqrt(nx*nx + ny*ny + nz*nz);
    if (len > 1e-12) {
      nx /= len; ny /= len; nz /= len;
    }

    normals.push_back(nx);
    normals.push_back(ny);
    normals.push_back(nz);
  }

  std::cout << "Normals calculated for " << normals.size() / 3 << " triangles.\n";
}

std::vector<double> VTKReader::readScalar(const std::string& name) {
  vtkDataArray* arr = grid->GetCellData()->GetScalars(name.c_str());
  std::vector<double> result;

  if (arr) {
    for (vtkIdType i = 0; i < arr->GetNumberOfTuples(); ++i)
      result.push_back(arr->GetComponent(i, 0));
  } else {
    std::cerr << "Error: " << name << " not found in CELL_DATA.\n";
  }

  return result;
}

std::vector<std::vector<double>> VTKReader::readVector(const std::string& name, bool fromCellData) {
  vtkDataArray* arr = fromCellData
    ? grid->GetCellData()->GetVectors(name.c_str())
    : grid->GetPointData()->GetVectors(name.c_str());

  std::vector<std::vector<double>> result;

  if (arr) {
    for (vtkIdType i = 0; i < arr->GetNumberOfTuples(); ++i) {
      double v[3];
      arr->GetTuple(i, v);
      result.push_back({v[0], v[1], v[2]});
    }
  } else {
    std::cerr << "Error: vector " << name 
              << " not found in " 
              << (fromCellData ? "CELL_DATA\n" : "POINT_DATA\n");
  }

  return result;
}

std::vector<std::string> VTKReader::getScalarNames(bool fromCellData) {
  vtkDataSetAttributes* attrs =
      fromCellData
        ? static_cast<vtkDataSetAttributes*>(grid->GetCellData())
        : static_cast<vtkDataSetAttributes*>(grid->GetPointData());

  std::vector<std::string> names;

  int n = attrs->GetNumberOfArrays();
  for (int i = 0; i < n; ++i) {
    const char* nm = attrs->GetArrayName(i);
    if (nm) names.push_back(nm);
  }
  return names;
}


std::vector<std::string> VTKReader::getVectorNames(bool fromCellData) {
  vtkDataSetAttributes* attrs =
      fromCellData
        ? static_cast<vtkDataSetAttributes*>(grid->GetCellData())
        : static_cast<vtkDataSetAttributes*>(grid->GetPointData());

  std::vector<std::string> names;

  int n = attrs->GetNumberOfArrays();
  for (int i = 0; i < n; ++i) {
    vtkDataArray* arr = attrs->GetArray(i);
    if (arr && arr->GetNumberOfComponents() == 3) {
      const char* nm = arr->GetName();
      if (nm) names.push_back(nm);
    }
  }
  return names;
}
