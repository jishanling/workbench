#ifndef __CARET_PREFERENCES__H_
#define __CARET_PREFERENCES__H_

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

#include <utility>

#include "CaretObject.h"
#include "LogLevelEnum.h"
#include "ImageCaptureMethodEnum.h"
#include "OpenGLDrawingMethodEnum.h"

class QSettings;
class QStringList;

namespace caret {

    class ModelTransform;
    class TileTabsConfiguration;
    
    class CaretPreferences : public CaretObject {
        
    public:
        CaretPreferences();
        
        virtual ~CaretPreferences();
        
        void getColorForeground(uint8_t colorForeground[3]) const;

        void getColorForeground(float colorForeground[3]) const;
        
        void setColorForeground(const uint8_t colorForeground[3]);
        
        void getColorBackground(uint8_t colorBackground[3]) const;
        
        void getColorBackground(float colorBackground[3]) const;
        
        void setColorBackground(const uint8_t colorBackground[3]);
        
        void getPreviousSpecFiles(std::vector<AString>& previousSpecFiles) const;
        
        void addToPreviousSpecFiles(const AString& specFileName);
        
        void clearPreviousSpecFiles();
        
        void getPreviousOpenFileDirectories(std::vector<AString>& previousOpenFileDirectories) const;
        
        void getPreviousOpenFileDirectories(QStringList& previousOpenFileDirectories) const;
        
        void addToPreviousOpenFileDirectories(const AString& directoryName);
        
        LogLevelEnum::Enum getLoggingLevel() const;
        
        void setLoggingLevel(const LogLevelEnum::Enum loggingLevel);
        
        ImageCaptureMethodEnum::Enum getImageCaptureMethod() const;
        
        void setImageCaptureMethod(const ImageCaptureMethodEnum::Enum imageCaptureMethod);
        
        OpenGLDrawingMethodEnum::Enum getOpenDrawingMethod() const;
        
        void setOpenGLDrawingMethod(const OpenGLDrawingMethodEnum::Enum openGLDrawingMethod);
        
        bool isVolumeAxesCrosshairsDisplayed() const;
        
        void setVolumeAxesCrosshairsDisplayed(const bool displayed);
        
        bool isVolumeAxesLabelsDisplayed() const;
        
        void setVolumeAxesLabelsDisplayed(const bool displayed);
        
        bool isVolumeMontageAxesCoordinatesDisplayed() const;
        
        void setVolumeMontageAxesCoordinatesDisplayed(const bool displayed);
        
        int32_t getVolumeMontageGap() const;
        
        void setVolumeMontageGap(const int32_t volumeMontageGap);
        
        int32_t getVolumeMontageCoordinatePrecision() const;
        
        void setVolumeMontageCoordinatePrecision(const int32_t volumeMontageCoordinatePrecision);
        
        void setAnimationStartTime(const double &time);
        
        void getAnimationStartTime(double &time);
        
        //bool isContralateralIdentificationEnabled() const;
        
        //void setContralateralIdentificationEnabled(const bool enabled);
        
        bool isSplashScreenEnabled() const;
        
        void setSplashScreenEnabled(const bool enabled);
        
        bool isDevelopMenuEnabled() const;
        
        void setDevelopMenuEnabled(const bool enabled);
        
        int32_t getToolBoxType() const;
        
        void setToolBoxType(const int32_t toolBoxType);
        
        void readTileTabsConfigurations(const bool performSync = true);
        
        std::vector<const TileTabsConfiguration*> getTileTabsConfigurationsSortedByName() const;
        
        TileTabsConfiguration* getTileTabsConfigurationByUniqueIdentifier(const AString& uniqueIdentifier);
        
        const TileTabsConfiguration* getTileTabsConfigurationByUniqueIdentifier(const AString& uniqueIdentifier) const;
        
//        std::vector<AString> getTileTabsConfigurationUniqueIdentifiers() const;
        
//        std::vector<AString> getTileTabsConfigurationNames() const;
        
        TileTabsConfiguration* getTileTabsConfigurationByName(const AString& name) const;
        
//        bool getTileTabsConfigurationByUniqueIdentifier(const AString& tileTabsUniqueIdentifier,
//                                            TileTabsConfiguration& tileTabsConfigurationOut) const;
        
        void addTileTabsConfiguration(TileTabsConfiguration* tileTabsConfiguration);
        
//        void addOrReplaceTileTabsConfiguration(const TileTabsConfiguration& tileTabsConfiguration);
        
        void removeTileTabsConfigurationByUniqueIdentifier(const AString& tileTabsUniqueIdentifier);
        
        void writeTileTabsConfigurations();
        
        void readCustomViews(const bool performSync = true);
        
        std::vector<AString> getCustomViewNames() const;
        
        std::vector<std::pair<AString,AString> > getCustomViewNamesAndComments() const;
        
        bool getCustomView(const AString& customViewName,
                           ModelTransform& modelTransformOut) const;
        
        void addOrReplaceCustomView(const ModelTransform& modelTransform);
        
        void removeCustomView(const AString& customViewName);
        
        bool isRemoteFilePasswordSaved();
        
        void setRemoteFilePasswordSaved(const bool saveRemotePasswordToPreferences);

        void getRemoteFileUserNameAndPassword(AString& userNameOut,
                                              AString& passwordOut) const;
        
        void setRemoteFileUserNameAndPassword(const AString& userName,
                                              const AString& password);
        
//        std::vector<ModelTransform*> getAllModelTransforms();
//        
//        void setAllModelTransforms(std::vector<ModelTransform*>& allModelTransforms);
//        
//        const ModelTransform* getModelTransform(const AString& viewName);
//        
//        void addModelTransform(const ModelTransform& ModelTransform);
//        
//        void removeModelTransform(const AString& viewName);
        
    private:
        CaretPreferences(const CaretPreferences&);

        CaretPreferences& operator=(const CaretPreferences&);
        
    public:
        virtual AString toString() const;
        
    private:
        bool getBoolean(const AString& name,
                        const bool defaultValue = false);
        
        void setBoolean(const AString& name,
                        const bool value);
        
        int getInteger(const AString& name,
                        const int defaultValue = false);
        
        void setInteger(const AString& name,
                        const int value);
        
        AString getString(const AString& name,
                          const AString& defaultValue = "");
        
        void setString(const AString& name,
                       const AString& value);
        
        void addToPrevious(std::vector<AString>& previousVector,
                           const AString& newName);
        
        void readPreferences();
        
        void removeAllTileTabsConfigurations();
        
        void removeAllCustomViews();
        
        void writeCustomViews();
        
        mutable QSettings* qSettings;
        
        uint8_t colorForeground[3];
        
        uint8_t colorBackground[3];
        
        std::vector<AString> previousSpecFiles;
        
        std::vector<AString> previousOpenFileDirectories;
        
        LogLevelEnum::Enum loggingLevel;
        
        ImageCaptureMethodEnum::Enum imageCaptureMethod;
        
        OpenGLDrawingMethodEnum::Enum openGLDrawingMethod;
        
        std::vector<ModelTransform*> customViews;

        std::vector<TileTabsConfiguration*> tileTabsConfigurations;
        
        bool displayVolumeAxesCrosshairs;
        
        bool displayVolumeAxesLabels;
        
        bool displayVolumeAxesCoordinates;
        
        int32_t volumeMontageGap;
        
        int32_t volumeMontageCoordinatePrecision;
        
        bool splashScreenEnabled;
        
        bool developMenuEnabled;
        
        double animationStartTime;
        
        int32_t toolBoxType;
        
        AString remoteFileUserName;
        AString remoteFilePassword;
        bool remoteFileLoginSaved;
        
        //bool contralateralIdentificationEnabled;
        
        static const AString NAME_ANIMATION_START_TIME;
        static const AString NAME_VOLUME_AXES_CROSSHAIRS;
        static const AString NAME_VOLUME_AXES_LABELS;
        static const AString NAME_VOLUME_AXES_COORDINATE;
        static const AString NAME_VOLUME_MONTAGE_GAP;
        static const AString NAME_VOLUME_MONTAGE_COORDINATE_PRECISION;
        static const AString NAME_COLOR_BACKGROUND;
        static const AString NAME_COLOR_FOREGROUND;
        static const AString NAME_DEVELOP_MENU;
        static const AString NAME_IMAGE_CAPTURE_METHOD;
//        static const AString NAME_IDENTIFICATION_CONTRALATERAL;
        static const AString NAME_LOGGING_LEVEL;
        static const AString NAME_OPENGL_DRAWING_METHOD;
        static const AString NAME_PREVIOUS_SPEC_FILES;
        static const AString NAME_PREVIOUS_OPEN_FILE_DIRECTORIES;
        static const AString NAME_SPLASH_SCREEN;
        static const AString NAME_CUSTOM_VIEWS;
        static const AString NAME_REMOTE_FILE_USER_NAME;
        static const AString NAME_REMOTE_FILE_PASSWORD;
        static const AString NAME_REMOTE_FILE_LOGIN_SAVED;
        static const AString NAME_TILE_TABS_CONFIGURATIONS;
        
        static const AString NAME_TOOLBOX_TYPE;
    };
    
#ifdef __CARET_PREFERENCES_DECLARE__
    const AString CaretPreferences::NAME_ANIMATION_START_TIME = "animationStartTime";
    const AString CaretPreferences::NAME_VOLUME_AXES_CROSSHAIRS = "volumeAxesCrosshairs";
    const AString CaretPreferences::NAME_VOLUME_AXES_LABELS     = "volumeAxesLabels";
    const AString CaretPreferences::NAME_VOLUME_AXES_COORDINATE     = "volumeAxesCoordinates";
    const AString CaretPreferences::NAME_VOLUME_MONTAGE_GAP     = "volumeMontageGap";
    const AString CaretPreferences::NAME_VOLUME_MONTAGE_COORDINATE_PRECISION     = "volumeMontageCoordinatePrecision";
    const AString CaretPreferences::NAME_COLOR_BACKGROUND     = "colorBackground";
    const AString CaretPreferences::NAME_COLOR_FOREGROUND     = "colorForeground";
    const AString CaretPreferences::NAME_DEVELOP_MENU     = "developMenu";
    const AString CaretPreferences::NAME_IMAGE_CAPTURE_METHOD = "imageCaptureMethod";
    //const AString CaretPreferences::NAME_IDENTIFICATION_CONTRALATERAL     = "identificationContralateral";
    const AString CaretPreferences::NAME_LOGGING_LEVEL     = "loggingLevel";
    const AString CaretPreferences::NAME_OPENGL_DRAWING_METHOD     = "openGLDrawingMethod";
    const AString CaretPreferences::NAME_PREVIOUS_SPEC_FILES     = "previousSpecFiles";
    const AString CaretPreferences::NAME_PREVIOUS_OPEN_FILE_DIRECTORIES     = "previousOpenFileDirectories";
    const AString CaretPreferences::NAME_SPLASH_SCREEN = "splashScreen";
    const AString CaretPreferences::NAME_TOOLBOX_TYPE = "toolBoxType";
    const AString CaretPreferences::NAME_CUSTOM_VIEWS     = "customViews";
    const AString CaretPreferences::NAME_REMOTE_FILE_USER_NAME = "remoteFileUserName";
    const AString CaretPreferences::NAME_REMOTE_FILE_PASSWORD = "remoteFilePassword";
    const AString CaretPreferences::NAME_REMOTE_FILE_LOGIN_SAVED = "removeFileLoginSaved";
    const AString CaretPreferences::NAME_TILE_TABS_CONFIGURATIONS = "tileTabsConfigurations";
#endif // __CARET_PREFERENCES_DECLARE__

} // namespace
#endif  //__CARET_PREFERENCES__H_
