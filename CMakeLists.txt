cmake_minimum_required (VERSION 3.20)

project ("pdf_scrivener")

# Include the FLTK targets file
include(/usr/local/share/fltk/FLTK-Targets.cmake)

# Add source to this project's executable.
add_executable (pdf_scrivener "src/pdf_scrivener.cpp" "src/pdf_scrivener.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET pdf_scrivener PROPERTY CXX_STANDARD 20)
endif()

# Include dependencies
include(FetchContent)
FetchContent_Declare(
  PDFHummus
  GIT_REPOSITORY https://github.com/galkahana/PDF-Writer.git
  GIT_TAG        master
) 
FetchContent_MakeAvailable(PDFHummus)

include(FetchContent)  
FetchContent_Declare(
  TextExtraction
  GIT_REPOSITORY https://github.com/galkahana/pdf-text-extraction.git
  GIT_TAG master
)
FetchContent_MakeAvailable(TextExtraction)

# Link the FLTK libraries to your executable
target_link_libraries(pdf_scrivener fltk TextExtraction)

# Install targets if needed
install(TARGETS pdf_scrivener DESTINATION bin) 

# CPack configuration for packaging the project
set(CPACK_PACKAGE_VENDOR "Gabriel")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "It scrivens the PDFs without memory leaks, or it gets the hose again")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dist")

include(CPack)