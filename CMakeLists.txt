project(lcd_tools C CXX)
cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)

include(ExternalProject)
ExternalProject_Add(wiringPiSrc
  GIT_REPOSITORY    https://github.com/WiringPi/WiringPi.git
  BUILD_IN_SOURCE YES
  CONFIGURE_COMMAND pwd && ls -la
  INSTALL_COMMAND ""
  BUILD_COMMAND ./build
)

ExternalProject_Get_Property(wiringPiSrc SOURCE_DIR)

find_package(PythonLibs REQUIRED)

include(FetchContent)
FetchContent_Declare(
  magic_enum
  URL https://github.com/Neargye/magic_enum/archive/refs/tags/v0.9.3.zip
)

FetchContent_MakeAvailable(magic_enum)

include_directories(${SOURCE_DIR}/wiringPi ${SOURCE_DIR}/wiringPiDev)
include_directories(${PYTHON_INCLUDE_DIRS})

link_directories(${SOURCE_DIR}/devLib)

set(target lcd_tools)

add_executable(${target} main.cpp lcd.c pscrypt.cpp)

add_dependencies(${target} wiringPiSrc)
target_link_libraries(${target} wiringPi magic_enum)
target_link_libraries(${target} ${PYTHON_LIBRARIES})

target_link_options(${target} PRIVATE -Wl,--copy-dt-needed-entries)






