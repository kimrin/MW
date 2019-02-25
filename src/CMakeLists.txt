CMAKE_MINIMUM_REQUIRED(VERSION 3.7 FATAL_ERROR)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/pybind11/tools)

set(CMAKE_FIND_FRAMEWORK NEVER)
project(mw)

# compiler must be 11 or 14
set(CMAKE_CXX_STANDARD 14)

include(pybind11Tools)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Specify build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
  set(CPYTHON_BUILD_TYPE Debug)
else ()
  set(CPYTHON_BUILD_TYPE Release)
endif()

# Add the cpython as an external project that will be included in the build
if(MSVC)
  if(CMAKE_CL_64)
    set(CPYTHON_PLATFORM x64)
    set(CPYTHON_BUILD_DIR amd64)
  else()
    set(CPYTHON_PLATFORM x86)
    set(CPYTHON_BUILD_DIR win32)
  endif()
  ExternalProject_Add(CPYTHON
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/cpython
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cd ${CMAKE_SOURCE_DIR}/cpython && MSBuild.exe /p:Configuration=${CPYTHON_BUILD_TYPE} /property:Platform=${CPYTHON_PLATFORM} "PCBuild/python.vcxproj" /nologo /verbosity:minimal /consoleloggerparameters:summar
    INSTALL_COMMAND ""
    TEST_COMMAND ""
  )
else()
  ExternalProject_Add(CPYTHON
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/cpython
    CONFIGURE_COMMAND cd ${CMAKE_SOURCE_DIR}/cpython && ./configure --disable-shared
    BUILD_COMMAND cd ${CMAKE_SOURCE_DIR}/cpython && make
    INSTALL_COMMAND ""
    TEST_COMMAND ""
  )
endif()

set(CPYTHON_STDLIB_DIR ${CMAKE_SOURCE_DIR}/cpython/Lib)
if(MSVC)
  set(CPYTHON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/cpython/Include ${CMAKE_SOURCE_DIR}/cpython/PC)
  set(CPYTHON_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/cpython/PCBuild/${CPYTHON_BUILD_DIR})
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CPYTHON_BIN ${CMAKE_SOURCE_DIR}/cpython/PCBuild/${CPYTHON_BUILD_DIR}/python38_d.dll)
  else()
    set(CPYTHON_BIN ${CMAKE_SOURCE_DIR}/cpython/PCBuild/${CPYTHON_BUILD_DIR}/python38.dll)
  endif()
else()
  set(CPYTHON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/cpython/Include ${CMAKE_SOURCE_DIR}/cpython)
  set(CPYTHON_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/cpython/Lib)
  set(CPYTHON_LIBRARY python3.8m)
  set(CPYTHON_BIN ${CMAKE_SOURCE_DIR}/cpython/libpython3.8m.a)
endif()


# required if linking to static library
add_definitions(${LLVM_DEFINITIONS})
add_definitions(-DANTLR4CPP_STATIC)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)
# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})

# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/antlr-4.7.2-complete.jar)
# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

antlr_target(SampleGrammarParser JSON.g4 LEXER
             PARSER
             PACKAGE antlr4mw
             LISTENER
             VISITOR)

# include generated files in project environment
include_directories(${LLVM_INCLUDE_DIRS}
                    ${ANTLR_SampleGrammarParser_OUTPUT_DIR})

# add generated grammar to demo binary target
add_executable(${PROJECT_NAME}
               MWMain.cpp
               ${SOURCES}
               ${HEADERS}
               ${ANTLR_SampleGrammarParser_CXX_OUTPUTS})

               # Set the executable to console application if MSVC
if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()

# Copy Python DLL to the build folder if different
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  -E copy_if_different ${CPYTHON_BIN} $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Copy our python sources to the build folder
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/app $<TARGET_FILE_DIR:${PROJECT_NAME}>/app
)

# Copy the Python stdlib into the build folder (needed by the embedded python)
add_custom_command(
  TARGET ${PROJECT_NAME} 
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} 
  -E copy_directory ${CPYTHON_STDLIB_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib/python3.8m
)

# Copy the Python stdlib into the build folder (needed by the embedded python)
add_custom_command(
  TARGET ${PROJECT_NAME} 
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} 
  -E copy_directory ${CMAKE_SOURCE_DIR}/cpython/build/lib.macosx-10.14-x86_64-3.8/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib/python3.8m/lib-dylib
)

add_subdirectory(pybind11)
set(PYBIND_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/pybind11/include)

# Source and header files
FILE(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
FILE(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

# Linking directories
link_directories(${CPYTHON_LIBRARY_DIR})

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(${PROJECT_NAME}
                      antlr4_static
                      pybind11::embed
                      ${llvm_libs})

# Add macros
target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX=1)

# Dependencies
# add_dependencies(${PROJECT_NAME} PYBIND)
add_dependencies(${PROJECT_NAME} CPYTHON)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CPYTHON_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PYBIND_INCLUDE_DIR})


