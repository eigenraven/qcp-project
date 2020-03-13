cmake_minimum_required (VERSION 3.10)
project(QCP_SIMULATOR)

set (CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(external/googletest-release-1.10.0 EXCLUDE_FROM_ALL)
add_subdirectory(external/GSL-2.1.0 EXCLUDE_FROM_ALL)
add_subdirectory(external/cpp-httplib-0.5.6 EXCLUDE_FROM_ALL)
add_subdirectory(external/benchmark-1.5.0 EXCLUDE_FROM_ALL)

include(GoogleTest)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

enable_testing()

if ( CMAKE_VERSION VERSION_GREATER "3.5" )
  set(ENABLE_CLANG_TIDY OFF CACHE BOOL "Add clang-tidy automatically to builds")
  if (ENABLE_CLANG_TIDY)
    find_program (CLANG_TIDY_EXE NAMES "clang-tidy" "clang-tidy-9")
    if (CLANG_TIDY_EXE)
      message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
      set(CLANG_TIDY_CHECKS "*,modernize-*,-modernize-use-trailing-return-type")
      set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=${CLANG_TIDY_CHECKS};-header-filter='${CMAKE_SOURCE_DIR}/*'"
        CACHE STRING "" FORCE)
    else()
      message(AUTHOR_WARNING "clang-tidy not found!")
      set(CMAKE_CXX_CLANG_TIDY "" CACHE STRING "" FORCE) # delete it
    endif()
  endif()
endif()

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS LIST_DIRECTORIES false src/*.cpp)
file(GLOB_RECURSE driver_cmd_files CONFIGURE_DEPENDS LIST_DIRECTORIES false drivers/cmd/*.cpp)
file(GLOB_RECURSE driver_http_files CONFIGURE_DEPENDS LIST_DIRECTORIES false drivers/http/*.cpp)
file(GLOB_RECURSE test_files CONFIGURE_DEPENDS LIST_DIRECTORIES false tests/*.cpp)
file(GLOB_RECURSE benchmark_files CONFIGURE_DEPENDS LIST_DIRECTORIES false benchmarks/*.cpp)

add_subdirectory(docs)

add_library(qcsim_common ${source_files})
target_link_libraries(qcsim_common PUBLIC GSL)
target_include_directories(qcsim_common PUBLIC src/)
target_compile_features(qcsim_common PUBLIC cxx_std_17)

add_executable(test_qcsim ${test_files})
target_link_libraries(test_qcsim qcsim_common gtest gmock gtest_main)
gtest_discover_tests(test_qcsim
  WORKING_DIRECTORY ${PROJECT_DIR}
  PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}")

add_executable(bench_qcsim ${benchmark_files})
target_link_libraries(bench_qcsim qcsim_common benchmark benchmark_main)

add_executable(qcsim ${driver_cmd_files})
target_link_libraries(qcsim qcsim_common)

add_executable(http_qcsim ${driver_http_files})
target_link_libraries(http_qcsim qcsim_common httplib Threads::Threads)
