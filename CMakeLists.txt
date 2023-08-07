cmake_minimum_required (VERSION 3.20)

project ("pdf_scrivener")

# include FLTK targets
include(/usr/local/share/fltk/FLTK-Targets.cmake)

# ----------------- GET ICU CONFIGURATION --------------- #
# ICU...Unicode stuff. We'll probably remove this
execute_process(COMMAND icu-config --cppflags OUTPUT_VARIABLE ICU_INCLUDE_DIRS)
execute_process(COMMAND icu-config --ldflags OUTPUT_VARIABLE ICU_LIBRARIES)
# strip leading and trialing whitespace from output
string(STRIP ${ICU_INCLUDE_DIRS} ICU_INCLUDE_DIRS)
string(STRIP ${ICU_LIBRARIES} ICU_LIBRARIES)

# add executable with all source files
add_executable (pdf_scrivener 
    "src/pdf_scrivener.cpp"
    "src/AppWizard.cpp"
    "src/ChoicePage.cpp"
    "src/OpenPDFPage.cpp"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET pdf_scrivener PROPERTY CXX_STANDARD 20)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(POPPLER_CPP REQUIRED IMPORTED_TARGET poppler-cpp) # PDF stuff

target_include_directories(pdf_scrivener PRIVATE ${ICU_INCLUDE_DIRS})

# Link the FLTK libraries to your executable
target_link_libraries(pdf_scrivener PkgConfig::POPPLER_CPP ${ICU_LIBRARIES} fltk)

# Install targets if needed
install(TARGETS pdf_scrivener DESTINATION bin)

# CPack configuration for packaging the project
set(CPACK_PACKAGE_VENDOR "Gabriel")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "It scrivens the PDFs without memory leaks, or it gets the hose again")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dist")

include(CPack)
