/**
 * @file vtkReader.h
 * @author Sagar Malik (github - maliksagar96)
 * @brief A library to read unstructured grid VTK files using VTK.
 * 
 * ## Usage
 * 1. Define constructor and input the filename  
 * 2. Read points using `read_points()`  
 * 3. Read connectivity using `read_connectivity()`  
 * 4. Read scalar or vector fields using `read_scalar()` or `read_vector()`  
 * 5. Use `get_scalar_names()` or `get_vector_names()` to explore field names
 * 
 * @version 0.1
 * @date 2025-07-08
 * 
 * @copyright Copyright (c) 2025
 */

#ifndef VTK_READER_H
#define VTK_READER_H

#include <iostream>
#include <vector>
#include <string>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>

/**
 * @class VTKReader
 * @brief A wrapper class to read `.vtk` files containing vtkUnstructuredGrid data.
 */
class VTKReader {
public:
  /**
   * @brief Constructor that initializes the reader with a file.
   * @param filename Path to the input VTK file.
   */
  VTKReader(std::string filename);

  /**
   * @brief Destructor.
   */
  ~VTKReader();

  /**
   * @brief Initializes and reads the input file.
   * @param filename Path to the VTK file.
   */
  void init(std::string filename);

  /**
   * @brief Reads all point coordinates into the `points` vector.
   */
  void read_points();

  /**
   * @brief Reads cell connectivity into the `connectivity` vector.
   */
  void read_connectivity();

  /**
   * @brief Calculates normal vectors for surface cells.
   */
  void calc_normal();

  /**
   * @brief Reads scalar field data by name.
   * @param name Name of the scalar field.
   * @return A vector of scalar values.
   */
  std::vector<double> read_scalar(const std::string& name);

  /**
   * @brief Reads vector field data by name.
   * @param name Name of the vector field.
   * @param fromCellData If true, reads from cell data; otherwise from point data.
   * @return A vector of 3D vectors.
   */
  std::vector<std::vector<double>> read_vector(const std::string& name, bool fromCellData = true);

  /**
   * @brief Gets the names of all scalar fields in the file.
   * @param fromCellData If true, returns names from cell data; otherwise from point data.
   * @return A list of scalar field names.
   */
  std::vector<std::string> get_scalar_names(bool fromCellData = true);

  /**
   * @brief Gets the names of all vector fields in the file.
   * @param fromCellData If true, returns names from cell data; otherwise from point data.
   * @return A list of vector field names.
   */
  std::vector<std::string> get_vector_names(bool fromCellData = true);

  /**
   * @brief Returns the number of points in the mesh.
   * @return Number of points.
   */
  int get_numpoints() const;

  /**
   * @brief Returns the number of cells in the mesh.
   * @return Number of cells.
   */
  int get_numcells() const;

private:
  std::string filename; ///< Path to the input VTK file.
  vtkSmartPointer<vtkUnstructuredGridReader> reader; ///< VTK reader object.
  vtkUnstructuredGrid* grid; ///< Raw pointer to the unstructured grid.
  int num_points; ///< Number of mesh points.

  std::vector<double> points; ///< Flattened vector of point coordinates.
  std::vector<double> connectivity; ///< Flattened vector of cell connectivity.
  std::vector<double> normal; ///< Surface normals (optional).
  std::vector<double> area; ///< Area per cell (if computed).
  std::vector<double> cellType; ///< VTK cell type codes (if needed).
};

#endif
