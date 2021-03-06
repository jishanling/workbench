/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileTypeEnum.h"
#include "GiftiFile.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "NiftiEnums.h"
#include "PaletteColorMapping.h"

#include <limits>

using namespace caret;

/**
 * Constructor.
 */
MetricFile::MetricFile()
: GiftiTypeFile(DataFileTypeEnum::METRIC)
{
    this->initializeMembersMetricFile();
}

/**
 * Copy constructor.
 *
 * @param sf
 *     Surface file that is copied.
 */
MetricFile::MetricFile(const MetricFile& sf)
: GiftiTypeFile(sf)
{
    this->copyHelperMetricFile(sf);
}


/**
 * Assignment operator.
 *
 * @param sf
 *     Surface file that is copied.
 * @return
 *     This surface file with content replaced
 *     by the MetricFile parameter.
 */
MetricFile& 
MetricFile::operator=(const MetricFile& sf)
{
    if (this != &sf) {
        GiftiTypeFile::operator=(sf);
        this->copyHelperMetricFile(sf);
    }
    return *this;
}

/**
 * Destructor.
 */
MetricFile::~MetricFile()
{
    this->columnDataPointers.clear();
}

/**
 * Clear the surface file.
 */
void 
MetricFile::clear()
{
    GiftiTypeFile::clear();
    this->columnDataPointers.clear();
}

/**
 * Validate the contents of the file after it
 * has been read such as correct number of 
 * data arrays and proper data types/dimensions.
 */
void 
MetricFile::validateDataArraysAfterReading() throw (DataFileException)
{
    this->columnDataPointers.clear();

    this->initializeMembersMetricFile();
        
    this->verifyDataArraysHaveSameNumberOfRows(0, 0);

    bool isLabelData = false;
    
    const int32_t numberOfDataArrays = this->giftiFile->getNumberOfDataArrays();
    for (int32_t i = 0; i < numberOfDataArrays; i++) {
        GiftiDataArray* gda = this->giftiFile->getDataArray(i);
        if (gda->getDataType() != NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32) {
            if (gda->getIntent() == NiftiIntentEnum::NIFTI_INTENT_LABEL) {
                isLabelData = true;
            }
            gda->convertToDataType(NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32);
        }
        int numDims = gda->getNumberOfDimensions();
        std::vector<int64_t> dims = gda->getDimensions();
        if (numDims == 1 || (numDims == 2 && dims[1] == 1))
        {
            this->columnDataPointers.push_back(gda->getDataPointerFloat());
        } else {
            if (numDims != 2)
            {
                throw DataFileException("Invalid number of dimensions in metric file '" + getFileName() + "': " + AString::number(numDims));
            }
            if (numberOfDataArrays != 1)
            {
                throw DataFileException("Two dimensional data arrays are not allowed in metric files with multiple data arrays");
            }
            std::vector<int64_t> newdims = dims;
            newdims[1] = 1;
            GiftiFile* newFile = new GiftiFile();//convert to multiple 1-d arrays on the fly
            *(newFile->getMetaData()) = *(giftiFile->getMetaData());
            int32_t indices[2], newindices[2] = {0, 0};
            for (indices[1] = 0; indices[1] < dims[1]; ++indices[1])
            {
                GiftiDataArray* tempArray = new GiftiDataArray(NiftiIntentEnum::NIFTI_INTENT_NORMAL,
                                                            NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32,
                                                            newdims,
                                                            GiftiEncodingEnum::GZIP_BASE64_BINARY);
                for (indices[0] = 0; indices[0] < dims[0]; ++indices[0])
                {
                    newindices[0] = indices[0];
                    tempArray->setDataFloat32(newindices, gda->getDataFloat32(indices));
                }
                newFile->addDataArray(tempArray);
                newFile->setDataArrayName(indices[1], "#" + AString::number(indices[1] + 1));
                columnDataPointers.push_back(tempArray->getDataPointerFloat());
            }
            delete giftiFile;//delete old 2D file
            giftiFile = newFile;//drop new 1D file in
        }
    }
    
    if (isLabelData) {
        CaretLogWarning("Metric File: "
                        + this->getFileName()
                        + " contains data array with NIFTI_INTENT_LABEL !!!");
    }
}

/**
 * Get the number of nodes.
 *
 * @return
 *    The number of nodes.
 */
int32_t
MetricFile::getNumberOfNodes() const
{
    int32_t numNodes = 0;
    int32_t numDataArrays = this->giftiFile->getNumberOfDataArrays();
    if (numDataArrays > 0) {
        numNodes = this->giftiFile->getDataArray(0)->getNumberOfRows();
    }
    return numNodes;
}

/**
 * Get the number of columns.
 *
 * @return
 *   The number of columns.
 */
int32_t
MetricFile::getNumberOfColumns() const
{
    const int32_t numCols = this->giftiFile->getNumberOfDataArrays();
    return numCols;
}

/**
 * Initialize members of this class.
 */
void 
MetricFile::initializeMembersMetricFile()
{
}

/**
 * Helps copying files.
 *
 * @param sf
 *    File that is copied.
 */
void 
MetricFile::copyHelperMetricFile(const MetricFile& /*sf*/)
{
    this->validateDataArraysAfterReading();
}

/**
 * Get value for a node.
 * 
 * @param nodeIndex
 *     Node index.
 * @param columnIndex
 *     Column index.
 * @return
 *     Value at the given node and column indices.
 */
float 
MetricFile::getValue(const int32_t nodeIndex,
                     const int32_t columnIndex) const
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    CaretAssertMessage((nodeIndex >= 0) && (nodeIndex < this->getNumberOfNodes()), 
                       "Node Index out of range.");
    
    return this->columnDataPointers[columnIndex][nodeIndex];
}

/**
 * set label key for a node.
 * 
 * @param nodeIndex
 *     Node index.
 * @param columnIndex
 *     Column index.
 * param value
 *     Value inserted at the given node and column indices.
 */
void 
MetricFile::setValue(const int32_t nodeIndex,
                     const int32_t columnIndex,
                     const float value)
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    CaretAssertMessage((nodeIndex >= 0) && (nodeIndex < this->getNumberOfNodes()), "Node Index out of range.");
    
    this->columnDataPointers[columnIndex][nodeIndex] = value;
    setModified();
}

const float* 
MetricFile::getValuePointerForColumn(const int32_t columnIndex) const
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    return this->columnDataPointers[columnIndex];
}

void MetricFile::setNumberOfNodesAndColumns(int32_t nodes, int32_t columns)
{
    giftiFile->clearAndKeepMetadata();
    columnDataPointers.clear();
    std::vector<int64_t> dimensions;
    dimensions.push_back(nodes);
    for (int32_t i = 0; i < columns; ++i)
    {
        giftiFile->addDataArray(new GiftiDataArray(NiftiIntentEnum::NIFTI_INTENT_NORMAL, NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32, dimensions, GiftiEncodingEnum::GZIP_BASE64_BINARY));
        columnDataPointers.push_back(giftiFile->getDataArray(i)->getDataPointerFloat());
    }
    setModified();
}

/**
 * Add map(s) to this GIFTI file.
 * @param numberOfNodes
 *     Number of nodes.  If file is not empty, this value must
 *     match the number of nodes that are in the file.
 * @param numberOfMaps
 *     Number of maps to add.
 */
void 
MetricFile::addMaps(const int32_t numberOfNodes,
                       const int32_t numberOfMaps) throw (DataFileException)
{
    if (numberOfNodes <= 0) {
        throw DataFileException("When adding maps to "
                                + this->getFileNameNoPath()
                                + " the number of nodes must be greater than zero");
    }
    
    if (this->getNumberOfNodes() > 0) {
        if (numberOfNodes != this->getNumberOfNodes()) {
            throw DataFileException("When adding maps to "
                                    + this->getFileNameNoPath()
                                    + " the requested number of nodes is "
                                    + AString::number(numberOfNodes)
                                    + " but the file contains "
                                    + AString::number(this->getNumberOfNodes())
                                    + " nodes.");
        }
    }
    
    if (numberOfMaps <= 0) {
        throw DataFileException("When adding maps, the number of maps must be greater than zero.");
    }
    
    if ((this->getNumberOfNodes() > 0) 
        && (this->getNumberOfMaps() > 0)) {
        std::vector<int64_t> dimensions;
        dimensions.push_back(numberOfNodes);
        
        for (int32_t i = 0; i < numberOfMaps; ++i)
        {
            this->giftiFile->addDataArray(new GiftiDataArray(NiftiIntentEnum::NIFTI_INTENT_NORMAL, 
                                                             NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32, 
                                                             dimensions, 
                                                             GiftiEncodingEnum::GZIP_BASE64_BINARY));
            const int32_t mapIndex = giftiFile->getNumberOfDataArrays() - 1;
            this->columnDataPointers.push_back(giftiFile->getDataArray(mapIndex)->getDataPointerFloat());
        }
    }
    else {
        this->setNumberOfNodesAndColumns(numberOfNodes, 
                                         numberOfMaps);
    }
    
    this->setModified();
}

void MetricFile::setValuesForColumn(const int32_t columnIndex, const float* valuesIn)
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    float* myColumn = columnDataPointers[columnIndex];
    int numNodes = (int)getNumberOfNodes();
    for (int i = 0; i < numNodes; ++i)
    {
        myColumn[i] = valuesIn[i];
    }
    setModified();
}

void MetricFile::initializeColumn(const int32_t columnIndex, const float& value)
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    float* myColumn = columnDataPointers[columnIndex];
    int numNodes = (int)getNumberOfNodes();
    for (int i = 0; i < numNodes; ++i)
    {
        myColumn[i] = value;
    }
    setModified();
}

/**
 * Get the minimum and maximum values from ALL maps in this file.
 * Note that not all files (due to size of file) are able to provide
 * the minimum and maximum values from the file.  The return value
 * indicates success/failure.  If the failure (false) is returned
 * the returned values are likely +/- the maximum float values.
 *
 * @param dataRangeMinimumOut
 *    Minimum data value found.
 * @param dataRangeMaximumOut
 *    Maximum data value found.
 * @return
 *    True if the values are valid, else false.
 */
bool
MetricFile::getDataRangeFromAllMaps(float& dataRangeMinimumOut,
                                               float& dataRangeMaximumOut) const
{
    const int32_t numberOfMaps = getNumberOfMaps();

    if (numberOfMaps > 0) {
        dataRangeMaximumOut = -std::numeric_limits<float>::max();
        dataRangeMinimumOut = std::numeric_limits<float>::max();
        
        for (int32_t i = 0; i < numberOfMaps; ++i) {
            GiftiDataArray* gda = this->giftiFile->getDataArray(i);
            float mapMin, mapMax;
            gda->getMinMaxValuesFloat(mapMin,
                                      mapMax);
            
            if (mapMin < dataRangeMinimumOut) {
                dataRangeMinimumOut = mapMin;
            }
            if (mapMax > dataRangeMaximumOut) {
                dataRangeMaximumOut = mapMax;
            }
            
        }
    }
    else {
        dataRangeMaximumOut = std::numeric_limits<float>::max();
        dataRangeMinimumOut = -dataRangeMaximumOut;
    }
    
    return true;
}



