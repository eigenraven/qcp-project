find_package(Doxygen)
if(DOXYGEN_FOUND)
    # Find all the public headers
    file(GLOB_RECURSE header_files CONFIGURE_DEPENDS LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.hpp)

    set(DOXYGEN_PROJECT_NAME "Quantum Computer Simulator")

    doxygen_add_docs(docs ${header_files}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()