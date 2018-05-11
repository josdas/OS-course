cmake_minimum_required(VERSION 3.9)
project(Libs)

set(CMAKE_CXX_STANDARD 14)

# Make static lib
ADD_LIBRARY( StaticLib STATIC static_lib/mul.cpp )

#

SET( APP_EXE LibsTest )
ADD_EXECUTABLE( ${APP_EXE} main.cpp )
TARGET_LINK_LIBRARIES( ${APP_EXE} StaticLib )

# add_executable(Libs main.cpp static_lib/mul.h static_lib/mul.cpp dynamic_lib/sum.h dynamic_lib/sum.cpp dynamic_lib/sub.h dynamic_lib/sub.cpp)