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

#include "CiftiMatrix.h"
#include "zlib.h"
#include "QFile"
#include "ByteSwapping.h"
#include "qtemporaryfile.h"
#include <FileInformation.h>
#include <qdir.h>
#ifdef CARET_OS_WINDOWS
//#include <io.h>
#include <unistd.h>
#else //not CARET_OS_WINDOWS
#include <unistd.h>
#endif //ifdef CARET_OS_WINDOWS
using namespace caret;
using namespace std;

CiftiMatrix::CiftiMatrix()
{
    init();
}

CiftiMatrix::CiftiMatrix(const AString &fileNameIn, const CacheEnum e, const AString &cacheFile)
{
    init();
    m_fileName = fileNameIn;
    m_cacheFileName = cacheFile;
    m_caching = e;
}

void CiftiMatrix::init()
{
    m_beenInitialized = false;
    m_caching = IN_MEMORY;
    m_matrixOffset = 0;
    matrixChanged = false;
}

void CiftiMatrix::deleteCache()
{
    m_matrix = CaretArray<float>();//deallocate by assigning default constructed - basically NULL
}

QFile * CiftiMatrix::getCacheFile()
{
    return this->m_cacheFile;
}

void CiftiMatrix::setup(vector<int64_t> &dimensions, const int64_t &offsetIn, const CacheEnum &e, const bool &needsSwapping) throw (CiftiFileException)
{
    m_needsSwapping = needsSwapping;
    
    if(dimensions.size() != 2)
    {
        throw CiftiFileException("Cifti only supports 2 dimensional matrices currently");
    }
    m_dimensions = dimensions;

    m_matrixOffset = offsetIn;
    m_caching = e;
    m_beenInitialized = true;    
    if(m_caching == IN_MEMORY)
    {
        int64_t matrixSize = m_dimensions[0]*m_dimensions[1];
        m_matrix = CaretArray<float>(matrixSize);
        if(!QFile::exists(m_fileName)) return;
        
        m_file.grabNew(NULL);
        m_readFile.grabNew(NULL);
        m_cacheFile.grabNew(NULL);

        m_file.grabNew(new QFile());
        m_file->setFileName(m_fileName);        
        m_file->open(QIODevice::ReadOnly);
#if 0        
        file->seek(m_matrixOffset);//TODO, see if QT has fixed reading large files
        //otherwise use stdio for this read...
        file->read((char *)m_matrix,matrixSize*sizeof(float));
#endif
        //QT sucks and is unable to handle reading from files over 2GB's in size
        int fh = m_file->handle();
#ifdef CARET_OS_WINDOWS
        _lseek(fh,m_matrixOffset,0);
#else
        lseek(fh,m_matrixOffset,0);
#endif
        int64_t rowSize = this->m_dimensions[m_dimensions.size()-1];
        int64_t columnSize = this->m_dimensions[m_dimensions.size()-2];
        for(int64_t i=0;i<columnSize;i++)//apparently read also is unable to handle reading in more than 2GB's at a time, reading a row at a time to get around this
        {
#ifdef CARET_OS_WINDOWS
            if (_read(fh,(char *)&m_matrix[i*rowSize],rowSize*sizeof(float)) != rowSize*sizeof(float)) throw CiftiFileException("error reading from file");
#else
            if (read(fh,(char *)&m_matrix[i*rowSize],rowSize*sizeof(float)) != (ssize_t)(rowSize*sizeof(float))) throw CiftiFileException("error reading from file");
#endif
        }
        m_file->close();
        if(m_needsSwapping)ByteSwapping::swapBytes(m_matrix,matrixSize);
    }
    else
    {
           
        m_file.grabNew(NULL);
        m_readFile.grabNew(NULL);
        m_cacheFile.grabNew(NULL);

        //set up cache file
        if(m_cacheFileName.length() == 0)
        {
            CaretPointer<QTemporaryFile> tf(new QTemporaryFile());
            tf->setAutoRemove(true);
            m_cacheFile = tf;
            if (!m_cacheFile->open(QIODevice::ReadWrite)) throw CiftiFileException("failed to open temporary file for writing");
        }
        else
        {
            CaretPointer<QFile> f(new QFile());
            f->setFileName(m_cacheFileName);
            if (!f->open(QIODevice::ReadWrite | QIODevice::Truncate)) throw CiftiFileException("failed to open file '" + m_cacheFileName + "' for writing");
            m_cacheFile = f;
        }

        if(!QFile::exists(m_fileName))
        {            
            m_readFile = m_cacheFile;//read and write happen on the same file handle when in write only mode            
        }
        else
        {
            m_file.grabNew(new QFile());
            m_file->setFileName(m_fileName);
            if (!m_file->open(QIODevice::ReadOnly)) throw CiftiFileException("failed to open file '" + m_fileName + "' for reading");
            m_readFile = m_file;
        }
    }    
}

void CiftiMatrix::setMatrixFile(const AString &fileNameIn, const AString &cacheFileIn)
{
    deleteCache();
    m_file.grabNew(NULL);
    m_readFile.grabNew(NULL);
    m_cacheFile.grabNew(NULL);
    
    init();
    m_fileName = fileNameIn;
    m_cacheFileName = cacheFileIn;
}

void CiftiMatrix::getMatrixFile(AString &fileNameOut)
{
    fileNameOut = m_fileName;
}

void CiftiMatrix::getMatrixDimensions(vector<int64_t> &dimensions) const
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    dimensions = m_dimensions;
}

/*void CiftiMatrix::setCaching(const CacheEnum &e)
{
    m_caching = e;
}*/

void CiftiMatrix::getCaching(CacheEnum &e) const
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    e = m_caching;
}

void CiftiMatrix::getMatrixOffset(int64_t &offsetOut)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    offsetOut = m_matrixOffset;
}

//Matrix IO
void CiftiMatrix::getRow(float *rowOut, const int64_t &rowIndex, const bool& tolerateShortRead) const throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    if(m_caching == IN_MEMORY)
    {
        memcpy((char *)rowOut, (char *)&m_matrix[rowIndex*m_dimensions[1]], m_dimensions[1]*sizeof(float));
    }
    else if(m_caching == ON_DISK)
    {
        qint64 numRead;
        {
            CaretMutexLocker locked(&m_fileMutex);
            if (!m_readFile->seek(m_matrixOffset+rowIndex*m_dimensions[1]*sizeof(float))) throw CiftiFileException("error seeking in file, file may be truncated");
            numRead = m_readFile->read((char *)rowOut,m_dimensions[1]*sizeof(float));
        }
        if (numRead != (qint64)(m_dimensions[1]*sizeof(float)) && !tolerateShortRead)
        {
            throw CiftiFileException("error reading row, file may be truncated");
        }
        if(m_needsSwapping) ByteSwapping::swapBytes(rowOut,m_dimensions[1]);//swap the whole thing even if it was a partial read, just for simplicity
    }
}

void CiftiMatrix::setRow(float *rowIn, const int64_t &rowIndex) throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    if(m_caching == IN_MEMORY)
    {
        memcpy((char *)&m_matrix[rowIndex*m_dimensions[1]], (char *)rowIn, m_dimensions[1]*sizeof(float));
    }
    else if(m_caching == ON_DISK)
    {
        {
            CaretMutexLocker locked(&m_fileMutex);
            if(!matrixChanged) updateCache();
            if(m_needsSwapping) ByteSwapping::swapBytes(rowIn,m_dimensions[1]);//TSC: after updateCache(), both files are the same! m_swapping is valid for both
            if (!m_cacheFile->seek(m_matrixOffset+rowIndex*m_dimensions[1]*sizeof(float))) throw CiftiFileException("error seeking in file, file may be truncated");
            if (m_cacheFile->write((char *)rowIn,m_dimensions[1]*sizeof(float)) != (qint64)(m_dimensions[1]*sizeof(float))) throw CiftiFileException("error writing to file, file may be truncated");
        }
        if(m_needsSwapping) ByteSwapping::swapBytes(rowIn,m_dimensions[1]);
    }
}

void CiftiMatrix::updateCache()
{
    matrixChanged = true;
    if(m_readFile != m_cacheFile)
    {
        copyMatrix(m_cacheFile, m_readFile, m_needsSwapping);
        m_readFile = m_cacheFile;//writes AND reads are now done on cache
        m_needsSwapping = false;//and cache is never swapped if it wasn't the reading file, so set this to false
    }
}

void CiftiMatrix::copyMatrix(QFile *output, QFile *input, const bool& needsSwapping)
{
    input->seek(this->m_matrixOffset);
    output->seek(this->m_matrixOffset);
    int64_t rowSize = m_dimensions[1];
    int64_t columnSize = m_dimensions[0];
    CaretArray<float> row(rowSize);
    input->seek(m_matrixOffset);
    output->seek(m_matrixOffset);
    for(int64_t i =0;i<columnSize;i++)
    {            
        input->read((char *)row.getArray(),rowSize*sizeof(float));
        if (needsSwapping) ByteSwapping::swapBytes(row, rowSize);
        output->write((char *)row.getArray(), rowSize*sizeof(float));
    }
}

void CiftiMatrix::getColumn(float *columnOut, const int64_t &columnIndex) const throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    int64_t rowSize = m_dimensions[1];
    int64_t columnSize = m_dimensions[0];
    if(m_caching == IN_MEMORY)
    {
        for(int64_t i=0;i<columnSize;i++){ columnOut[i]=m_matrix[columnIndex+rowSize*i]; }
    }
    else if(m_caching == ON_DISK)
    {
        {
            CaretMutexLocker locked(&m_fileMutex);
            //let's hope that people aren't stupid enough to think this is fast...    
            for(int64_t i=0;i<columnSize;i++)
            {
                if (!m_readFile->seek(m_matrixOffset+(columnIndex + i*rowSize)*sizeof(float))) throw CiftiFileException("error seeking in file, file may be truncated");
                if (m_readFile->read((char *)&columnOut[i],sizeof(float)) != sizeof(float)) throw CiftiFileException("error reading from file, file may be truncated");
            }
        }
        if(m_needsSwapping) ByteSwapping::swapBytes(columnOut,columnSize);
    }
}

void CiftiMatrix::setColumn(float *columnIn, const int64_t &columnIndex) throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    int64_t rowSize = m_dimensions[1];
    int64_t columnSize = m_dimensions[0];
    if(m_caching == IN_MEMORY)
    {
        for(int64_t i=0;i<columnSize;i++){ m_matrix[columnIndex+rowSize*i]=columnIn[i]; }
    }
    else if(m_caching == ON_DISK)
    {
        {
            CaretMutexLocker locked(&m_fileMutex);
            if(!matrixChanged) updateCache();
            if(m_needsSwapping) ByteSwapping::swapBytes(columnIn,columnSize);//TSC: after updateCache(), both files are the same! m_swapping is valid for both
            for(int64_t i=0;i<columnSize;i++)
            {
                if (!m_cacheFile->seek(m_matrixOffset+(columnIndex + i*rowSize)*sizeof(float))) throw CiftiFileException("error seeking in file, file may be truncated");
                if (m_cacheFile->write((char *)&columnIn[i],sizeof(float)) != sizeof(float)) throw CiftiFileException("error writing to file, file may be truncated");
            }
        }
        if(m_needsSwapping) ByteSwapping::swapBytes(columnIn,columnSize);
    }
}

void CiftiMatrix::getMatrix(float *matrixOut) throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    int64_t matrixLength = m_dimensions[0]*m_dimensions[1];
    if(m_caching == IN_MEMORY)
    {
        memcpy((char *)matrixOut,(char *)m_matrix.getArray(),matrixLength*sizeof(float));
    }
    else if(m_caching == ON_DISK)
    {//TODO, see if QT has fixed reading large files
        {
            CaretMutexLocker locked(&m_fileMutex);
            if (!m_readFile->seek(m_matrixOffset)) throw CiftiFileException("error seeking in file, file may be truncated");
            //otherwise use stdio for this read...
            if (m_readFile->read((char *)matrixOut,matrixLength*sizeof(float)) != (qint64)(matrixLength*sizeof(float))) throw CiftiFileException("error reading from file, file may be truncated (or larger than QFile::read can handle)");
        }
        if(m_needsSwapping) ByteSwapping::swapBytes(matrixOut,matrixLength);
    }
}

void CiftiMatrix::setMatrix(float *matrixIn) throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    int64_t matrixLength = m_dimensions[0]*m_dimensions[1];
    if(m_caching == IN_MEMORY)
    {
        memcpy((char *)m_matrix.getArray(),(char *)matrixIn,matrixLength*sizeof(float));
    }
    else if(m_caching == ON_DISK)
    {
        {
            CaretMutexLocker locked(&m_fileMutex);
            if(!matrixChanged) updateCache();//TODO, see if QT has fixed reading large files
            if(m_needsSwapping) ByteSwapping::swapBytes(matrixIn,matrixLength);//TSC: after updateCache(), both files are the same! m_swapping is valid for both
            if (!m_cacheFile->seek(m_matrixOffset)) throw CiftiFileException("error seeking in file, file may be truncated");
            //otherwise use stdio for this read...
            if (m_cacheFile->write((char *)matrixIn,matrixLength*sizeof(float)) != (qint64)(matrixLength*sizeof(float))) throw CiftiFileException("error writing to file, file may be truncated (or larger than QFile::write can handle)");
        }
        if(m_needsSwapping) ByteSwapping::swapBytes(matrixIn,matrixLength);
    }
}

CiftiMatrix::~CiftiMatrix()
{
    deleteCache();
}

/*
void CiftiMatrix::flushCache() throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    if(m_caching == IN_MEMORY)
    {
        int64_t matrixSize = m_dimensions[0]*m_dimensions[1];
        file->setFileName(m_fileName);
        file->open(QIODevice::ReadWrite);
        file->seek(m_matrixOffset);//TODO, see if QT has fixed reading large files
        //otherwise use stdio for this read...
        if(m_needsSwapping)ByteSwapping::swapBytes(m_matrix,matrixSize);
        file->write((char *)m_matrix,matrixSize*sizeof(float));
        file->close();
        if(m_needsSwapping)ByteSwapping::swapBytes(m_matrix,matrixSize);
    }
}*/

void CiftiMatrix::writeToNewFile(const AString &fileNameIn, const int64_t  &offsetIn) throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");

    if(m_caching == IN_MEMORY)
    {
        QFile outFile;
        outFile.setFileName(fileNameIn);
        outFile.open(QIODevice::ReadWrite);
        outFile.seek(offsetIn);
        int64_t matrixLength = m_dimensions[0]*m_dimensions[1];
        outFile.write((char *)m_matrix.getArray(),matrixLength*sizeof(float));
        outFile.close();
    }
    else if(m_caching == ON_DISK)
    {
        CaretMutexLocker locked(&m_fileMutex);//not really sure what all in here needs to be protected, but lock anyway - DO NOT call getRow or setRow with this locked
        if(!m_cacheFile) throw CiftiFileException("Cifti matrix is setup to cache on disk, but no cache file exists.");
        if(QDir::toNativeSeparators(m_cacheFile->fileName()) == QDir::toNativeSeparators(fileNameIn))
        {
            if(!matrixChanged) //the writes have already been written to the cache file
            {
                this->updateCache();
            }
            this->m_cacheFile->flush();
            return;
        }
              
        int64_t rowSize = m_dimensions[1];
        int64_t columnSize = m_dimensions[0];
        float * row = new float[rowSize];
        m_readFile->seek(m_matrixOffset);
        QFile outFile;
        outFile.setFileName(fileNameIn);
        outFile.open(QIODevice::ReadWrite);
        outFile.seek(offsetIn);
        for(int64_t i =0;i<columnSize;i++)
        {            
            if (m_readFile->read((char *)row,rowSize*sizeof(float)) != (qint64)(rowSize * sizeof(float))) throw CiftiFileException("error reading from file, file may be truncated");
            if (m_needsSwapping) ByteSwapping::swapBytes(row,rowSize);
            if (outFile.write((char *)row, rowSize*sizeof(float)) != (qint64)(rowSize * sizeof(float))) throw CiftiFileException("error writing to file, file may be truncated");
        }
        delete[] row;
        outFile.close();
    }
    
}
