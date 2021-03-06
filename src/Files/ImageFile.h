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

#ifndef __IMAGE_FILE_H__
#define __IMAGE_FILE_H__

#include "DataFile.h"

class QImage;

namespace caret {
    
/// File for images
class ImageFile : public DataFile {
public:
    /**
     * Location of origin in image data.
     */
    enum IMAGE_DATA_ORIGIN_LOCATION {
        /** Origin at bottom (OpenGL has origin at bottom) */
        IMAGE_DATA_ORIGIN_AT_BOTTOM,
        /** Origin at top (most image formats have origin at top) */
        IMAGE_DATA_ORIGIN_AT_TOP
    };
    
    ImageFile();
    
    ImageFile(const unsigned char* imageDataRGBA,
              const int imageWidth,
              const int imageHeight,
              const IMAGE_DATA_ORIGIN_LOCATION imageOrigin);
    
    ImageFile(const QImage& img);
    
    ~ImageFile();
    
    void appendImageAtBottom(const ImageFile& img);
    
    void clear();
    
    virtual bool compareFileForUnitTesting(const DataFile* df,
                                           const float tolerance,
                                           AString& messageOut) const;
    
    bool isEmpty() const;
    
    //QImage* getAsQImage();
    
    const QImage* getAsQImage() const;
    
    void setFromQImage(const QImage& img);
    
    virtual void readFile(const AString& filename) throw (DataFileException);
    
    virtual void writeFile(const AString& filename) throw (DataFileException);
    
    void cropImageRemoveBackground(const int marginSize,
                                   const uint8_t backgroundColor[3]);
    
    void findImageObject(const uint8_t backgroundColor[3],
                         int objectBoundsOut[4]) const; 
    
    void addMargin(const int marginSize,
                   const uint8_t backgroundColor[3]);
    
    void addMargin(const int marginSizeX,
                   const int marginSizeY,
                   const uint8_t backgroundColor[3]);
    
    void setDotsPerMeter(const int x,
                         const int y);
    
    void resizeToMaximumWidthOrHeight(const int32_t maximumWidthOrHeight);
    
    void resizeToMaximumWidth(const int32_t maximumWidth);
    
    void resizeToMaximumHeight(const int32_t maximumHeight);
    
    void resizeToWidth(const int32_t width);
    
    void getImageInByteArray(QByteArray& byteArrayOut,
                             const AString& format) const throw (DataFileException);
    
    void setImageFromByteArray(const QByteArray& byteArray,
                               const AString& format) throw (DataFileException);
    
    void combinePreservingAspectAndFillIfNeeded(const std::vector<ImageFile*>& imageFiles,
                                                const int numImagesPerRow,
                                                const uint8_t backgroundColor[3]);
    
    static void getImageFileExtensions(std::vector<AString>& imageFileExtensions,
                                       AString& defaultExtension);
    
    static void getImageFileFilters(std::vector<AString>& imageFileFilters,
                                    AString& defaultFilter);

private:
    ImageFile(const ImageFile&);
    
    ImageFile& operator=(const ImageFile&);
    
    void insertImage(const QImage& otherImage,
                     const int x,
                     const int y) throw (DataFileException);
    
    static void insertImage(const QImage& insertThisImage,
                            QImage& intoThisImage,
                            const int positionX,
                            const int positionY) throw (DataFileException);
    
    QImage* m_image;
};

} // namespace

#endif // __IMAGE_FILE_H__
