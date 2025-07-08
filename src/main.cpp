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
  vector<string> scalar_names = sampleFile->get_scalar_names();

  for(auto scalar:scalar_names) {
    cout<<scalar<<endl;
  }
  
  return 0;
}