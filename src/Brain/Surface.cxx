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

#include "BoundingBox.h"
#include "BrainStructure.h"
#include "Surface.h"

using namespace caret;

/**
 * Constructor.
 */
Surface::Surface()
{
    this->initializeMemberSurface();
}

/**
 * Destructor.
 */
Surface::~Surface()
{
}

/**
 * Copy constructor.
 *
 * @param s
 *    Surface that is copied.
 */
Surface::Surface(const Surface& s)
: SurfaceFile(s)
{
    this->copyHelperSurface(s);
}

/**
 * Assignment operator.
 *
 * @param s
 *    Contents of "s" are assigned to this.
 */
Surface& 
Surface::operator=(const Surface& s)
{
    if (this != &s) {
        SurfaceFile::operator=(s);
        this->copyHelperSurface(s);
    }
    
    return *this;
}

/**
 *
 */
AString 
Surface::getNameForGUI(bool includeStructureFlag) const
{
    AString msg;
    if (includeStructureFlag) {
        msg += StructureEnum::toGuiName(this->getStructure());
        msg += " ";
    }
    msg += SurfaceTypeEnum::toGuiName(this->getSurfaceType());
    msg += " ";
    msg += this->getFileNameNoPath();
    return msg;
}

/**
 * Set a bounding box using this surface's coordinates.
 *
 * @param boundingBoxOut
 *    Bounding box that is updated.
 */
void 
Surface::getBounds(BoundingBox& boundingBoxOut) const
{
    boundingBoxOut.set(this->getCoordinate(0), 
                       this->getNumberOfNodes());
}

/**
 * Initialize members of this class.
 */
void 
Surface::initializeMemberSurface()
{
    this->brainStructure = NULL;
}

/**
 * Helps with copying this surface.
 */
void 
Surface::copyHelperSurface(const Surface& /*s*/)
{
    this->initializeMemberSurface();
    this->computeNormals();
}

/**
 * @return Brain structure that holds this surface.
 */
const BrainStructure* 
Surface::getBrainStructure() const
{
    return this->brainStructure;
}

/**
 * @return Brain structure that holds this surface.
 */
BrainStructure* 
Surface::getBrainStructure()
{
    return this->brainStructure;
}

/**
 * Set brain structure that holds this surface.
 * @param brainStructure
 *   New value for brain structure.
 */
void 
Surface::setBrainStructure(BrainStructure* brainStructure)
{
    this->brainStructure = brainStructure;
}




