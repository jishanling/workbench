#ifndef __REDUCTION_ENUM_H__
#define __REDUCTION_ENUM_H__

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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


#include <stdint.h>
#include <vector>
#include "AString.h"

namespace caret {

/**
 * \brief Enumerated type for a structure in a brain.
 *
 * Enumerated types for reduction operators.
 */
class ReductionEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
            INVALID,
            MAX,
            MIN,
            SUM,
            MEAN,
            STDEV,
            SAMPSTDEV,
            VARIANCE,
            MEDIAN,
            MODE
    };

    ~ReductionEnum();

    static AString toName(Enum enumValue);
    
    static AString toExplanation(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static void getAllEnums(std::vector<Enum>& allEnums);

private:
    ReductionEnum(const Enum enumValue, 
                 const AString& name,
                 const AString& explanation);

    static const ReductionEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<ReductionEnum> enumData;

    /** Initialize instances that contain the enum values and metadata */
    static void initialize();

    /** Indicates instance of enum values and metadata have been initialized */
    static bool initializedFlag;
    
    /** The enumerated type value for an instance */
    Enum enumValue;

    /** The name, a text string that is identical to the enumerated value */
    AString name;
    
    /** An explanation of the function */
    AString explanation;
};

} // namespace
#endif  //__REDUCTION_ENUM_H__