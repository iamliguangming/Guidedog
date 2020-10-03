# Install script for directory: /home/iamliguangming/Graduate_courses/CIS700/Guidedog/catkin_ws/src/pedsim_ros/pedsim_sensors

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/iamliguangming/Graduate_courses/CIS700/Guidedog/catkin_ws/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/iamliguangming/Graduate_courses/CIS700/Guidedog/catkin_ws/build/pedsim_ros/pedsim_sensors/catkin_generated/installspace/pedsim_sensors.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pedsim_sensors/cmake" TYPE FILE FILES
    "/home/iamliguangming/Graduate_courses/CIS700/Guidedog/catkin_ws/build/pedsim_ros/pedsim_sensors/catkin_generated/installspace/pedsim_sensorsConfig.cmake"
    "/home/iamliguangming/Graduate_courses/CIS700/Guidedog/catkin_ws/build/pedsim_ros/pedsim_sensors/catkin_generated/installspace/pedsim_sensorsConfig-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pedsim_sensors" TYPE FILE FILES "/home/iamliguangming/Graduate_courses/CIS700/Guidedog/catkin_ws/src/pedsim_ros/pedsim_sensors/package.xml")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_people_sensor" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_people_sensor")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_people_sensor"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors" TYPE EXECUTABLE FILES "/home/iamliguangming/Graduate_courses/CIS700/Guidedog/catkin_ws/devel/lib/pedsim_sensors/pedsim_people_sensor")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_people_sensor" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_people_sensor")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_people_sensor"
         OLD_RPATH "/opt/ros/melodic/lib:/home/iamliguangming/Graduate_courses/CIS700/Guidedog/catkin_ws/devel/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_people_sensor")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_obstacle_sensor" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_obstacle_sensor")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_obstacle_sensor"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors" TYPE EXECUTABLE FILES "/home/iamliguangming/Graduate_courses/CIS700/Guidedog/catkin_ws/devel/lib/pedsim_sensors/pedsim_obstacle_sensor")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_obstacle_sensor" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_obstacle_sensor")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_obstacle_sensor"
         OLD_RPATH "/opt/ros/melodic/lib:/home/iamliguangming/Graduate_courses/CIS700/Guidedog/catkin_ws/devel/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_obstacle_sensor")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_occlusion_sensor" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_occlusion_sensor")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_occlusion_sensor"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors" TYPE EXECUTABLE FILES "/home/iamliguangming/Graduate_courses/CIS700/Guidedog/catkin_ws/devel/lib/pedsim_sensors/pedsim_occlusion_sensor")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_occlusion_sensor" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_occlusion_sensor")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_occlusion_sensor"
         OLD_RPATH "/opt/ros/melodic/lib:/home/iamliguangming/Graduate_courses/CIS700/Guidedog/catkin_ws/devel/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/pedsim_sensors/pedsim_occlusion_sensor")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/pedsim_sensors" TYPE DIRECTORY FILES "/home/iamliguangming/Graduate_courses/CIS700/Guidedog/catkin_ws/src/pedsim_ros/pedsim_sensors/include/pedsim_sensors/" FILES_MATCHING REGEX "/[^/]*\\.h$" REGEX "/\\.svn$" EXCLUDE)
endif()

