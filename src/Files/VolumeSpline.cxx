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

#include "CaretOMP.h"
#include "CubicSpline.h"
#include "VolumeSpline.h"

#include <algorithm>
#include <cmath>
#include <vector>

using namespace std;
using namespace caret;

VolumeSpline::VolumeSpline()
{
    m_dims[0] = 0;
    m_dims[1] = 0;
    m_dims[2] = 0;
}

VolumeSpline::VolumeSpline(const float* frame, const int64_t framedims[3])
{
    m_dims[0] = framedims[0];
    m_dims[1] = framedims[1];
    m_dims[2] = framedims[2];
    m_deconv = CaretArray<float>(m_dims[0] * m_dims[1] * m_dims[2]);
    CaretArray<float> scratchArray(m_dims[0] * max(m_dims[1], m_dims[2])), deconvScratch(max(m_dims[0], max(m_dims[1], m_dims[2])));//allocate as much as we will need, even if we don't use it all yet
    predeconvolve(deconvScratch, m_dims[0]);
    for (int k = 0; k < m_dims[2]; ++k)
    {
        int64_t index = m_dims[0] * m_dims[1] * k;
        int64_t index2 = 0;
        for (int j = 0; j < m_dims[1]; ++j)
        {
            for (int i = 0; i < m_dims[0]; ++i)
            {
                scratchArray[index2] = frame[index];
                ++index;
                ++index2;
            }
        }
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int j = 0; j < m_dims[1]; ++j)
        {
            int64_t privIndex = j * m_dims[0];
            deconvolve(scratchArray.getArray() + privIndex, deconvScratch, m_dims[0]);
        }
        index = m_dims[0] * m_dims[1] * k;
        index2 = 0;
        for (int j = 0; j < m_dims[1]; ++j)
        {
            for (int i = 0; i < m_dims[0]; ++i)
            {
                m_deconv[index] = scratchArray[index2];
                ++index;
                ++index2;
            }
        }
    }
    predeconvolve(deconvScratch, m_dims[1]);
    for (int k = 0; k < m_dims[2]; ++k)
    {
        int64_t indexbase = k * m_dims[1] * m_dims[0];
        int64_t index = indexbase;
        for (int j = 0; j < m_dims[1]; ++j)
        {
            int64_t index2 = j;
            for (int i = 0; i < m_dims[0]; ++i)
            {
                scratchArray[index2] = m_deconv[index];//read linearly from frame while writing transposed should be slightly faster, because cache can stay dirty?
                index2 += m_dims[1];
                ++index;
            }
        }
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int i = 0; i < m_dims[0]; ++i)
        {
            int64_t privindex = i * m_dims[1];
            deconvolve(scratchArray.getArray() + privindex, deconvScratch, m_dims[1]);
        }
        index = 0;
        for (int i = 0; i < m_dims[0]; ++i)
        {
            int64_t index2 = i + indexbase;
            for (int j = 0; j < m_dims[1]; ++j)
            {
                m_deconv[index2] = scratchArray[index];//even though scratch should be cached now, if writing to frame collides, reading linearly should give better behavior
                ++index;
                index2 += m_dims[0];
            }
        }
    }
    predeconvolve(deconvScratch, m_dims[2]);
    for (int j = 0; j < m_dims[1]; ++j)//finally, use a similar strategy to do linear reads instead of widely interleaved reads for k-rows
    {
        int64_t indexbase = j * m_dims[0];
        int64_t increment = m_dims[1] * m_dims[0];
        for (int k = 0; k < m_dims[2]; ++k)
        {
            int64_t index = indexbase + k * increment;
            int64_t index2 = k;
            for (int i = 0; i < m_dims[0]; ++i)
            {
                scratchArray[index2] = m_deconv[index];
                index2 += m_dims[2];
                ++index;
            }
        }
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int i = 0; i < m_dims[0]; ++i)
        {
            int64_t privindex = i * m_dims[2];
            deconvolve(scratchArray.getArray() + privindex, deconvScratch, m_dims[2]);
        }
        for (int i = 0; i < m_dims[0]; ++i)
        {
            int64_t index = indexbase + i;
            int64_t index2 = i * m_dims[2];
            for (int k = 0; k < m_dims[2]; ++k)
            {
                m_deconv[index] = scratchArray[index2];
                index += increment;
                ++index2;
            }
        }
    }
}

float VolumeSpline::sample(const float& ifloat, const float& jfloat, const float& kfloat)
{
    if (m_dims[0] < 1 || ifloat < 0.0f || jfloat < 0.0f || kfloat < 0.0f || ifloat > m_dims[0] - 1 || jfloat > m_dims[1] - 1 || kfloat > m_dims[2] - 1) return 0.0f;//yeesh
    const int64_t zstep = m_dims[0] * m_dims[1];
    float iparti, ipartj, ipartk;
    float fparti = modf(ifloat, &iparti);
    float fpartj = modf(jfloat, &ipartj);
    float fpartk = modf(kfloat, &ipartk);
    int64_t lowi = (int64_t)iparti;
    int64_t lowj = (int64_t)ipartj;
    int64_t lowk = (int64_t)ipartk;
    bool lowedgei = (lowi < 1);
    bool lowedgej = (lowj < 1);
    bool lowedgek = (lowk < 1);
    bool highedgei = (lowi >= m_dims[0] - 2);
    bool highedgej = (lowj >= m_dims[1] - 2);
    bool highedgek = (lowk >= m_dims[2] - 2);
    CubicSpline ispline = CubicSpline::bspline(fparti, lowedgei, highedgei);
    CubicSpline jspline = CubicSpline::bspline(fpartj, lowedgej, highedgej);
    CubicSpline kspline = CubicSpline::bspline(fpartk, lowedgek, highedgek);
    float jtemp[4], ktemp[4];//the weights of the splines are zero for off-the edge values, but zero the data anyway
    jtemp[0] = 0.0f;
    jtemp[3] = 0.0f;
    ktemp[0] = 0.0f;
    ktemp[3] = 0.0f;
    if (lowedgei || lowedgej || lowedgek || highedgei || highedgej || highedgek)
    {//there is an edge nearby, use the generic version with more conditionals
        int jstart = lowedgej ? 1 : 0;
        int kstart = lowedgek ? 1 : 0;
        int jend = highedgej ? 3 : 4;
        int kend = highedgek ? 3 : 4;
        for (int k = kstart; k < kend; ++k)
        {
            int64_t indexk = (k + lowk - 1) * zstep;
            for (int j = jstart; j < jend; ++j)
            {
                int64_t indexj = indexk + (j + lowj - 1) * m_dims[0];
                if (lowedgei)//have to do these tests for the simple reason that otherwise we might access off the end of the array in two of the 8 corners
                {
                    if (highedgei)
                    {
                        jtemp[j] = ispline.evalBothEdge(m_deconv[indexj + 1], m_deconv[indexj + 2]);
                    } else {
                        jtemp[j] = ispline.evalLowEdge(m_deconv[indexj + 1], m_deconv[indexj + 2], m_deconv[indexj + 3]);
                    }
                } else {
                    if (highedgei)
                    {
                        jtemp[j] = ispline.evalHighEdge(m_deconv[indexj], m_deconv[indexj + 1], m_deconv[indexj + 2]);
                    } else {
                        jtemp[j] = ispline.evaluate(m_deconv[indexj], m_deconv[indexj + 1], m_deconv[indexj + 2], m_deconv[indexj + 3]);
                    }
                }
            }
            ktemp[k] = jspline.evaluate(jtemp[0], jtemp[1], jtemp[2], jtemp[3]);
        }
        return kspline.evaluate(ktemp[0], ktemp[1], ktemp[2], ktemp[3]);
    } else {//we are clear of all edges, we can use fewer conditionals
        int64_t indexbase = lowi - 1 + m_dims[0] * (lowj - 1 + m_dims[1] * (lowk - 1));
        const float* basePtr = m_deconv.getArray() + indexbase;
        int64_t indexk = 0;
        for (int k = 0; k < 4; ++k)
        {
            int64_t indexj = indexk;
            for (int j = 0; j < 4; ++j)
            {
                jtemp[j] = ispline.evaluate(basePtr[indexj], basePtr[indexj + 1], basePtr[indexj + 2], basePtr[indexj + 3]);
                indexj += m_dims[0];
            }
            ktemp[k] = jspline.evaluate(jtemp[0], jtemp[1], jtemp[2], jtemp[3]);
            indexk += zstep;
        }
        return kspline.evaluate(ktemp[0], ktemp[1], ktemp[2], ktemp[3]);
    }
}

void VolumeSpline::deconvolve(float* data, const float* backsubs, const int64_t& length)
{
    if (length < 1) return;
    const float A = 1.0f / 6.0f, B = 2.0f / 3.0f;//the coefficients of a bspline at center and +/-1
    //forward pass simulating gaussian elimination on matrix of bspline kernels and data
    data[0] /= B;
    for (int i = 1; i < length; ++i)//the first row is handled slightly differently
    {
        data[i] = (data[i] - A * data[i - 1]) / (B - A * backsubs[i - 1]);
    }//back substitution, making it gauss-jordan
    for (int i = length - 2; i >= 0; --i)//the last row doesn't need back-substitution
    {
        data[i] -= backsubs[i] * data[i + 1];
    }
}

void VolumeSpline::predeconvolve(float* backsubs, const int64_t& length)
{
    if (length < 1) return;
    const float A = 1.0f / 6.0f, B = 2.0f / 3.0f;
    backsubs[0] = A / B;
    for (int i = 1; i < length; ++i)
    {
        backsubs[i] = A / (B - A * backsubs[i - 1]);
    }
}