cmake_minimum_required( VERSION 3.5.1 )

project(lab_4_mosaic)

# Compiler flags
if (MSVC)
  set(compiler_options /W4)
  add_definitions(/D_USE_MATH_DEFINES)
else ()
  set(compiler_options -Wall -Wextra -Wpedantic)
endif (MSVC)

# Add libraries.
find_package(OpenCV 4 REQUIRED)
find_package(Eigen3 3.3.4 REQUIRED)

add_executable(${PROJECT_NAME}
  main.cpp
  feature_utils.h
  feature_utils.cpp
  homography_estimator.h
  homography_estimator.cpp
  lab_4.h
  lab_4.cpp
  )

target_link_libraries(${PROJECT_NAME}
  ${OpenCV_LIBS}
  Eigen3::Eigen
  )

target_compile_options(${PROJECT_NAME} PRIVATE ${compiler_options})
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 14
  )
