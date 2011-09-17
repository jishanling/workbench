#ifndef __ABSTRACT_ALGORITHM_H__
#define __ABSTRACT_ALGORITHM_H__

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

//make it easy to use the progress object, don't just forward declare
#include "ProgressObject.h"

namespace caret {

   class AbstractAlgorithm
   {
      AbstractAlgorithm();//prevent default construction
   protected:
      AbstractAlgorithm(ProgressObject* myProgressObject);
   public:
      static float getAlgorithmWeight();
      static float getAlgorithmInternalWeight();//override these to make progress bars smooth
      static float getSubAlgorithmWeight();
   };

}
#endif //__ABSTRACT_ALGORITHM_H__
