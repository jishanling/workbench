#ifndef __BRAIN_OPEN_G_L_SHAPE_CYLINDER_H__
#define __BRAIN_OPEN_G_L_SHAPE_CYLINDER_H__

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


#include "BrainOpenGLShape.h"

namespace caret {

    class BrainOpenGLShapeCylinder : public BrainOpenGLShape {
        
    public:
        BrainOpenGLShapeCylinder(const int32_t numberOfSides);
        
        virtual ~BrainOpenGLShapeCylinder();
        
    private:
        BrainOpenGLShapeCylinder(const BrainOpenGLShapeCylinder&);

        BrainOpenGLShapeCylinder& operator=(const BrainOpenGLShapeCylinder&);
        
    protected:
        void drawShape(const BrainOpenGL::DrawMode drawMode,
                       const uint8_t rgba[4]);
        
        void drawShape(const BrainOpenGL::DrawMode drawMode,
                       const float rgba[4]);
        
        
        void setupOpenGLForShape(const BrainOpenGL::DrawMode drawMode);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

        // ADD_NEW_MEMBERS_HERE
        
        const int32_t m_numberOfSides;
        
        GLuint m_coordinatesBufferID;
        
        GLuint m_coordinatesRgbaByteBufferID;
        GLuint m_coordinatesRgbaFloatBufferID;
        
        GLuint m_normalsBufferID;
        GLuint m_triangleStripBufferID;
        
        GLuint m_displayList;
        
        std::vector<GLfloat> m_coordinates;
        std::vector<GLubyte> m_rgbaByte;
        std::vector<GLfloat> m_rgbaFloat;
        
        std::vector<GLfloat> m_normals;
        std::vector<GLuint> m_triangleStrip;
        
        bool m_isApplyColoring;
    };
    
#ifdef __BRAIN_OPEN_G_L_SHAPE_CYLINDER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_SHAPE_CYLINDER_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_SHAPE_CYLINDER_H__
