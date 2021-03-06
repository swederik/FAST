#include "MetaImageImporter.hpp"
#include "DeviceManager.hpp"
#include "Exception.hpp"
#include <fstream>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using namespace fast;

Image::pointer MetaImageImporter::getOutput() {
    mOutput->setSource(mPtr.lock());
    return mOutput;
}

void MetaImageImporter::setFilename(std::string filename) {
    mFilename = filename;
    mIsModified = true;
}

void MetaImageImporter::setDevice(ExecutionDevice::pointer device) {
    mDevice = device;
    mIsModified = true;
}

MetaImageImporter::MetaImageImporter() {
    mDevice = DeviceManager::getInstance().getDefaultComputationDevice();
    mFilename = "";
    mOutput = Image::New();
    mIsModified = true;
}

std::vector<std::string> stringSplit(std::string str, std::string delimiter) {
    std::vector<std::string> values;
    int startPosition = 0;
    int endPosition = str.find(delimiter, startPosition+1);
    while(true) {
        if(endPosition == std::string::npos) {
            endPosition = str.size();
            values.push_back(str.substr(startPosition, endPosition-startPosition));
            break;
        } else {
            values.push_back(str.substr(startPosition, endPosition-startPosition));
            startPosition = endPosition+1;
            endPosition = str.find(delimiter, startPosition+1);
        }
    }
    return values;
}

template <class T>
void * readRawData(std::string rawFilename, unsigned int width, unsigned int height, unsigned int depth, unsigned int nrOfComponents) {
    boost::iostreams::mapped_file_source file;
    file.open(rawFilename, width*height*depth*nrOfComponents*sizeof(T));
    if(!file.is_open())
        throw FileNotFoundException(rawFilename);
    T * data = new T[width*height*depth*nrOfComponents];
    T * fileData = (T*)file.data();
    memcpy(data,fileData,width*height*depth*nrOfComponents*sizeof(T));
    file.close();
    return data;
}

void MetaImageImporter::execute() {
    if(mFilename == "")
        throw Exception("Filename was not set in MetaImageImporter");

    // Open and parse mhd file
    std::fstream mhdFile;
    mhdFile.open(mFilename.c_str(), std::fstream::in);
    if(!mhdFile.is_open())
        throw FileNotFoundException(mFilename);
    std::string line;
    std::string rawFilename;
    bool sizeFound = false,
         rawFilenameFound = false,
         typeFound = false,
         dimensionsFound = false;
    std::string typeName;
    //this->spacing = float3(1.0f,1.0f,1.0f);

    // Find NDims first
    bool imageIs3D = false;
    do {
        std::getline(mhdFile, line);
        if(line.substr(0, 5) == "NDims") {
            if(line.substr(5+3, 1) == "3") {
                imageIs3D = true;
            } else if(line.substr(5+3, 1) == "2") {
                imageIs3D = false;
            }
            dimensionsFound = true;
        }
    } while(!mhdFile.eof() && !dimensionsFound);

    if(!dimensionsFound)
        throw Exception("NDims not found in metaimage file.");

    // Reset and start reading file from beginning
    mhdFile.seekg(0);

    unsigned int width, height, depth = 1;
    unsigned int nrOfComponents = 1;
    Image::pointer output = mOutput;

    do{
        std::getline(mhdFile, line);
        boost::trim(line);
        if(!mhdFile.eof()) {
            int firstSpace = line.find(" ");
            std::string key = line.substr(0, firstSpace);
            std::string value = line.substr(firstSpace+3);
            //this->setAttribute(key, value);
        }
        if(line.substr(0, 7) == "DimSize") {
            std::string sizeString = line.substr(7+3);
            std::string sizeX = sizeString.substr(0,sizeString.find(" "));
            sizeString = sizeString.substr(sizeString.find(" ")+1);
            std::string sizeY = sizeString.substr(0,sizeString.find(" "));
            width = boost::lexical_cast<int>(sizeX.c_str());
            height = boost::lexical_cast<int>(sizeY.c_str());
            if(imageIs3D) {
                sizeString = sizeString.substr(sizeString.find(" ")+1);
                std::string sizeZ = sizeString.substr(0,sizeString.find(" "));
                depth = boost::lexical_cast<int>(sizeZ.c_str());
            }

            sizeFound = true;
        } else if(line.substr(0, 15) == "ElementDataFile") {
            rawFilename = line.substr(15+3);
            rawFilenameFound = true;

            // Remove any trailing spaces
            int pos = rawFilename.find(" ");
            if(pos > 0)
            rawFilename = rawFilename.substr(0,pos);

            // Get path name
            pos = mFilename.rfind('/');
            if(pos > 0)
                rawFilename = mFilename.substr(0,pos+1) + rawFilename;
        } else if(line.substr(0, 11) == "ElementType") {
            typeFound = true;
            typeName = line.substr(11+3);

            // Remove any trailing spaces
            int pos = typeName.find(" ");
            if(pos > 0)
            typeName = typeName.substr(0,pos);

            if(typeName == "MET_SHORT") {
            } else if(typeName == "MET_USHORT") {
            } else if(typeName == "MET_CHAR") {
            } else if(typeName == "MET_UCHAR") {
            } else if(typeName == "MET_INT") {
            } else if(typeName == "MET_UINT") {
            } else if(typeName == "MET_FLOAT") {
            } else {
                throw Exception("Trying to read volume of unsupported data type", __LINE__, __FILE__);
            }
        } else if(line.substr(0,23) == "ElementNumberOfChannels") {
            nrOfComponents = boost::lexical_cast<int>(line.substr(23+3).c_str());
            if(nrOfComponents <= 0)
                throw Exception("Error in reading the number of components in the MetaImageImporter");
        } else if(line.substr(0, 14) == "ElementSpacing") {
            std::string sizeString = line.substr(14+3);
            boost::trim(sizeString);
            std::string sizeX = sizeString.substr(0,sizeString.find(" "));
            sizeString = sizeString.substr(sizeString.find(" ")+1);
            std::string sizeY = sizeString.substr(0,sizeString.find(" "));
            std::string sizeZ = "1";
            if(imageIs3D) {
                sizeString = sizeString.substr(sizeString.find(" ")+1);
                sizeZ = sizeString.substr(0,sizeString.find(" "));
            }

            Float<3> spacing;
            spacing[0] = boost::lexical_cast<float>(sizeX.c_str());
            spacing[1] = boost::lexical_cast<float>(sizeY.c_str());
            spacing[2] = boost::lexical_cast<float>(sizeZ.c_str());
            output->setSpacing(spacing);
        } else if(line.substr(0, 16) == "CenterOfRotation") {
            std::string sizeString = line.substr(16+3);
            boost::trim(sizeString);
            std::string sizeX = sizeString.substr(0,sizeString.find(" "));
            sizeString = sizeString.substr(sizeString.find(" ")+1);
            std::string sizeY = sizeString.substr(0,sizeString.find(" "));
            sizeString = sizeString.substr(sizeString.find(" ")+1);
            std::string sizeZ = sizeString.substr(0,sizeString.find(" "));

            Float<3> centerOfRotation;
            centerOfRotation[0] = boost::lexical_cast<float>(sizeX.c_str());
            centerOfRotation[1] = boost::lexical_cast<float>(sizeY.c_str());
            centerOfRotation[2] = boost::lexical_cast<float>(sizeZ.c_str());
            output->setCenterOfRotation(centerOfRotation);
        } else if(line.substr(0, 6) == "Offset") {
            std::string sizeString = line.substr(6+3);
            boost::trim(sizeString);
            std::string sizeX = sizeString.substr(0,sizeString.find(" "));
            sizeString = sizeString.substr(sizeString.find(" ")+1);
            std::string sizeY = sizeString.substr(0,sizeString.find(" "));
            sizeString = sizeString.substr(sizeString.find(" ")+1);
            std::string sizeZ = sizeString.substr(0,sizeString.find(" "));

            Float<3> offset;
            offset[0] = boost::lexical_cast<float>(sizeX.c_str());
            offset[1] = boost::lexical_cast<float>(sizeY.c_str());
            offset[2] = boost::lexical_cast<float>(sizeZ.c_str());
            output->setOffset(offset);
        } else if(line.substr(0, 15) == "TransformMatrix") {
            std::string string = line.substr(15+3);
            boost::trim(string);

            std::vector<std::string> values = stringSplit(string, " ");
            if(values.size() != 9)
                throw Exception("Encountered a transform matrix with incorrect number of elements in the MetaImageImporter");

            Float<9> matrix;
            for(unsigned int i = 0; i < 9; i++) {
                matrix[i] = boost::lexical_cast<float>(values[i].c_str());
            }
            output->setTransformMatrix(matrix);
        }

    } while(!mhdFile.eof());


    mhdFile.close();
    if(!sizeFound || !rawFilenameFound || !typeFound || !dimensionsFound)
        throw Exception("Error reading the mhd file", __LINE__, __FILE__);

    if(rawFilename.substr(rawFilename.length()-5) == ".zraw")
        throw Exception("Error reading MetaImage. Compressed raw files (.zraw) currently not supported.");

    void * data;
    DataType type;
    if(typeName == "MET_SHORT") {
        type = TYPE_INT16;
        data = readRawData<short>(rawFilename, width, height, depth, nrOfComponents);
    } else if(typeName == "MET_USHORT") {
        type = TYPE_UINT16;
        data = readRawData<unsigned short>(rawFilename, width, height, depth, nrOfComponents);
    } else if(typeName == "MET_CHAR") {
        type = TYPE_INT8;
        data = readRawData<char>(rawFilename, width, height, depth, nrOfComponents);
    } else if(typeName == "MET_UCHAR") {
        type = TYPE_UINT8;
        data = readRawData<unsigned char>(rawFilename, width, height, depth, nrOfComponents);
    } else if(typeName == "MET_FLOAT") {
        type = TYPE_FLOAT;
        data = readRawData<float>(rawFilename, width, height, depth, nrOfComponents);
    }

    if(imageIs3D) {
        output->create3DImage(width,height,depth,type,nrOfComponents,mDevice,data);
    } else {
        output->create2DImage(width,height,type,nrOfComponents,mDevice,data);
    }

    // Clean up
    if(!mDevice->isHost()) {
        deleteArray(data, type);
    }
}
