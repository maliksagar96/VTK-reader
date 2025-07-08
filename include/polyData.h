/**
 * @file polyData.h
 * @author Sagar Malik (github - maliksagar96)
 * @brief A library to read a polydata VTK file.
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

/**
 * @class polyReader
 * @brief A lightweight wrapper for reading `.vtk` PolyData files using VTK.
 */
class polyReader {

public:
  /**
   * @brief Constructor that initializes the reader with a file.
   * @param filename Path to the input VTK PolyData file.
   */
  polyReader(const std::string& filename);

  /**
   * @brief Destructor.
   */
  ~polyReader() = default;

  /**
   * @brief Initializes and reads the input file.
   * @param filename Path to the VTK file.
   */
  void init(const std::string& filename);

  /**
   * @brief Reads and stores all point coordinates into `points`.
   */
  void read_points();

  /**
   * @brief Reads and stores cell connectivity into `connectivity`.
   */
  void read_connectivity();

  /**
   * @brief Reads scalar field values by name.
   * @param name Name of the scalar field.
   * @return A vector of scalar values.
   */
  std::vector<double> read_scalar(const std::string& name);

  /**
   * @brief Reads vector field values by name.
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
   * @brief Returns the total number of points.
   * @return Number of points in the mesh.
   */
  int get_numpoints() const;

  /**
   * @brief Returns the total number of cells.
   * @return Number of cells in the mesh.
   */
  int get_numcells() const;

private:
  std::string filename; ///< Path to the input VTK file.
  vtkSmartPointer<vtkPolyDataReader> reader; ///< VTK reader object.
  vtkPolyData* polyData = nullptr; ///< Raw polydata pointer.

  std::vector<double> points; ///< Flattened vector of point coordinates.
  std::vector<double> connectivity; ///< Flattened vector of cell connectivity.
};

#endif
