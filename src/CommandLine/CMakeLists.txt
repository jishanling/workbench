
#
# Name of project
#
PROJECT (CommandLine)

#cmake_policy(SET CMP0015 OLD) 

#
# Need XML from Qt
#
#SET(QT_DONT_USE_QTGUI TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})

#
# Name of executable
#
set (EXE_NAME wb_command)

#
# Resources
#
SET (RESOURCES_QRC_FILE ../Resources/resources.qrc)
QT4_ADD_RESOURCES(IMAGE_RCS_SRCS ${RESOURCES_QRC_FILE})

#
# Create the executable
# Apple creates a bundle
#
IF (APPLE)
   ADD_EXECUTABLE(${EXE_NAME}
      MACOSX_BUNDLE
      wb_command.cxx
      ${IMAGE_RCS_SRCS}
   )
ELSE (APPLE)
   ADD_EXECUTABLE(${EXE_NAME}
      wb_command.cxx
      ${IMAGE_RCS_SRCS}
   )
ENDIF (APPLE)


#
# Libraries that are linked
# Note: OSMESA library variables will be empty and have no effect
# if OSMESA is not available
#
TARGET_LINK_LIBRARIES(${EXE_NAME}
Commands
Operations
Algorithms
OperationsBase
Brain
GlfFont
Files
Palette
Gifti
Cifti
Nifti
Charting
FilesBase
Scenes
Xml
Common
Quazip
${QT_LIBRARIES}
${OSMESA_OFFSCREEN_LIBRARY}
${OSMESA_GL_LIBRARY}
${OSMESA_GLU_LIBRARY}
${ZLIB_LIBRARIES}
${LIBS})

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/CommandLine
${CMAKE_SOURCE_DIR}/Commands
${CMAKE_SOURCE_DIR}/Operations
${CMAKE_SOURCE_DIR}/Algorithms
${CMAKE_SOURCE_DIR}/OperationsBase
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/GlfFont
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Scenes
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Quazip
)

