
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

#define __CARET_DATA_FILE_DECLARE__
#include "CaretDataFile.h"
#undef __CARET_DATA_FILE_DECLARE__

#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::CaretDataFile 
 * \brief A data file with abstract methods for caret data
 *
 * This class is essentially an interface that defines methods
 * that are supported by most Caret Data Files.
 */
/**
 * Constructor.
 */
CaretDataFile::CaretDataFile(const DataFileTypeEnum::Enum dataFileType)
: DataFile(),
SceneableInterface()
{
    m_dataFileType = dataFileType;
    m_displayedInGuiFlag = false;
    
    AString name = (DataFileTypeEnum::toName(m_dataFileType).toLower()
                    + "_file_"
                    + AString::number(s_defaultFileNameCounter)
                    + "."
                    + DataFileTypeEnum::toFileExtension(m_dataFileType));
    s_defaultFileNameCounter++;
    
    setFileName(name);
}

/**
 * Destructor.
 */
CaretDataFile::~CaretDataFile()
{
    
}

/**
 * @return The type of this data file.
 */
DataFileTypeEnum::Enum 
CaretDataFile::getDataFileType() const
{
    return m_dataFileType; 
}

/**
 * Override the default data type for the file.
 * Use this with extreme caution as using a type invalid
 * with the file may cause disaster.
 * 
 * @param dataFileType
 *    New value for file's data type.
 */
void 
CaretDataFile::setDataFileType(const DataFileTypeEnum::Enum dataFileType)
{
    m_dataFileType = dataFileType;
}

/**
 * Copy constructor.
 * @param cdf
 *    Instance that is copied to this.
 */
CaretDataFile::CaretDataFile(const CaretDataFile& cdf)
: DataFile(cdf),
SceneableInterface(cdf)
{
    copyDataCaretDataFile(cdf);
}

/**
 * Assignment operator.
 * @param cdf
 *    Instance that is assigned to this.
 */
CaretDataFile& 
CaretDataFile::operator=(const CaretDataFile& cdf)
{
    if (this != &cdf) {
        DataFile::operator=(cdf);
        copyDataCaretDataFile(cdf);
    }
    return *this;
}

/**
 * Assists with copying instances of this class.
 * @param cdf
 *    Instance that is copied to this.
 */
void 
CaretDataFile::copyDataCaretDataFile(const CaretDataFile& cdf)
{
    m_dataFileType = cdf.m_dataFileType;
}

/**
 * @return Is this file displayed in the graphical user-interface?
 *
 * Note: Before calling this method, Brain::determineDisplayedDataFiles()
 * must be called.  It will set the displayed status for all 
 * files that is owns.
 */
bool 
CaretDataFile::isDisplayedInGUI() const
{
    return m_displayedInGuiFlag;
}

/**
 * Set the displayed in graphical user-interface status.
 * @param displayedInGUI
 *    New status of displayed in GUI.
 *
 * Note: This method is called by Brain::determineDisplayedDataFiles().
 */
void 
CaretDataFile::setDisplayedInGUI(const bool displayedInGUI)
{
    m_displayedInGuiFlag = displayedInGUI;
}

/**
 * Set the username and password for reading files, typically from
 * a database or website.
 *
 * @param username
 *     Account's username.
 * @param password
 *     Account's password.
 */
void
CaretDataFile::setFileReadingUsernameAndPassword(const AString& username,
                                                 const AString& password)
{
    s_fileReadingUsername = username;
    s_fileReadingPassword = password;
}

/**
 * @return The username for file reading from database or website.
 */
AString
CaretDataFile::getFileReadingUsername()
{
    return s_fileReadingUsername;
}

/**
 * @return The password for file reading from database or website.
 */
AString
CaretDataFile::getFileReadingPassword()
{
    return s_fileReadingPassword;
}

/**
 * Create a scene for an instance of a class.
 *
 * NOTE: In most cases, subclasses should not override this method but
 * instead override  saveFileDataToScene() use it to add data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  If there is no data for the scene, a NULL pointer
 *    will be returned.
 */
SceneClass*
CaretDataFile::saveToScene(const SceneAttributes* sceneAttributes,
                                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "CaretDataFile",
                                            1);
    
    saveFileDataToScene(sceneAttributes,
                        sceneClass);
    
    if (sceneClass->getNumberOfObjects() <= 0) {
        delete sceneClass;
        sceneClass = NULL;
    }
    //    const int32_t numMaps = getNumberOfMaps();
    //    if (numMaps > 0) {
    //        bool* mapEnabledArray = new bool[numMaps];
    //        for (int32_t i = 0; i < numMaps; i++) {
    //            mapEnabledArray[i] = m_mapContent[i]->m_dataLoadingEnabled;
    //        }
    //
    //        sceneClass->addBooleanArray("mapEnabled",
    //                                    mapEnabledArray,
    //                                    numMaps);
    //        delete[] mapEnabledArray;
    //    }
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 *
 * NOTE: In most cases, subclasses should not override this method but
 * instead override restoreFileDataFromScene() use it to access 
 * data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
CaretDataFile::restoreFromScene(const SceneAttributes* sceneAttributes,
                                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    restoreFileDataFromScene(sceneAttributes,
                             sceneClass);
}

/**
 * Save file data from the scene.  For subclasses that need to
 * save to a scene, this method should be overriden.  sceneClass
 * will be valid and any scene data should be added to it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.
 */
void
CaretDataFile::saveFileDataToScene(const SceneAttributes* /*sceneAttributes*/,
                                           SceneClass* /*sceneClass*/)
{
    /* Nothing as subclasses needing to save to scenes will override. */
}

/**
 * Restore file data from the scene.  For subclasses that need to
 * restore from a scene, this method should be overridden. The scene class
 * will be valid and any scene data may be obtained from it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
CaretDataFile::restoreFileDataFromScene(const SceneAttributes* /*sceneAttributes*/,
                                                const SceneClass* /*sceneClass*/)
{
    /* Nothing as subclasses needing to restore from scenes will override. */
}

/**
 * @return True if this file type supports writing, else false.
 *
 * By default, this method returns true.  Files that do not support
 * writing should override this method and return false.
 */
bool
CaretDataFile::supportsWriting() const
{
    return true;
}





