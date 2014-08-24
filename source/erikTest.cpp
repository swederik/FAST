#include "Importers/MetaImageImporter.hpp"
//#include "VTKImageImporter.hpp"

#include "Streamers/MetaImageStreamer.hpp"

#include "Algorithms/GaussianSmoothingFilter/GaussianSmoothingFilter.hpp"
#include "Algorithms/SurfaceExtraction/SurfaceExtraction.hpp"

#include "Exporters/CTMSurfaceExporter.hpp"
#include "Exporters/MetaImageExporter.hpp"

#include "SimpleWindow.hpp"
#include "SurfaceRenderer.hpp"

//#include "DeviceManager.hpp"

using namespace fast;
 
int main()
{

std::string FAST_TEST_DATA_DIR = "/Code/FAST/TestData/";
//std::string STREAMING_MODE_PROCESS_ALL_FRAMES = "1";
//VTKImageImporter::pointer image = VTKImageImporter::New();
//image->setFilenameFormat(std::string(FAST_TEST_DATA_DIR)+"Vertebrae.mhd");
//image->setStreamingMode(STREAMING_MODE_PROCESS_ALL_FRAMES);
//DeviceManager& deviceManager = DeviceManager::getInstance();
//ExecutionDevice::pointer device = deviceManager.getOneCPUDevice();

//MetaImageImporter::pointer mhdImporter = MetaImageImporter::New();
//mhdImporter->setFilename(std::string(FAST_TEST_DATA_DIR)+"/Vertebrae.mhd");
//mhdImporter->setFilename(std::string(FAST_TEST_DATA_DIR)+"/US-3Dt/US-3Dt_1.mhd");
//mhdImporter->setDevice(device);

MetaImageStreamer::pointer mhdStreamer = MetaImageStreamer::New();
mhdStreamer->setFilenameFormat(std::string(FAST_TEST_DATA_DIR)+"/US-3Dt/US-3Dt_#.mhd");
//mhdStreamer->setStreamingMode(STREAMING_MODE_PROCESS_ALL_FRAMES);
mhdStreamer->setDevice(Host::New());

/*SurfaceExtraction::pointer extractor = SurfaceExtraction::New();
extractor->setInput(mhdImporter->getOutput());
extractor->setThreshold(200);

/*CTMSurfaceExporter::pointer surface = CTMSurfaceExporter::New();
surface->setInput(extractor->getOutput());
surface->setFilename(std::string("/Code/FAST/TestData/FAST_output.ctm"));*/
//std::string AZR = "/Code/FAST/TestData/";
//extractor->update();

GaussianSmoothingFilter::pointer filter = GaussianSmoothingFilter::New();
filter->setInput(mhdStreamer->getOutput());
filter->setMaskSize(5);
filter->setStandardDeviation(2.0);

// Export image
MetaImageExporter::pointer exporter = MetaImageExporter::New();
exporter->setFilename("Erik_MetaImageExporterTest3D.mhd");
exporter->setInput(filter->getOutput());
exporter->update();
std::cout << "---- Ran Erik's Test ----" << std::endl;

SurfaceExtraction::pointer extractor = SurfaceExtraction::New();
extractor->setInput(filter->getOutput());
extractor->setThreshold(200);
extractor->update();
std::cout << "---- Finished Surface Extraction ----" << std::endl;

/*SurfaceRenderer::pointer renderer = SurfaceRenderer::New();
renderer->setInput(extractor->getOutput());

SimpleWindow::pointer window = SimpleWindow::New();
window->addRenderer(renderer);
window->setTimeout(10*1000); // timeout after 10 seconds
window->runMainLoop();*/
}
