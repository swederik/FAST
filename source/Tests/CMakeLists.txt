set(SOURCE_FILES
#    TestFixture.hpp
#    TestFixture.cpp
)

set(EXECUTABLE_SOURCE_FILES
    catch.hpp
    CatchMain.cpp
    DataComparison.cpp
    DataComparison.hpp
    DummyObjects.hpp
    ProcessObjectTests.cpp
    DataObjectTests.cpp
    Algorithms/GaussianSmoothingFilterTests.cpp
    Algorithms/SeededRegionGrowingTests.cpp
    Algorithms/DoubleFilter.cpp
    Algorithms/DoubleFilter.hpp
    Algorithms/DoubleFilterTests.cpp
    Data/ImageTests.cpp
    Data/DynamicImageTests.cpp
    Importers/ImageImporterTests.cpp
    Exporters/ImageExporterTests.cpp
    Importers/MetaImageImporterTests.cpp
    Exporters/MetaImageExporterTests.cpp
    Streamers/MetaImageStreamerTests.cpp
    Streamers/ImageStreamerTests.cpp
    Visualization/ImageRendererTests.cpp
    Visualization/SliceRendererTests.cpp
    Visualization/SurfaceRendererTests.cpp
    SystemTests.cpp
)

include_directories(
    ./
)

if(VTK_INTEROP)
set(EXECUTABLE_SOURCE_FILES
    ${EXECUTABLE_SOURCE_FILES} 
    Importers/VTKImageImporterTests.cpp
    Exporters/VTKImageExporterTests.cpp
)
endif()

if(ITK_INTEROP)
set(EXECUTABLE_SOURCE_FILES
    ${EXECUTABLE_SOURCE_FILES} 
    Importers/ITKImageImporterTests.cpp
    Exporters/ITKImageExporterTests.cpp
)
endif()

set(FAST_LINK_LIBRARIES
    ${FAST}
)

#add_library (TestFAST ${SOURCE_FILES})
#target_link_libraries(TestFAST FAST ${FAST_LINK_LIBRARIES})

#set(TestFAST_LIBRARY
#    TestFAST
#    PARENT_SCOPE
#)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFAST_TEST_DATA_DIR=\\\"${PROJECT_SOURCE_DIR}/../TestData/\\\"")

add_executable (test ${EXECUTABLE_SOURCE_FILES} ${SOURCE_FILES})
target_link_libraries(test FAST ${FAST_LINK_LIBRARIES})

