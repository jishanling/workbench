#ifndef __SURFACE_NODE_COLORING__H_
#define __SURFACE_NODE_COLORING__H_

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


#include "CaretObject.h"

namespace caret {

    class BrainStructure;
    class BrowserTabContent;
    class CiftiMappableConnectivityMatrixDataFile;
    class CiftiBrainordinateDataSeriesFile;
    class CiftiBrainordinateLabelFile;
    class CiftiBrainordinateScalarFile;
    class CiftiParcelScalarFile;
    class CiftiParcelSeriesFile;
    class DisplayPropertiesLabels;
    class Model;
    class LabelFile;
    class MetricFile;
    class Palette;
    class PaletteColorMapping;
    class RgbaFile;
    class Surface;
    class OverlaySet;
    
    /// Performs coloring of surface nodes
    class SurfaceNodeColoring : public CaretObject {
        
    public:
        SurfaceNodeColoring();
        
        virtual ~SurfaceNodeColoring();
        
        float* colorSurfaceNodes(Model* model,
                                 Surface* surface,
                                 const int32_t browserTabIndex);
        
    private:
        SurfaceNodeColoring(const SurfaceNodeColoring&);

        SurfaceNodeColoring& operator=(const SurfaceNodeColoring&);
        
    public:
        virtual AString toString() const;
        
    private:
        enum MetricColorType {
            METRIC_COLOR_TYPE_NORMAL,
            METRIC_COLOR_TYPE_POS_THRESH_COLOR,
            METRIC_COLOR_TYPE_NEG_THRESH_COLOR,
            METRIC_COLOR_TYPE_DO_NOT_COLOR
        };        
        
        void colorSurfaceNodes(const DisplayPropertiesLabels* dpl,
                               const int32_t browserTabIndex,
                               const Surface* surface,
                               OverlaySet* overlaySet,
                               float* rgbaNodeColors);
        
        bool assignLabelColoring(const DisplayPropertiesLabels* dpl,
                                 const int32_t browserTabIndex,
                                 const BrainStructure* brainStructure,
                                 const Surface* surface,
                                 const LabelFile* labelFile,
                                 const AString& labelMapUniqueID,
                                 const int32_t numberOfNodes,
                                 float* rgbv);

        bool assignCiftiLabelColoring(const DisplayPropertiesLabels* displayPropertiesLabels,
                                      const int32_t browserTabIndex,
                                      const BrainStructure* brainStructure,
                                  CiftiBrainordinateLabelFile* ciftiScalarFile,
                                  const AString& metricMapUniqueID,
                                  const int32_t numberOfNodes,
                                  float* rgbv);
        
        bool assignCiftiScalarColoring(const BrainStructure* brainStructure,
                                       CiftiBrainordinateScalarFile* ciftiScalarFile,
                                       const AString& metricMapUniqueID,
                                       const int32_t numberOfNodes,
                                       float* rgbv);
        
        bool assignCiftiParcelScalarColoring(const BrainStructure* brainStructure,
                                       CiftiParcelScalarFile* ciftiScalarFile,
                                       const AString& metricMapUniqueID,
                                       const int32_t numberOfNodes,
                                       float* rgbv);
        
        bool assignCiftiDataSeriesColoring(const BrainStructure* brainStructure,
                                       CiftiBrainordinateDataSeriesFile* ciftiDataSeriesFile,
                                       const AString& metricMapUniqueID,
                                       const int32_t numberOfNodes,
                                       float* rgbv);
        
        bool assignCiftiParcelSeriesColoring(const BrainStructure* brainStructure,
                                           CiftiParcelSeriesFile* ciftiParcelSeriesFile,
                                           const AString& metricMapUniqueID,
                                           const int32_t numberOfNodes,
                                           float* rgbv);
        
        bool assignCiftiMappableConnectivityMatrixColoring(const BrainStructure* brainStructure,
                                                   CiftiMappableConnectivityMatrixDataFile* ciftiConnectivityMatrixFile,
                                                   const AString& selectedMapUniqueID,
                                                   const int32_t numberOfNodes,
                                                   float* rgbv);
        
        bool assignMetricColoring(const BrainStructure* brainStructure,
                                  MetricFile* metricFile,
                                  const AString& metricMapUniqueID,
                                  const int32_t numberOfNodes,
                                  float* rgbv);
        
        bool assignRgbaColoring(const BrainStructure* brainStructure,
                                const RgbaFile* rgbaFile,
                                const AString& rgbaMapUniqueID,
                                const int32_t numberOfNodes,
                                float* rgbv);
        
};
    
#ifdef __SURFACE_NODE_COLORING_DECLARE__
#endif // __SURFACE_NODE_COLORING_DECLARE__

} // namespace
#endif  //__SURFACE_NODE_COLORING__H_
