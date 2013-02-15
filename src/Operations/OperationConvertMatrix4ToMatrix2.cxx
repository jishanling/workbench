/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "OperationConvertMatrix4ToMatrix2.h"
#include "OperationException.h"

#include "CiftiFile.h"
#include "CaretSparseFile.h"

using namespace caret;
using namespace std;

AString OperationConvertMatrix4ToMatrix2::getCommandSwitch()
{
    return "-convert-matrix4-to-matrix2";
}

AString OperationConvertMatrix4ToMatrix2::getShortDescription()
{
    return "GENERATES A MATRIX2 CIFTI FROM MATRIX4 WBSPARSE";
}

OperationParameters* OperationConvertMatrix4ToMatrix2::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "matrix4-wbsparse", "a wbsparse matrix4 file");
    
    ret->addCiftiOutputParameter(2, "counts-out", "the total fiber counts, as a cifti file");
    
    OptionalParameter* distanceOpt = ret->createOptionalParameter(3, "-distances", "output average trajectory distance");
    distanceOpt->addCiftiOutputParameter(1, "distance-out", "the distances, as a cifti file");
    
    ret->setHelpText(
        AString("This command makes a cifti file from the fiber counts in a matrix4 wbsparse file, and optionally a second cifti file from the distances.")
    );
    return ret;
}

void OperationConvertMatrix4ToMatrix2::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString matrix4Name = myParams->getString(1);
    CiftiFile* myCountsOut = myParams->getOutputCifti(2);
    CiftiFile* myDistOut = NULL;
    OptionalParameter* distanceOpt = myParams->getOptionalParameter(3);
    if (distanceOpt->m_present)
    {
        myDistOut = distanceOpt->getOutputCifti(1);
    }
    CaretSparseFile matrix4(matrix4Name);
    const CiftiXML& myXML = matrix4.getCiftiXML();
    myCountsOut->setCiftiXML(myXML);
    if (myDistOut != NULL) myDistOut->setCiftiXML(myXML);
    int rowSize = myXML.getNumberOfColumns(), numRows = myXML.getNumberOfRows();
    vector<float> scratchRow(rowSize, 0.0f);
    vector<int64_t> indices;
    vector<FiberFractions> fibers;
    for (int i = 0; i < numRows; ++i)
    {
        matrix4.getFibersRowSparse(i, indices, fibers);
        int numIndices = (int)indices.size();
        for (int j = 0; j < numIndices; ++j)
        {
            scratchRow[indices[j]] = fibers[j].totalCount;
        }
        myCountsOut->setRow(scratchRow.data(), i);
        if (myDistOut != NULL)
        {
            for (int j = 0; j < numIndices; ++j)
            {
                scratchRow[indices[j]] = fibers[j].distance;
            }
            myDistOut->setRow(scratchRow.data(), i);
        }
        for (int j = 0; j < numIndices; ++j)
        {
            scratchRow[indices[j]] = 0.0f;
        }
    }
}