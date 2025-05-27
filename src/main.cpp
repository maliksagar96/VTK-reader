#include <iostream> 
#include "vtkReader.h"

using namespace std;

int main() {

  VTKReader* twoElements = new VTKReader("quad.vtk");

  // twoElements->read_points();

  delete twoElements;

  return 0;
}