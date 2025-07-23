# 🔍 VTK Reader Library

A lightweight C++ library for reading `.vtk` files (both `vtkPolyData` and `vtkUnstructuredGrid`) using [VTK](https://vtk.org/).  
This tool extracts geometry (points, connectivity) and field data (scalars/vectors), making it useful for scientific visualization, electromagnetics, CFD, and FEM post-processing.

---

## 📦 Features

- ✅ Read `.vtk` PolyData and UnstructuredGrid formats
- ✅ Extract:
  - Point coordinates
  - Cell connectivity
  - Scalar fields (e.g., pressure, temperature)
  - Vector fields (e.g., electric field, velocity)
- ✅ List all available scalar/vector field names
- ✅ Built using modern C++ (RAII, smart pointers)

---

## 📁 What's Inside?

### 🔹 `polyReader.h` / `polyReader.cpp`

- Reads `vtkPolyData` files (usually used for surface meshes like `.stl.vtk`)
- Functions:
  - `read_points()` – Get coordinates of mesh points
  - `read_connectivity()` – Get triangle/line connectivity
  - `read_scalar()` / `read_vector()` – Access field data
  - `get_scalar_names()` / `get_vector_names()` – Explore what's in the file

### 🔹 `vtkReader.h` / `vtkReader.cpp`

- Reads `vtkUnstructuredGrid` files (typically volume meshes from FEA/CFD)
- Similar interface as `polyReader`, but suited for volume elements

### 🔹 `main.cpp`

- A sample test program showing how to use the library

---

## 🧱 Project Structure

VTK-reader/
├── include/
│ ├── polyData.h
│ └── vtkReader.h
├── src/
│ ├── main.cpp
│ ├── polyData.cpp
│ └── vtkReader.cpp
├── CMakeLists.txt
└── README.md

---

## ⚙️ Dependencies

- [VTK](https://vtk.org/) (tested with 9.x)
- CMake ≥ 3.10
- C++11 or later

### ✅ Install VTK (Ubuntu)

```bash
sudo apt update
sudo apt install libvtk9-dev

# Clone this repository
git clone https://github.com/maliksagar96/VTK-reader.git
cd VTK-reader

# Create build directory
mkdir build && cd build

# Run CMake
cmake ..

# Build the project
make



🧪 Sample Usage

Here’s a sample program to demonstrate reading a .vtk PolyData file:

#include <iostream>
#include <memory>
#include "vtkReader.h"
#include "polyData.h"

using namespace std;

int main() {
  unique_ptr<polyReader> sampleFile = make_unique<polyReader>("../src/ref_surface.vtk");

  sampleFile->read_points();
  sampleFile->read_connectivity();

  vector<double> ex_imag = sampleFile->read_scalar("ElectricFieldXImag");
  vector<string> scalar_names = sampleFile->get_scalar_names();

  for (auto scalar : scalar_names) {
    cout << scalar << endl;
  }

  return 0;
}
```
