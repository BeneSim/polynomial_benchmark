cmake_minimum_required(VERSION 3.1)

project(polynomial_benchmark)

find_package(xtl REQUIRED CONFIG)
find_package(xsimd REQUIRED CONFIG)
find_package(xtensor REQUIRED CONFIG)
find_package(xtensor-python REQUIRED CONFIG)

find_package(Eigen3 REQUIRED CONFIG)

find_package(pybind11 REQUIRED CONFIG)
find_package(NumPy REQUIRED)

option(USE_MARCH_NATIVE "Use -march=native" OFF)
option(USE_XSIMD "Use xsimd library" OFF)
option(USE_LAMBDA_XFUNCTION "Use make_lambda_xfunction" ON)


add_library(polynomial INTERFACE)
target_include_directories(polynomial INTERFACE include)
target_link_libraries(polynomial
  INTERFACE xtensor Eigen3::Eigen $<$<BOOL:${USE_XSIMD}>:xsimd>)
target_compile_definitions(polynomial
  INTERFACE $<$<BOOL:${USE_XSIMD}>:XTENSOR_USE_XSIMD>
  INTERFACE $<$<BOOL:${USE_LAMBDA_XFUNCTION}>:USE_LAMBDA_XFUNCTION>)
target_compile_options(polynomial
  INTERFACE $<$<BOOL:${USE_MARCH_NATIVE}>:-march=native>)
target_compile_features(polynomial INTERFACE cxx_std_17)

add_library(py_polynomial SHARED src/python_bindings.cpp)
target_link_libraries(py_polynomial
  PUBLIC polynomial xtensor-python pybind11::pybind11)
target_include_directories(py_polynomial PUBLIC ${NUMPY_INCLUDE_DIRS})
set_target_properties(py_polynomial PROPERTIES
  PREFIX "${PYTHON_MODULE_PREFIX}")
set_target_properties(py_polynomial PROPERTIES
  SUFFIX "${PYTHON_MODULE_EXTENSION}")

configure_file(src/benchmark.py ${CMAKE_CURRENT_BINARY_DIR}/benchmark.py
  COPYONLY)

add_custom_target(benchmark
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/benchmark.py
  $<$<BOOL:${USE_XSIMD}>:--with-xsimd>
  $<$<BOOL:${USE_MARCH_NATIVE}>:--with-march-native>
  $<$<BOOL:${USE_XSIMD}>:--xsimd-version>
  $<$<BOOL:${USE_XSIMD}>:${xsimd_VERSION}>
  --xtensor-version ${xtensor_VERSION}
  --xtensor-python-version ${xtensor-python_VERSION}
  --eigen-version ${Eigen3_VERSION}
  --pybind11-version ${pybind11_VERSION}
  $<$<BOOL:${USE_LAMBDA_XFUNCTION}>:--output-prefix>
  $<$<BOOL:${USE_LAMBDA_XFUNCTION}>:"lambda_">
  DEPENDS py_polynomial)
