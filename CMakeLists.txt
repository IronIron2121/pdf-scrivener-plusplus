# CMakeList.txt : CMake project for pdf_scrivener
#
cmake_minimum_required (VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("pdf_scrivener")

# Add source to this project's executable.
add_executable (pdf_scrivener "src/pdf_scrivener.cpp" "src/pdf_scrivener.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET pdf_scrivener PROPERTY CXX_STANDARD 20)
endif()

# include wxWidgets for GUI stuff
find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)
include(${wxWidgets_USE_FILE})

# include PDF-Writer (dependency for pdf-text-extraction)
include(FetchContent)
FetchContent_Declare(
  PDFHummus
  GIT_REPOSITORY https://github.com/galkahana/PDF-Writer.git
  GIT_TAG        4.5
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(PDFHummus)

include(FetchContent)
FetchContent_Declare(
  TextExtraction
  GIT_REPOSITORY https://github.com/galkahana/pdf-text-extraction.git
  GIT_TAG master
)
FetchContent_MakeAvailable(TextExtraction)

# Now linking both libraries with keyword syntax.
target_link_libraries(pdf_scrivener PRIVATE ${wxWidgets_LIBRARIES} TextExtraction)

# install targets if needed
install(TARGETS pdf_scrivener DESTINATION bin) 

# CPack configuration for packaging the project
set(CPACK_PACKAGE_VENDOR "Gabriel")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "It scrivens the PDFs without memory leaks, or it gets the hose again")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dist")

include(CPack)
