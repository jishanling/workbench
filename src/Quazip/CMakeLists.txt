#
# Name of project
#
PROJECT(Quazip)

#
# QT include files
#
SET(QT_DONT_USE_QTGUI TRUE)

INCLUDE(${QT_USE_FILE})

SET(MOC_INPUT_HEADER_FILES
quazipfile.h
)

QT4_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})

#
# Prevents dll linkage errors on windows
#
#IF(WIN32)
#   ADD_DEFINITIONS(-DQUAZIP_STATIC)
#   IF(MSVC)
#      ADD_DEFINITIONS(-DQUAZIP_STATIC)
#      #ADD_DEFINITIONS(-DQUAZIP_BUILD)
#   ENDIF(MSVC)
#ELSE(WIN32)
#    ADD_DEFINITIONS(-DQUAZIP_STATIC)
#ENDIF(WIN32)

#
# Create a library
#
ADD_LIBRARY(Quazip
JlCompress.h
crypt.h
ioapi.h
quaadler32.h
quachecksum32.h
quacrc32.h
quazip.h
quazip_global.h
quazipfile.h
quazipfileinfo.h
quazipnewinfo.h
unzip.h
zip.h

${MOC_SOURCE_FILES}

JlCompress.cpp
qioapi.cpp
quaadler32.cpp
quacrc32.cpp
quazip.cpp
quazipfile.cpp
quazipnewinfo.cpp
unzip.c
zip.c
)

