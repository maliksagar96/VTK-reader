#include <iostream> 
#include <memory>
#include "vtkReader.h"

using namespace std;

int main() {

  unique_ptr<VTKReader> twoElements = make_unique<VTKReader>("../src/quad.vtk");
  twoElements->read_points();
  twoElements->read_connectivity();

  

  return 0;
}