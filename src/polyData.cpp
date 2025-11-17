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
  vtkCellArray* cells = polyData->GetPolys();
  vtkIdType npts;
  const vtkIdType* ptIds;   // ✔ Fix: must be const

  connectivity.clear();

  cells->InitTraversal();
  while (cells->GetNextCell(npts, ptIds)) {
    for (vtkIdType j = 0; j < npts; ++j) {
      connectivity.push_back(static_cast<int>(ptIds[j]));
    }
  }
}

void polyReader::read_offset() {
  std::cout << "Reading offsets.\n";

  vtkCellArray* cells = polyData->GetPolys();
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
    offsets.push_back(offset);
    for (vtkIdType j = 0; j < npts; ++j)
      connectivity.push_back(static_cast<int>(legacy->GetValue(i++)));
    offset = connectivity.size();
  }

#else
  vtkIdTypeArray* offsetsArray = vtkIdTypeArray::SafeDownCast(cells->GetOffsetsArray());
  if (!offsetsArray) {
    std::cerr << "Error: No offsets array found or invalid type.\n";
    return;
  }

  vtkIdType numOffsets = offsetsArray->GetNumberOfTuples();
  offsets.clear();
  offsets.reserve(numOffsets);

  for (vtkIdType i = 0; i < numOffsets; ++i)
    offsets.push_back(static_cast<int>(offsetsArray->GetValue(i)));
#endif

  std::cout << "Offsets read: " << offsets.size() << " entries.\n";
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

void polyReader::calc_centroids() {
  centroids.clear();

  std::vector<int> new_conn;
  std::vector<int> new_off;

  int conn_index = 0;

  for (size_t i = 1; i < offsets.size(); ++i) {
    int cell_size = offsets[i] - offsets[i - 1];

    if (cell_size == 3) { // Triangles only
      int id1 = connectivity[conn_index];
      int id2 = connectivity[conn_index + 1];
      int id3 = connectivity[conn_index + 2];

      // store new triangle connectivity
      new_conn.insert(new_conn.end(), {id1, id2, id3});
      new_off.push_back(static_cast<int>(new_conn.size()));

      // centroid
      double cx = (points[id1*3]     + points[id2*3]     + points[id3*3])     / 3.0;
      double cy = (points[id1*3 + 1] + points[id2*3 + 1] + points[id3*3 + 1]) / 3.0;
      double cz = (points[id1*3 + 2] + points[id2*3 + 2] + points[id3*3 + 2]) / 3.0;

      centroids.push_back(cx);
      centroids.push_back(cy);
      centroids.push_back(cz);
    }

    conn_index += cell_size;
  }

  connectivity.swap(new_conn);
  offsets.swap(new_off);

  std::cout << "Triangle centroids calculated: " << centroids.size()/3 << " cells.\n";
}


void polyReader::calc_normal() {
  if (connectivity.empty() || points.empty()) {
    std::cerr << "Error: Connectivity or points empty. Cannot compute normals.\n";
    return;
  }

  normals.clear();
  areas.clear();
  normals.reserve(connectivity.size());

  for (size_t i = 0; i < connectivity.size(); i += 3) {
    int id1 = connectivity[i];
    int id2 = connectivity[i + 1];
    int id3 = connectivity[i + 2];

    // points
    double x1 = points[id1*3],     y1 = points[id1*3 + 1],     z1 = points[id1*3 + 2];
    double x2 = points[id2*3],     y2 = points[id2*3 + 1],     z2 = points[id2*3 + 2];
    double x3 = points[id3*3],     y3 = points[id3*3 + 1],     z3 = points[id3*3 + 2];

    // vectors
    double ux = x2 - x1, uy = y2 - y1, uz = z2 - z1;
    double vx = x3 - x1, vy = y3 - y1, vz = z3 - z1;

    // cross product
    double nx = uy * vz - uz * vy;
    double ny = uz * vx - ux * vz;
    double nz = ux * vy - uy * vx;

    // area
    double area = 0.5 * std::sqrt(nx*nx + ny*ny + nz*nz);
    areas.push_back(area);

    // normalize
    double len = std::sqrt(nx*nx + ny*ny + nz*nz);
    if (len > 1e-12) {
      nx /= len;
      ny /= len;
      nz /= len;
    }

    normals.push_back(nx);
    normals.push_back(ny);
    normals.push_back(nz);
  }

  std::cout << "Normals calculated for " << normals.size() / 3 << " triangles.\n";
}
