cmake_minimum_required( VERSION 3.24.0 )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED TRUE )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}.. )

add_compile_options( "/MP" "/MD" "/W4" )

project( img )
set( SOURCES
    "src/main.cpp"
    "src/ImageViewer/window.cpp"
    "src/ImageViewer/view.cpp"
    "src/ImageViewer/page.cpp"
    "src/ImageReader/fm.cpp"
    "src/ImageReader/chapter.cpp"
    "src/ImageReader/series.cpp"
)

set( SFML_DIR "C:\\cpp\\Downloads\\Extracted\\XLibs\\PreCompiled\\SFML-2.6.1\\MSVC\\lib\\cmake\\SFML" )
set( LIBZIP_LIB "C:\\cpp\\Downloads\\Extracted\\XLibs\\MachineCompiled\\libzip_msvc\\lib\\zip.lib" )

# set( SFML_STATIC_LIBRARIES TRUE )
find_package( SFML REQUIRED COMPONENTS graphics )

add_executable( ${PROJECT_NAME} ${SOURCES} )
target_include_directories( ${PROJECT_NAME} PRIVATE
    "include"
    "C:\\cpp\\Downloads\\Extracted\\XLibs\\PreCompiled\\SFML-2.6.1\\MSVC\\include"
    "C:\\cpp\\Downloads\\Extracted\\XLibs\\MachineCompiled\\libzip_msvc\\include"
    "C:\\cpp\\Downloads\\Extracted\\XLibs\\PreCompiled\\rapidxml-1.13\\include" 
)
target_link_libraries( ${PROJECT_NAME} 
    sfml-graphics
    ${LIBZIP_LIB}
)