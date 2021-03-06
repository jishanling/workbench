#ifndef __APPLICATION_INFORMATION__H_
#define __APPLICATION_INFORMATION__H_

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

    class ApplicationInformation : public CaretObject {
        
    public:
        ApplicationInformation();
        
        virtual ~ApplicationInformation();
        
        AString getName() const;
        
        AString getVersion() const;
        
        void getAllInformation(std::vector<AString>& informationValues) const;
        
        AString getAllInformationInString(const AString& separator) const;
        
        AString getCompiledWithDebugStatus() const;
        
    private:
        ApplicationInformation(const ApplicationInformation&);

        ApplicationInformation& operator=(const ApplicationInformation&);
        
        AString name;
        
        AString version;
        
        AString commit;
        
        AString commitDate;
        
        AString compiledWithDebugOn;
        
        AString operatingSystemName;
    };
    
#ifdef __APPLICATION_INFORMATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __APPLICATION_INFORMATION_DECLARE__

} // namespace
#endif  //__APPLICATION_INFORMATION__H_
