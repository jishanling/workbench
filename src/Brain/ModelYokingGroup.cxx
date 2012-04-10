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

#include <algorithm>
#include <cmath>

#include "BrowserTabContent.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "ModelYokingGroup.h"

#include "Brain.h"
#include "BrainOpenGL.h"
#include "Surface.h"

using namespace caret;

/**
 * Constructor.
 * @param surface - surface for this controller.
 *
 */
ModelYokingGroup::ModelYokingGroup(const int32_t yokingGroupIndex,
                                                                     const AString& yokingGroupName,
                                                                     const YokingTypeEnum::Enum yokingType)
: Model(ModelTypeEnum::MODEL_TYPE_YOKING,
                         YOKING_ALLOWED_YES,
                         ROTATION_ALLOWED_YES,
                         NULL)
{
    this->initializeMembersModelYokingGroup();
    this->yokingType = yokingType;
    this->yokingGroupIndex = yokingGroupIndex;
    this->yokingGroupName  = yokingGroupName;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->dorsalView(i);
    }
}

/**
 * Destructor
 */
ModelYokingGroup::~ModelYokingGroup()
{
}

void
ModelYokingGroup::initializeMembersModelYokingGroup()
{
    this->yokingGroupIndex = -1;
}

/**
 * @return Type of yoking this yoking controller performs.
 */
YokingTypeEnum::Enum 
ModelYokingGroup::getYokingType() const
{
    return this->yokingType;
}

/**
 * Get the name for use in a GUI.
 *
 * @param includeStructureFlag - Prefix label with structure to which
 *      this structure model belongs.
 * @return   Name for use in a GUI.
 *
 */
AString
ModelYokingGroup::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    return this->yokingGroupName;
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model controller.
 */
AString 
ModelYokingGroup::getNameForBrowserTab() const
{
    return this->yokingGroupName;
}

/**
 * @return The index of this yoking group.
 */
int32_t 
ModelYokingGroup::getYokingGroupIndex() const
{
    return this->yokingGroupIndex;
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
OverlaySet* 
ModelYokingGroup::getOverlaySet(const int /*tabIndex*/)
{
    CaretAssertMessage(0, "NEVER should be called.");
    return NULL;
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
const OverlaySet* 
ModelYokingGroup::getOverlaySet(const int /*tabIndex*/) const
{
    CaretAssertMessage(0, "NEVER should be called.");
    return NULL;
}

/**
 * Initilize the overlays for this controller.
 */
void 
ModelYokingGroup::initializeOverlays()
{
}


