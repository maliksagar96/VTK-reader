#include <iostream> 
#include <memory>
#include "vtkReader.h"
#include "polyData.h"


using namespace std;

int main() {

  // unique_ptr<VTKReader> twoElements = make_unique<VTKReader>("../src/00719freq_ref_post_ref_sruface.stl.vtk");
  // twoElements->read_points();
  // twoElements->read_connectivity();

  unique_ptr<polyReader> sampleFile = make_unique<polyReader>("../src/00719freq_ref_post_ref_sruface.stl.vtk");
  sampleFile->read_points();
  sampleFile->read_connectivity();
  vector<double> ex_imag = sampleFile->read_scalar("ElectricFieldXImag");

  cout<<ex_imag[0]<<endl;
  cout<<ex_imag[1]<<endl;
  cout<<ex_imag[2]<<endl;
  
  return 0;
}