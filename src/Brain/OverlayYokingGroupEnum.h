#ifndef __OVERLAY_YOKING_GROUP_ENUM_H__
#define __OVERLAY_YOKING_GROUP_ENUM_H__

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


#include <stdint.h>
#include <vector>
#include "AString.h"

namespace caret {

class OverlayYokingGroupEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** Off */
        OVERLAY_YOKING_GROUP_OFF,
        /** Group 1 */
        OVERLAY_YOKING_GROUP_1,
        /** Group 2 */
        OVERLAY_YOKING_GROUP_2,
        /** Group 3*/
        OVERLAY_YOKING_GROUP_3,
        /** Group 4 */
        OVERLAY_YOKING_GROUP_4,
        /** Group 5 */
        OVERLAY_YOKING_GROUP_5,
        /** Group 6 */
        OVERLAY_YOKING_GROUP_6,
        /** Group 7 */
        OVERLAY_YOKING_GROUP_7,
        /** Group 8 */
        OVERLAY_YOKING_GROUP_8,
        /** Group 9 */
        OVERLAY_YOKING_GROUP_9,
        /** Group 10 */
        OVERLAY_YOKING_GROUP_10
    };


    ~OverlayYokingGroupEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toGuiName(Enum enumValue);
    
    static Enum fromGuiName(const AString& guiName, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum enumValue);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums);

    static void getAllNames(std::vector<AString>& allNames, const bool isSorted);

    static void getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted);

    static int32_t getSelectedMapIndex(const Enum enumValue);
    
    static void setSelectedMapIndex( const Enum enumValue,
                                       const int32_t mapIndex);
    
private:
    OverlayYokingGroupEnum(const Enum enumValue, 
                 const AString& name,
                 const AString& guiName);

    static OverlayYokingGroupEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<OverlayYokingGroupEnum> enumData;

    /** Initialize instances that contain the enum values and metadata */
    static void initialize();

    /** Indicates instance of enum values and metadata have been initialized */
    static bool initializedFlag;
    
    /** Auto generated integer codes */
    static int32_t integerCodeCounter;
    
    /** The enumerated type value for an instance */
    Enum enumValue;

    /** The integer code associated with an enumerated value */
    int32_t integerCode;

    /** The name, a text string that is identical to the enumerated value */
    AString name;
    
    /** A user-friendly name that is displayed in the GUI */
    AString guiName;
    
    /** Index of the selected map */
    int32_t mapIndex;
};

#ifdef __OVERLAY_YOKING_GROUP_ENUM_DECLARE__
std::vector<OverlayYokingGroupEnum> OverlayYokingGroupEnum::enumData;
bool OverlayYokingGroupEnum::initializedFlag = false;
int32_t OverlayYokingGroupEnum::integerCodeCounter = 0; 
#endif // __OVERLAY_YOKING_GROUP_ENUM_DECLARE__

} // namespace
#endif  //__OVERLAY_YOKING_GROUP_ENUM_H__
