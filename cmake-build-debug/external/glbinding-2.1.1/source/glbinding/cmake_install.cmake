# Install script for directory: /Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/external/glbinding-2.1.1/source/glbinding

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/cmake-build-debug/external/glbinding-2.1.1/libglbindingd.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglbindingd.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglbindingd.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libglbindingd.a")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/external/glbinding-2.1.1/source/glbinding/include/glbinding")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/cmake-build-debug/external/glbinding-2.1.1/source/glbinding/include/glbinding")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/glbinding/glbinding-export.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/glbinding/glbinding-export.cmake"
         "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/cmake-build-debug/external/glbinding-2.1.1/source/glbinding/CMakeFiles/Export/cmake/glbinding/glbinding-export.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/glbinding/glbinding-export-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/glbinding/glbinding-export.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake/glbinding" TYPE FILE FILES "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/cmake-build-debug/external/glbinding-2.1.1/source/glbinding/CMakeFiles/Export/cmake/glbinding/glbinding-export.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake/glbinding" TYPE FILE FILES "/Users/zeroliam/Documents/HCI 1719/WiSe 2018/Computer Graphics/Homework Project/2018 hw/cg_assignments/cmake-build-debug/external/glbinding-2.1.1/source/glbinding/CMakeFiles/Export/cmake/glbinding/glbinding-export-debug.cmake")
  endif()
endif()

