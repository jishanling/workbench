#ifndef __EVENT_GET_MODEL_DISPLAY_CONTROLLERS_H__
#define __EVENT_GET_MODEL_DISPLAY_CONTROLLERS_H__

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

#include <vector>

#include "Event.h"

namespace caret {

    class Model;
    class ModelSurface;
    
    
    /// Event for getting model display controllers
    class EventModelGetAll : public Event {
        
    public:
        EventModelGetAll();
        
        virtual ~EventModelGetAll();
        
        void addModels(const std::vector<Model*>& modelDisplayControllers);

        const std::vector<Model*> getModels() const;
        
        bool isModelValid(const Model* modelDisplayController) const;
        
        Model* getFirstModel() const;
        
        ModelSurface* getFirstModelSurface() const;
        
    private:
        EventModelGetAll(const EventModelGetAll&);
        
        EventModelGetAll& operator=(const EventModelGetAll&);
        
        std::vector<Model*> modelDisplayControllers;
    };

} // namespace

#endif // __EVENT_GET_MODEL_DISPLAY_CONTROLLERS_H__