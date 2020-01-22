cmake_minimum_required (VERSION 3.10)
project(QCP_SIMULATOR)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS LIST_DIRECTORIES false src/*.cpp)

add_executable(qcsim ${source_files})
target_compile_features(qcsim PUBLIC cxx_std_17)
