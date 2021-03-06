#
# Name of project
#
PROJECT (Operations)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})

#
# Create the helper library
#
ADD_LIBRARY(Operations
OperationAddToSpecFile.h
OperationBackendAverageDenseROI.h
OperationBackendAverageROICorrelation.h
OperationCiftiChangeTimestep.h
OperationCiftiConvert.h
OperationCiftiConvertToScalar.h
OperationCiftiEstimateFWHM.h
OperationCiftiLabelImport.h
OperationCiftiMath.h
OperationCiftiPalette.h
OperationCiftiROIAverage.h
OperationCiftiSeparateAll.h
OperationConvertAffine.h
OperationConvertFiberOrientations.h
OperationConvertMatrix4ToMatrix2.h
OperationConvertMatrix4ToWorkbenchSparse.h
OperationConvertWarpfield.h
OperationEstimateFiberBinghams.h
OperationException.h
OperationFileInformation.h
OperationFociGetProjectionVertex.h
OperationFociListCoords.h
OperationLabelExportTable.h
OperationLabelMask.h
OperationMetadataRemoveProvenance.h
OperationMetadataStringReplace.h
OperationMetricConvert.h
OperationMetricLabelImport.h
OperationMetricMask.h
OperationMetricMath.h
OperationMetricMerge.h
OperationMetricPalette.h
OperationMetricVertexSum.h
OperationNiftiInformation.h
OperationProbtrackXDotConvert.h
OperationSetMapName.h
OperationSetMapNames.h
OperationSetStructure.h
OperationShowScene.h
OperationSpecFileMerge.h
OperationSurfaceClosestVertex.h
OperationSurfaceCoordinatesToMetric.h
OperationSurfaceCutResample.h
OperationSurfaceFlipNormals.h
OperationSurfaceGeodesicDistance.h
OperationSurfaceGeodesicROIs.h
OperationSurfaceInformation.h
OperationSurfaceVertexAreas.h
OperationVolumeCapturePlane.h
OperationVolumeCreate.h
OperationVolumeLabelImport.h
OperationVolumeMath.h
OperationVolumePalette.h
OperationVolumeReorient.h
OperationVolumeSetSpace.h
OperationWbsparseMergeDense.h
OperationZipSceneFile.h
OperationZipSpecFile.h

OperationAddToSpecFile.cxx
OperationBackendAverageDenseROI.cxx
OperationBackendAverageROICorrelation.cxx
OperationCiftiChangeTimestep.cxx
OperationCiftiConvert.cxx
OperationCiftiConvertToScalar.cxx
OperationCiftiEstimateFWHM.cxx
OperationCiftiLabelImport.cxx
OperationCiftiMath.cxx
OperationCiftiPalette.cxx
OperationCiftiROIAverage.cxx
OperationCiftiSeparateAll.cxx
OperationConvertAffine.cxx
OperationConvertFiberOrientations.cxx
OperationConvertMatrix4ToMatrix2.cxx
OperationConvertMatrix4ToWorkbenchSparse.cxx
OperationConvertWarpfield.cxx
OperationException.cxx
OperationEstimateFiberBinghams.cxx
OperationFileInformation.cxx
OperationFociGetProjectionVertex.cxx
OperationFociListCoords.cxx
OperationLabelExportTable.cxx
OperationLabelMask.cxx
OperationMetadataRemoveProvenance.cxx
OperationMetadataStringReplace.cxx
OperationMetricConvert.cxx
OperationMetricLabelImport.cxx
OperationMetricMask.cxx
OperationMetricMath.cxx
OperationMetricMerge.cxx
OperationMetricPalette.cxx
OperationMetricVertexSum.cxx
OperationNiftiInformation.cxx
OperationProbtrackXDotConvert.cxx
OperationSetMapName.cxx
OperationSetMapNames.cxx
OperationSetStructure.cxx
OperationShowScene.cxx
OperationSpecFileMerge.cxx
OperationSurfaceClosestVertex.cxx
OperationSurfaceCoordinatesToMetric.cxx
OperationSurfaceCutResample.cxx
OperationSurfaceFlipNormals.cxx
OperationSurfaceGeodesicDistance.cxx
OperationSurfaceGeodesicROIs.cxx
OperationSurfaceInformation.cxx
OperationSurfaceVertexAreas.cxx
OperationVolumeCapturePlane.cxx
OperationVolumeCreate.cxx
OperationVolumeLabelImport.cxx
OperationVolumeMath.cxx
OperationVolumePalette.cxx
OperationVolumeReorient.cxx
OperationVolumeSetSpace.cxx
OperationWbsparseMergeDense.cxx
OperationZipSceneFile.cxx
OperationZipSpecFile.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Algorithms
${CMAKE_SOURCE_DIR}/Operations
${CMAKE_SOURCE_DIR}/OperationsBase
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/Charting
${CMAKE_SOURCE_DIR}/GlfFont
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Quazip
${CMAKE_SOURCE_DIR}/Scenes
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)

#
# Mesa Library used by show scene command
#
IF (OSMESA_FOUND)
    ADD_DEFINITIONS(${OSMESA_DEFINITION})
    INCLUDE_DIRECTORIES(${OSMESA_INCLUDE_DIRECTORY})
ENDIF (OSMESA_FOUND)

