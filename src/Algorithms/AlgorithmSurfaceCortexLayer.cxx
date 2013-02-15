#ifdef VELAB_INTERNAL_RELEASE_ONLY

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

#include "AlgorithmSurfaceCortexLayer.h"
#include "AlgorithmException.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include "Vector3D.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmSurfaceCortexLayer::getCommandSwitch()
{
    return "-surface-cortex-layer";
}

AString AlgorithmSurfaceCortexLayer::getShortDescription()
{
    return "CREATE SURFACE APPROXIMATING A CORTICAL LAYER";
}

OperationParameters* AlgorithmSurfaceCortexLayer::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "white-surface", "the white matter surface");
    
    ret->addSurfaceParameter(2, "pial-surface", "the pial surface");
    
    ret->addDoubleParameter(3, "location", "what volume fraction to place the layer at");
    
    ret->addSurfaceOutputParameter(4, "out-surface", "the output surface");
    
    OptionalParameter* metricOpt = ret->createOptionalParameter(5, "-placement-out", "output the placement as a distance fraction from pial to white");
    metricOpt->addMetricOutputParameter(1, "placement-metric", "output metric");
    
    ret->createOptionalParameter(6, "-untwist", "temporary option for comparing methods, specify to use old method");
    
    ret->setHelpText(
        AString("The input surfaces must have node correspondence.  ") +
        "The output surface is generated by placing nodes between the two surfaces such that the enclosed volume within any small patch of the new and white surfaces " +
        "is the given fraction of the volume of the same patch between the pial and white surfaces " +
        "(i.e., specifying 0 would give the white surface, 1 would give the pial surface).  "
    );
    return ret;
}

void AlgorithmSurfaceCortexLayer::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* myWhiteSurf = myParams->getSurface(1);
    SurfaceFile* myPialSurf = myParams->getSurface(2);
    float myVolFrac = myParams->getDouble(3);
    SurfaceFile* myOutSurf = myParams->getOutputSurface(4);
    MetricFile* myMetricOut = NULL;
    OptionalParameter* metricOpt = myParams->getOptionalParameter(5);
    if (metricOpt->m_present)
    {
        myMetricOut = metricOpt->getOutputMetric(1);
    }
    bool untwistMode = myParams->getOptionalParameter(6)->m_present;
    AlgorithmSurfaceCortexLayer(myProgObj, myWhiteSurf, myPialSurf, myVolFrac, myOutSurf, myMetricOut, untwistMode);
}

AlgorithmSurfaceCortexLayer::AlgorithmSurfaceCortexLayer(ProgressObject* myProgObj, const SurfaceFile* myWhiteSurf, const SurfaceFile* myPialSurf,
                                                         const float& myVolFrac, SurfaceFile* myOutSurf, MetricFile* myMetricOut, const bool& untwistMode) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = myWhiteSurf->getNumberOfNodes();
    if (numNodes != myPialSurf->getNumberOfNodes())
    {
        throw AlgorithmException("input surfaces have different number of nodes");
    }
    *myOutSurf = *myWhiteSurf;//copy topology
    myOutSurf->setSecondaryType(SecondarySurfaceTypeEnum::MIDTHICKNESS);//???
    if (myMetricOut != NULL)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        myMetricOut->setStructure(myWhiteSurf->getStructure());
        myMetricOut->setColumnName(0, "distance fraction");
    }
    const float* whiteCoords = myWhiteSurf->getCoordinateData();
    const float* pialCoords = myPialSurf->getCoordinateData();
    bool outside = false;
    bool above = false;
    if (myVolFrac < 0 || myVolFrac > 1)
    {
        outside = true;
        if (myVolFrac > 1) above = true;
    }
    CaretPointer<TopologyHelper> myTopoHelp = myWhiteSurf->getTopologyHelper();//trust that the input surfaces have the same topology
    for (int i = 0; i < numNodes; ++i)
    {
        Vector3D whiteCenter = whiteCoords + i * 3;
        Vector3D pialCenter = pialCoords + i * 3;
        float distFrac = 0.0f;
        if (untwistMode)
        {
            float d1;
            Vector3D axisHat = (pialCenter - whiteCenter).normal(&d1);
            const vector<int32_t>& neighbors = myTopoHelp->getNodeNeighbors(i);
            int numNeigh = (int)neighbors.size();
            for (int j = 0; j < numNeigh; ++j)
            {
                Vector3D whiteNeighbor = whiteCoords + neighbors[j] * 3;
                Vector3D pialNeighbor = pialCoords + neighbors[j] * 3;
                float R0 = axisHat.cross(whiteCenter - whiteNeighbor).length();
                float RF = axisHat.cross(pialCenter - pialNeighbor).length();
                float Rd = RF - R0;
                float h0 = axisHat.dot(whiteCenter - whiteNeighbor);
                float hF = axisHat.dot(pialCenter - pialNeighbor);
                float hd = hF - h0;
                float a = d1 * Rd * Rd;//coefficient for t^3
                float b = Rd * (3 * d1 * R0 + h0 * Rd - hd * R0);//t^2
                float c = R0 * (3 * d1 * R0 + 2 * h0 * Rd - 2 * hd * R0);//t
                float fullVol = c + b + a;//trivial to evaluate for t = 1, and t = 0 gives 0
                float target = fullVol * myVolFrac;
                float lowt = 0, lowval = 0, hight = 1, highval = fullVol;
                if (outside)
                {
                    if (above)
                    {
                        if (Rd / R0 < 0)
                        {
                            hight = min(-R0 / Rd, 1 + (myVolFrac - 1) * 3);//because conical volumes are 1/3 the equivalent cylinder
                        } else {
                            hight = myVolFrac;//because the longest result here is cylinder
                        }
                        highval = hight * (c + hight * (b + hight * a));
                    } else {
                        if (Rd / R0 > 0)
                        {
                            lowt = max(-R0 / Rd, myVolFrac * 3);//ditto
                        } else {
                            lowt = myVolFrac;
                        }
                        lowval = lowt * (c + lowt * (b + lowt * a));
                    }
                }//these ranges SHOULD make the function monotonic
                const float TOLER = 0.0001f;//stop when high and low bounds are this close
                const int MAX_ITERS = 100;//but don't take too long trying to get there
                int iter = 0;
                while (abs(hight - lowt) > TOLER && iter < MAX_ITERS)
                {
                    ++iter;
                    float highdiff = (highval - target);
                    float lowdiff = (target - lowval);
                    float guess = (lowt * highdiff + hight * lowdiff) / (highdiff + lowdiff);//weighted average to get guess
                    float highcap = (lowt + 9 * hight) / 10.0f;//make guess lie in the middle 80%, to make sure it always moves nontrivially
                    float lowcap = (9 * lowt + hight) / 10.0f;
                    if (guess < lowcap) guess = lowcap;
                    if (guess > highcap) guess = highcap;
                    float guessval = guess * (c + guess * (b + guess * a));
                    if ((guessval < target) == (lowval < highval))//in case positive t produces negative volume
                    {
                        lowt = guess;
                        lowval = guessval;
                    } else {
                        hight = guess;
                        highval = guessval;
                    }
                }
                float highdiff = (highval - target);//one more iteration to get the value to add to accum
                float lowdiff = (target - lowval);
                float guess = (lowt * highdiff + hight * lowdiff) / (highdiff + lowdiff);
                float highcap = (lowt + 9 * hight) / 10.0f;
                float lowcap = (9 * lowt + hight) / 10.0f;
                if (guess < lowcap) guess = lowcap;
                if (guess > highcap) guess = highcap;
                distFrac += guess;
            }
            distFrac /= numNeigh;
        } else {
            float a = 0.0f, b = 0.0f, c = 0.0f;//constants for the cubic function that will give the volume
            const vector<int32_t>& myTiles = myTopoHelp->getNodeTiles(i);
            int numTiles = (int)myTiles.size();
            for (int j = 0; j < numTiles; ++j)
            {
                const int32_t* whiteTile = myWhiteSurf->getTriangle(myTiles[j]);
                Vector3D whitePoints[3], pialPoints[3], layerDelta[3];
                for (int k = 0; k < 3; ++k)
                {
                    whitePoints[k] = whiteCoords + whiteTile[k] * 3;
                    pialPoints[k] = pialCoords + whiteTile[k] * 3;
                    layerDelta[k] = pialPoints[k] - whitePoints[k];
                }//calculate volume with vector field trick: http://en.wikipedia.org/wiki/Polyhedron#Volume
                Vector3D refPoint = whitePoints[0];//use an "origin" for the vector field that is near the polyhedron for numerical stability - also, this makes several sides have zero contribution
                //NOTE: the white surface triangle has zero contribution due to choice of reference point, would otherwise contribute to both volumes
                //layerVol += (layerPoints[0] - refPoint).dot((layerPoints[1] - layerPoints[0]).cross(layerPoints[2] - layerPoints[1]));
                //            (t * (pialPoints[0] - whitePoints[0])).dot((t * (pialPoints[1] - pialPoints[0] - (whitePoints[1] - whitePoints[0])) + whitePoints[1] - whitePoints[0]).cross(...)
                Vector3D edge10delta = pialPoints[1] - pialPoints[0] - whitePoints[1] + whitePoints[0];
                Vector3D edge21delta = pialPoints[2] - pialPoints[1] - whitePoints[2] + whitePoints[1];
                //            (t * layerDelta[0]).dot((t * edge10delta + whitePoints[1] - whitePoints[0]).cross(t * edge21delta + whitePoints[2] - whitepoints[1]));
                a += layerDelta[0].dot(edge10delta.cross(edge21delta));
                b += layerDelta[0].dot((whitePoints[1] - whitePoints[0]).cross(edge21delta) + edge10delta.cross(whitePoints[2] - whitePoints[1]));
                c += layerDelta[0].dot((whitePoints[1] - whitePoints[0]).cross(whitePoints[2] - whitePoints[1]));
                int m = 2;
                for (int k = 0; k < 3; ++k)//walk the side quadrilaterals as 2->0, 0->1, 1->2
                {//add the average of the two triangulations of the quadrilateral
                    Vector3D mref = whitePoints[m] - refPoint, kref = whitePoints[k] - refPoint;//precalculate reference points on the white surface for use in both volumes
                    Vector3D whiteEdge = whitePoints[k] - whitePoints[m];//precalculate this frequently used edge also
                    if (k != 0 && m != 0) //layerVol += 0.5f * mref.dot(whiteEdge.cross(layerPoints[k] - whitePoints[k]));
                    {
                        c += 0.5f * mref.dot(whiteEdge.cross(layerDelta[k]));
                    }
                    if (m != 0) //layerVol += 0.5f * mref.dot((layerPoints[m] - whitePoints[m]).cross(whitePoints[m] - layerPoints[k]));
                    {
                        //                    0.5f * mref.dot((t * layerDelta[m]).cross(whitePoints[m] - (whitePoints[k] + t * layerDelta[k])));
                        b += 0.5f * mref.dot(layerDelta[m].cross(-layerDelta[k]));
                        c += 0.5f * mref.dot(layerDelta[m].cross(-whiteEdge));
                    }
                    if (k != 0 && m != 0) //layerVol += 0.5f * kref.dot(whiteEdge.cross(layerPoints[m] - whitePoints[k]));
                    {
                        //                              0.5f * kref.dot(whiteEdge.cross(layerPoints[m] - whitePoints[m]));//use a formula where t = 0 gives a zero length edge
                        c += 0.5f * kref.dot(whiteEdge.cross(layerDelta[m]));
                    }
                    if (k != 0) //layerVol += 0.5f * kref.dot((layerPoints[k] - whitePoints[k]).cross(layerPoints[m] - layerPoints[k]));
                    {
                        //                    0.5f * kref.dot((t * layerDelta[k]).cross((whitePoints[m] + t * layerDelta[m]) - (whitePoints[k] + t * layerDelta[k])));
                        b += 0.5f * kref.dot(layerDelta[k].cross(layerDelta[m] - layerDelta[k]));
                        c += 0.5f * kref.dot(layerDelta[k].cross(-whiteEdge));
                    }
                    m = k;
                }
            }
            float fullVol = a + b + c;//trivial to evaluate for t = 1, and t = 0 gives 0
            float target = fullVol * myVolFrac;
            float lowt = 0, lowval = 0, hight = 1, highval = fullVol;
            if (outside)
            {
                if (above)
                {
                    hight = 1 + (myVolFrac - 1) * 3;//set a limit at 3 times further out than the requested fraction, and just hope that it is monotonic
                    highval = hight * (c + hight * (b + hight * a));
                } else {
                    lowt = myVolFrac * 3;//ditto
                    lowval = lowt * (c + lowt * (b + lowt * a));
                }
            }//these ranges SHOULD make the function monotonic
            const float TOLER = 0.0001f;//stop when high and low bounds are this close
            const int MAX_ITERS = 100;//but don't take too long trying to get there
            int iter = 0;
            while (abs(hight - lowt) > TOLER && iter < MAX_ITERS)
            {
                ++iter;
                float highdiff = (highval - target);
                float lowdiff = (target - lowval);
                float guess = (lowt * highdiff + hight * lowdiff) / (highdiff + lowdiff);//weighted average to get guess
                float highcap = (lowt + 9 * hight) / 10.0f;//make guess lie in the middle 80%, to make sure it always moves nontrivially
                float lowcap = (9 * lowt + hight) / 10.0f;
                if (guess < lowcap) guess = lowcap;
                if (guess > highcap) guess = highcap;
                float guessval = guess * (c + guess * (b + guess * a));
                if ((guessval < target) == (lowval < highval))//in case positive t produces negative volume
                {
                    lowt = guess;
                    lowval = guessval;
                } else {
                    hight = guess;
                    highval = guessval;
                }
            }
            float highdiff = (highval - target);//one more iteration to get the value to add to accum
            float lowdiff = (target - lowval);
            float guess = (lowt * highdiff + hight * lowdiff) / (highdiff + lowdiff);
            float highcap = (lowt + 9 * hight) / 10.0f;
            float lowcap = (9 * lowt + hight) / 10.0f;
            if (guess < lowcap) guess = lowcap;
            if (guess > highcap) guess = highcap;
            distFrac = guess;
        }
        myOutSurf->setCoordinate(i, distFrac * (pialCenter - whiteCenter) + whiteCenter);
        if (myMetricOut != NULL)
        {
            myMetricOut->setValue(i, 0, distFrac);
        }
    }
}

float AlgorithmSurfaceCortexLayer::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceCortexLayer::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}

#endif // VELAB_INTERNAL_RELEASE_ONLY
