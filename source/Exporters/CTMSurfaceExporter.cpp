#include "CTMSurfaceExporter.hpp"
//#include <QImage>
#include "Exception.hpp"
#include "Utility.hpp"
#include "Surface.hpp"
//#include "openctm.h"

namespace fast {

void CTMSurfaceExporter::setInput(Surface::pointer surface) {
    mInput = surface;
    setParent(mInput);
    mIsModified = true;
}

void CTMSurfaceExporter::setFilename(std::string filename) {
    mFilename = filename;
    mIsModified = true;
}

CTMSurfaceExporter::CTMSurfaceExporter() {
    mFilename = "";
    mIsModified = true;
}

void MySaveFile(CTMuint aVertCount, CTMuint aTriCount,
    CTMfloat * aVertices, CTMuint * aIndices,
    const char * aFileName)
{
    try
    {
        // Create a new OpenCTM exporter object
        CTMexporter ctm;
        // Define our mesh representation to OpenCTM
        ctm.DefineMesh(aVertices, aVertCount, aIndices,
        aTriCount, NULL);
        // Save the OpenCTM file
        ctm.Save(aFileName);
    }
    catch(exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void CTMSurfaceExporter::execute() {
    if(!mInput.isValid())
        throw Exception("No input surface given to CTMSurfaceExporter");

    if(mFilename == "")
        throw Exception("No filename given to CTMSurfaceExporter");

    Surface::pointer input;

    input = mInput;
    aVertCount = input->getNrOfTriangles();
    

    // From OpenCTM C++ Exporter example
    try
    {
        // Create a new OpenCTM exporter object
        CTMexporter ctm;
        // Define our mesh representation to OpenCTM
        ctm.DefineMesh(aVertices, aVertCount, aIndices, aTriCount, aNormals);
        // Save the OpenCTM file
        ctm.Save(mFileName);
    }
    catch(exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

}

}; // end namespace fast
