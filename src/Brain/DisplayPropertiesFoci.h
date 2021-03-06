#ifndef __DISPLAY_PROPERTIES_FOCI__H_
#define __DISPLAY_PROPERTIES_FOCI__H_

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

#include "BrainConstants.h"
#include "DisplayGroupEnum.h"
#include "DisplayProperties.h"
#include "FeatureColoringTypeEnum.h"
#include "FociDrawingTypeEnum.h"

namespace caret {

    class DisplayPropertiesFoci : public DisplayProperties {
        
    public:
        DisplayPropertiesFoci();
        
        virtual ~DisplayPropertiesFoci();

        virtual void reset();
        
        virtual void update();
        
        virtual void copyDisplayProperties(const int32_t sourceTabIndex,
                                        const int32_t targetTabIndex);
        
        bool isDisplayed(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex) const;
        
        void setDisplayed(const DisplayGroupEnum::Enum displayGroup,
                          const int32_t tabIndex,
                          const bool displayStatus);
        
        bool isContralateralDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                      const int32_t tabIndex) const;
        
        void setContralateralDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex,
                                       const bool contralateralDisplayStatus);
        
        DisplayGroupEnum::Enum getDisplayGroupForTab(const int32_t browserTabIndex) const;
        
        void setDisplayGroupForTab(const int32_t browserTabIndex,
                             const DisplayGroupEnum::Enum  displayGroup);
        
        float getFociSize(const DisplayGroupEnum::Enum displayGroup,
                          const int32_t tabIndex) const;
        
        void setFociSize(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex,
                         const float pointSize);
        
        FeatureColoringTypeEnum::Enum getColoringType(const DisplayGroupEnum::Enum displayGroup,
                                                   const int32_t tabIndex) const;
        
        void setColoringType(const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             const FeatureColoringTypeEnum::Enum coloringType);
        
        FociDrawingTypeEnum::Enum getDrawingType(const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex) const;
        
        void setDrawingType(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex,
                            const FociDrawingTypeEnum::Enum drawingType);
        
        void setPasteOntoSurface(const DisplayGroupEnum::Enum displayGroup,
                                 const int32_t tabIndex,
                                 const bool enabled);
        
        bool isPasteOntoSurface(const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex) const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private:
        DisplayPropertiesFoci(const DisplayPropertiesFoci&);

        DisplayPropertiesFoci& operator=(const DisplayPropertiesFoci&);
        
        DisplayGroupEnum::Enum m_displayGroup[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_displayStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        bool m_displayStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_contralateralDisplayStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        bool m_contralateralDisplayStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_pasteOntoSurfaceInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        bool m_pasteOntoSurfaceInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_fociSizeInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        float m_fociSizeInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        FeatureColoringTypeEnum::Enum m_coloringTypeInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        FeatureColoringTypeEnum::Enum m_coloringTypeInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        FociDrawingTypeEnum::Enum m_drawingTypeInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        FociDrawingTypeEnum::Enum m_drawingTypeInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
    };
    
#ifdef __DISPLAY_PROPERTIES_FOCI_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_FOCI_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_FOCI__H_
