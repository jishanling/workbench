#ifndef __ALGORITHM_CIFTI_CORRELATION_GRADIENT_H__
#define __ALGORITHM_CIFTI_CORRELATION_GRADIENT_H__

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

#include "AbstractAlgorithm.h"
#include "StructureEnum.h"

namespace caret {
    
    class AlgorithmCiftiCorrelationGradient : public AbstractAlgorithm
    {
        AlgorithmCiftiCorrelationGradient();
        struct CacheRow
        {
            int m_ciftiIndex;
            std::vector<float> m_row;
        };
        struct RowInfo
        {
            bool m_haveCalculated;
            float m_mean, m_rootResidSqr;
            int m_cacheIndex;
            RowInfo()
            {
                m_haveCalculated = false;
                m_cacheIndex = -1;
            }
        };
        std::vector<CacheRow> m_rowCache;
        std::vector<RowInfo> m_rowInfo;
        std::vector<std::vector<float> > m_tempRows;//reuse return values in getRow instead of reallocating
        std::vector<float> m_outColumn;
        int m_cacheUsed;//reuse cache entries instead of reallocating them
        int m_numCols;
        const CiftiFile* m_inputCifti;//so that accesses work through the cache functions
        void cacheRow(const int& ciftiIndex);
        void computeRowStats(const float* row, float& mean, float& rootResidSqr);
        void doSubtract(float* row, const float& mean);
        void clearCache();
        const float* getRow(const int& ciftiIndex, float& rootResidSqr, const bool& mustBeCached = false);
        float* getTempRow();
        float correlate(const float* row1, const float& rrs1, const float* row2, const float& rrs2);
        void init(const CiftiFile* input);
        int numRowsForMem(const float& memLimitGB, const int& numComponentFloats, bool& cacheFullInput);
        void processSurfaceComponentLocal(StructureEnum::Enum& myStructure, const float& surfKern, const float& memLimitGB, SurfaceFile* mySurf);
        void processSurfaceComponent(StructureEnum::Enum& myStructure, const float& surfKern, const float& memLimitGB, SurfaceFile* mySurf);
        void processVolumeComponentLocal(StructureEnum::Enum& myStructure, const float& volKern, const float& memLimitGB);
        void processVolumeComponent(StructureEnum::Enum& myStructure, const float& volKern, const float& memLimitGB);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiCorrelationGradient(ProgressObject* myProgObj, const CiftiFile* myCifti, const float& volKern,
                                            CiftiFile* myCiftiOut, SurfaceFile* myLeftSurf = NULL, SurfaceFile* myRightSurf = NULL,
                                            SurfaceFile* myCerebSurf = NULL, const float& surfKern = -1.0f, const float& memLimitGB = -1.0f);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiCorrelationGradient> AutoAlgorithmCiftiCorrelationGradient;

}

#endif //__ALGORITHM_CIFTI_CORRELATION_GRADIENT_H__