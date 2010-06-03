set (PACKAGE mvthimage)
project (${PACKAGE})
cmake_minimum_required (VERSION 2.6)

option (BUILD_SHARED_LIB "Build Shared Libraries." ON)

# Set up some variables that we can use in our code.
set (MVTHIMAGE_VERSION 1.6)
set (MVTHIMAGE_MAJOR_VERSION 1)
set (MVTHIMAGE_MINOR_VERSION 6)
set (MVTHIMAGE_PATCH_LEVEL 0)
set (TCLSCRIPTDIR ${CMAKE_INSTALL_PREFIX}/lib/${PACKAGE})
set (MVTHIMAGELIB ${CMAKE_INSTALL_PREFIX}/lib/${PACKAGE}/libMvthImage.so)
set (VERSION ${MVTHIMAGE_VERSION})
add_definitions (-DHAVE_CONFIG_H)
include_directories (${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Fix for static library linking to shared library on 64 bit systems:
# with -fPIC
IF(UNIX AND NOT WIN32)
  FIND_PROGRAM(CMAKE_UNAME uname /bin /usr/bin /usr/local/bin )
  IF(CMAKE_UNAME)
    EXEC_PROGRAM(uname ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
    SET(CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} CACHE INTERNAL
			"processor type (i386 and x86_64)")
		 IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
		   ADD_DEFINITIONS(-fPIC)
		 ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  ENDIF(CMAKE_UNAME)
ENDIF(UNIX AND NOT WIN32)

#gitnarc
exec_program(gn ARGS gen-build-info mvthImage -c libMvthImage -c mvthImage -c viewImage --header) # generate build info source files when we configure
add_custom_target(gitnarc ALL gn gen-build-info mvthImage -c libMvthImage -c mvthImage -c viewImage --header) # generate build info source files every time we build
# set variables to the build info source files
set(LIB_MVTH_IMAGE_BUILD_INFO ${PROJECT_BINARY_DIR}/mvthImage_buildinfo_libMvthImage.c)
set(MVTH_IMAGE_BUILD_INFO ${PROJECT_BINARY_DIR}/mvthImage_buildinfo_mvthImage.c)
set(VIEW_IMAGE_BUILD_INFO ${PROJECT_BINARY_DIR}/mvthImage_buildinfo_viewImage.c)


add_subdirectory(src)
add_subdirectory(scripts)
#add_subdirectory(testing)
