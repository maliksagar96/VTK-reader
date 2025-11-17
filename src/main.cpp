#include <iostream> 
#include <memory>
#include "vtkReader.h"
#include "polyData.h"
#include <vtkDataSetReader.h>

using namespace std;

std::string detectVTKType(const std::string& filename) {

	vtkSmartPointer<vtkDataSetReader> reader =
			vtkSmartPointer<vtkDataSetReader>::New();

	reader->SetFileName(filename.c_str());
	reader->Update();

	vtkDataSet* data = reader->GetOutput();

	if (!data) return "Unknown";

	if (vtkPolyData::SafeDownCast(data))
			return "PolyData";

	if (vtkUnstructuredGrid::SafeDownCast(data))
			return "UnstructuredGrid";

	return data->GetClassName();   // fallback (StructuredGrid, ImageData, etc.)
}

int main() {

	std::cout << "Reading Fourier Monitor\n";
	
	std::string type = detectVTKType(Fourier_monitor);
	std::cout << "File type = " << type << "\n";

	std::unique_ptr<VTKReader> fft_unstruct_monitor;
	std::unique_ptr<polyReader> fft_poly_monitor;		
	std::vector<double> centers;
	std::vector<double> norms;

	if (type == "PolyData") {
		fft_poly_monitor = std::make_unique<polyReader>(Fourier_monitor);
		fft_poly_monitor->read_points();
		fft_poly_monitor->read_connectivity();
		fft_poly_monitor->read_offset();
		fft_poly_monitor->calc_centroids();
		fft_poly_monitor->calc_normal();

		monitor_points = fft_poly_monitor->getPoints();
		fft_monitor_connectivity = fft_poly_monitor->getConnectivity();
		fft_offsets = fft_poly_monitor->getOffsets();
		centers = fft_poly_monitor->getCentroids();	
		norms = fft_poly_monitor->getNormals();
		fft_monitor_areas = fft_poly_monitor->getAreas();
	}

	else if (type == "UnstructuredGrid") {
		fft_unstruct_monitor = std::make_unique<VTKReader>(Fourier_monitor);
		fft_unstruct_monitor->read_points();
		fft_unstruct_monitor->read_connectivity();
		fft_unstruct_monitor->read_offset();
		fft_unstruct_monitor->calc_centroids();
		fft_unstruct_monitor->calc_normal();

		monitor_points = fft_unstruct_monitor->getPoints();
		fft_monitor_connectivity = fft_unstruct_monitor->getConnectivity();
		fft_offsets = fft_unstruct_monitor->getOffsets();
		centers = fft_unstruct_monitor->getCentroids();	
		norms = fft_unstruct_monitor->getNormals();
		fft_monitor_areas = fft_unstruct_monitor->getAreas();
	}
  
  return 0;
}