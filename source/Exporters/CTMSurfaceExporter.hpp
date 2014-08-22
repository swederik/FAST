#ifndef CTMSurfaceExporter_HPP_
#define CTMSurfaceExporter_HPP_

#include "ProcessObject.hpp"
#include "Surface.hpp"
#include <string>

namespace fast {

class CTMSurfaceExporter : public ProcessObject {
    FAST_OBJECT(CTMSurfaceExporter)
    public:
        void setInput(Surface::pointer surface);
        void setFilename(std::string filename);
    private:
        CTMSurfaceExporter();
        void execute();

        std::string mFilename;
        Surface::pointer mInput;

};


} // end namespace fast




#endif /* CTMSurfaceExporter */
