#include "Importers/MetaImageImporter.hpp"
//#include "VTKImageImporter.hpp"

#include "Streamers/MetaImageStreamer.hpp"

#include "Algorithms/GaussianSmoothingFilter/GaussianSmoothingFilter.hpp"
#include "Algorithms/SurfaceExtraction/SurfaceExtraction.hpp"

#include "Visualization/SurfaceRenderer/SurfaceRenderer.hpp"
#include "Visualization/ImageRenderer/ImageRenderer.hpp"
#include "Visualization/SliceRenderer/SliceRenderer.hpp"

#include "Visualization/VolumeRenderer/VolumeRenderer.hpp"
#include "Visualization/VolumeRenderer/OpacityTransferFunction.hpp"
#include "Visualization/VolumeRenderer/ColorTransferFunction.hpp"

#include "Visualization/SimpleWindow.hpp"

#include "Exporters/CTMSurfaceExporter.hpp"

#include "DeviceManager.hpp"

using namespace fast;
 
int main()
{

std::string FAST_TEST_DATA_DIR = "/Code/FAST/TestData/";
std::string STREAMING_MODE_PROCESS_ALL_FRAMES = "1";
//VTKImageImporter::pointer image = VTKImageImporter::New();
//image->setFilenameFormat(std::string(FAST_TEST_DATA_DIR)+"Vertebrae.mhd");
//image->setStreamingMode(STREAMING_MODE_PROCESS_ALL_FRAMES);
DeviceManager& deviceManager = DeviceManager::getInstance();
ExecutionDevice::pointer device = deviceManager.getOneCPUDevice();

MetaImageImporter::pointer mhdImporter = MetaImageImporter::New();
//mhdImporter->setFilename(std::string(FAST_TEST_DATA_DIR)+"/Vertebrae.mhd");
mhdImporter->setFilename(std::string(FAST_TEST_DATA_DIR)+"/US-3Dt/US-3Dt_1.mhd");
mhdImporter->setDevice(device);

MetaImageStreamer::pointer mhdstreamer = MetaImageStreamer::New();
mhdstreamer->setFilenameFormat(std::string(FAST_TEST_DATA_DIR)+"/US-3Dt/US-3Dt_#.mhd");
//mhdstreamer->setStreamingMode(STREAMING_MODE_PROCESS_ALL_FRAMES);
mhdstreamer->setDevice(Host::New());


/*GaussianSmoothingFilter::pointer filter = GaussianSmoothingFilter::New();
filter->setInput(mhdstreamer->getOutput());
filter->setMaskSize(5);
filter->setStandardDeviation(2.0);*/

SurfaceExtraction::pointer extractor = SurfaceExtraction::New();
extractor->setInput(mhdstreamer->getOutput());
extractor->setThreshold(200);

//CTMSurfaceExporter::pointer surface = CTMSurfaceExporter::New();
//surface->setInput(extractor->getOutput());
//surface->setFilename(std::string("/Code/FAST/TestData/FAST_output.ctm"));
//std::string AZR = "/Code/FAST/TestData/";

// Render filtered image
SliceRenderer::pointer renderer = SliceRenderer::New();
renderer->setInput(mhdstreamer->getOutput());
SimpleWindow::pointer window = SimpleWindow::New();
window->addRenderer(renderer);
window->setTimeout(100*1000*1000);
//window->runMainLoop();

//SurfaceRenderer::pointer renderer2 = SurfaceRenderer::New();


ColorTransferFunction::pointer ctf1 = ColorTransferFunction::New();
ctf1->addRGBPoint(000.0, 1.0, 0.0, 0.0);
ctf1->addRGBPoint(127.0, 0.0, 1.0, 0.0);
ctf1->addRGBPoint(255.0, 0.0, 0.0, 1.0);

OpacityTransferFunction::pointer otf1 = OpacityTransferFunction::New();
otf1->addAlphaPoint(000.0, 0.0);
otf1->addAlphaPoint(255.0, 1.0);

VolumeRenderer::pointer VolumeRenderer = VolumeRenderer::New();
VolumeRenderer->addInput(mhdstreamer->getOutput());
VolumeRenderer->setColorTransferFunction(0, ctf1);
VolumeRenderer->setOpacityTransferFunction(0, otf1);

VolumeRenderer->enableRuntimeMeasurements();
SimpleWindow::pointer window2 = SimpleWindow::New();
window2->setMaximumFramerate(2500);
window2->addRenderer(VolumeRenderer);
window2->runMainLoop();
VolumeRenderer->getRuntime()->print();
}
