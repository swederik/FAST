#include "ImageImporter.hpp"
#include <QImage>
#include "DataTypes.hpp"
#include "DeviceManager.hpp"
#include "Exception.hpp"
using namespace fast;

void ImageImporter::execute() {
    if(mFilename == "")
        throw Exception("No filename was supplied to the ImageImporter");

    // Load image from disk using Qt
    QImage image;
    std::cout << "Trying to load image..." << std::endl;
    if(!image.load(mFilename.c_str())) {
        throw FileNotFoundException(mFilename);
    }
    std::cout << "Loaded image with size " << image.width() << " "  << image.height() << std::endl;

    // Convert image to make sure color tables are not used
    QImage convertedImage = image.convertToFormat(QImage::Format_RGB32);

    // Get pixel data
    const unsigned char * pixelData = convertedImage.constBits();
    // The pixel data array should contain one uchar value for the
    // R, G, B, A components for each pixel

    // TODO: do some conversion to requested output format, also color vs. no color
    float * convertedPixelData = new float[image.width()*image.height()];
    for(int i = 0; i < image.width()*image.height(); i++) {
        // Converted to grayscale
        convertedPixelData[i] = (1.0f/255.0f)*(float)(pixelData[i*4]+pixelData[i*4+1]+pixelData[i*4+2])/3.0f;
    }

    // Transfer to texture(if OpenCL) or copy raw pixel data (if host)
    mOutput->create2DImage(image.width(),
            image.height(),
            TYPE_FLOAT,
            1,
            mDevice,
            convertedPixelData);
    delete[] convertedPixelData;

    mOutput->updateModifiedTimestamp();
}

ImageImporter::ImageImporter() {
    mOutput = Image::New();
    mDevice = DeviceManager::getInstance().getDefaultComputationDevice();
    mIsModified = true;
}

Image::pointer ImageImporter::getOutput() {
    mOutput->setSource(mPtr.lock());
    return mOutput;
}

void ImageImporter::setFilename(std::string filename) {
    mFilename = filename;
    mIsModified = true;
}

void ImageImporter::setDevice(ExecutionDevice::pointer device) {
    mDevice = device;
    mIsModified = true;
}
