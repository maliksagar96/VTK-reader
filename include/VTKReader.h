/**
 * @file VTKReader.h
 * @author 
 *   Sagar Malik (github.com/maliksagar96)
 *
 * @brief A modern C++ wrapper for reading `.vtk` unstructured grid files using VTK.
 *
 * ## Typical Usage:
 *   VTKReader reader("mesh.vtk");
 *   reader.loadPoints();
 *   reader.loadConnectivity();
 *   reader.loadOffsets();
 *   reader.computeCentroids();
 *   reader.computeNormals();
 *
 *   auto pts = reader.getPoints();
 *   auto conn = reader.getConnectivity();
 *   auto norms = reader.getNormals();
 *
 * @version 1.0
 * @date 2025-07-08
 */

#ifndef VTK_READER_H
#define VTK_READER_H

#include <vector>
#include <string>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>

/**
 * @class VTKReader
 * @brief A lightweight class to read VTK unstructured grid files and extract
 *        points, connectivity, scalar fields, vector fields, normals, etc.
 */
class VTKReader {
public:
  /// Construct and load the VTK file.
  explicit VTKReader(const std::string& filename);

  /// Destructor.
  ~VTKReader();

  /// Loads the file and initializes internal VTK structures.
  void loadFile(const std::string& filename);

  /// Reads point coordinates into `points`.
  void loadPoints();

  /// Reads triangle connectivity into `connectivity`.
  void loadConnectivity();

  /// Reads offsets (triangle-only) into `offsets`.
  void loadOffsets();

  /// Computes centroids for each triangle.
  void computeCentroids();

  /// Computes normals + areas for each triangle.
  void computeNormals();

  /// Reads a scalar field by name.
  std::vector<double> readScalar(const std::string& name);

  /// Reads a 3D vector field by name.
  std::vector<std::vector<double>> readVector(const std::string& name, bool fromCellData = true);

  /// Returns list of scalar field names.
  std::vector<std::string> getScalarNames(bool fromCellData = true);

  /// Returns list of vector field names.
  std::vector<std::string> getVectorNames(bool fromCellData = true);

  /// Number of points in the mesh.
  int getNumPoints() const { return numPoints; }

  /// Number of triangles.
  int getNumCells() const { return numTriangles; }

  // ---- Getters for raw data ----
  const std::vector<double>& getPoints() const { return points; }
  const std::vector<int>&    getConnectivity() const { return connectivity; }
  const std::vector<int>&    getOffsets() const { return offsets; }
  const std::vector<double>& getCentroids() const { return centroids; }
  const std::vector<double>& getNormals() const { return normals; }
  const std::vector<double>& getAreas() const { return areas; }

private:
  std::string filename;
  vtkSmartPointer<vtkUnstructuredGridReader> reader;
  vtkUnstructuredGrid* grid = nullptr;

  int numPoints = 0;
  int numTriangles = 0;

  std::vector<double> points;      // Flattened XYZs
  std::vector<int>    connectivity; // Flattened 3*N triangle indices
  std::vector<int>    offsets;      // Offsets for triangle connectivity

  std::vector<double> centroids; // Flattened centroid triples
  std::vector<double> normals;   // Flattened normal triples
  std::vector<double> areas;     // Area per triangle
  std::vector<int>    cellTypes; // VTK cell types (optional)
};

#endif
