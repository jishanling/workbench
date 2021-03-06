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

#include <QtCore>
#include "iostream"
#include "NiftiHeaderIO.h"
#include <QFile>
#include "zlib.h"
#include "FloatMatrix.h"

using namespace caret;
using namespace std;
/**
 * Constructor
 *
 * Constructor that reads from the specified file handle
 * @param inputFile the input handle that starts with the NiftiHeaderIO
 */
NiftiHeaderIO::NiftiHeaderIO(const AString &inputFileIn) throw (NiftiException)
{
    if(QFile::exists(inputFileIn)) readFile(inputFileIn);
}


/**
 * getSwapNeeded
 *
 * determines whether the byte order needs to be swapped, i.e. if
 * the Nifti file is in big endian format, but the machine architecture
 * is little-endian, then a swap would be necessary
 * @return swapNeeded true if swap is needed
 */
bool NiftiHeaderIO::getSwapNeeded()
{
    return m_swapNeeded;
}

bool NiftiHeaderIO::isCompressed(const AString &fileName) const
{
    if(fileName.length()!=0)
    {
        if(fileName.endsWith(".gz")) return true;
        else return false;
    }
    else return false;
}

void NiftiHeaderIO::readFile(const AString &inputFile) throw (NiftiException)
{
    AString temp = inputFile;
    QDir fpath(temp);
    temp = fpath.toNativeSeparators(temp);
    if(this->isCompressed(inputFile))
    {
        gzFile file = gzopen(temp.toStdString().c_str(),"r");
        readFile(file);
        gzclose(file);
    }
    else
    {
        QFile file(temp);
        file.open(QIODevice::ReadOnly);
        readFile(file);
        file.close();
    }
}



void NiftiHeaderIO::writeFile(const AString &inputFile, NIFTI_BYTE_ORDER byteOrder) throw (NiftiException)
{
    AString temp;
    temp = inputFile;
    QDir fpath(temp);
    temp = fpath.toNativeSeparators(temp);
    if(this->isCompressed(inputFile))
    {
        gzFile file = gzopen(temp.toStdString().c_str(),"w");
        writeFile(file, byteOrder);
        gzclose(file);
    }
    else
    {
        QFile file(temp);
        file.open(QIODevice::WriteOnly);
        writeFile(file, byteOrder);
        file.close();
    }
}

/**
 * readFile
 *
 * reads file from a qfile handle
 * @param header
 */
void NiftiHeaderIO::readFile(QFile &file) throw (NiftiException)
{    
    int bytes_read = 0;
    char bytes[548];
    m_swapNeeded=false;
    niftiVersion = 0;
    nifti_1_header n1header;
    nifti_2_header n2header;
    
    bytes_read = file.read((char *)bytes, sizeof(nifti_1_header));
    memcpy((char *)&n1header, bytes, sizeof(nifti_1_header));
    if(bytes_read < (int)sizeof(nifti_1_header))
    {
        throw NiftiException("Error reading Nifti header, file is too short.");
    }
    else if((NIFTI2_VERSION(n1header))==1)
    {
        niftiVersion=1;
    }
    else if((NIFTI2_VERSION(n1header))==2)
    {
        niftiVersion=2;
        //read the rest of the bytes
        file.read((char *)&bytes[sizeof(nifti_1_header)], sizeof(nifti_2_header) - sizeof(nifti_1_header));
        memcpy((char *)&n2header, bytes, sizeof(nifti_2_header));
    }
    else throw NiftiException("Unrecognized Nifti Version.");

    if(niftiVersion==1)
    {

        if(NIFTI2_NEEDS_SWAP(n1header))
        {
            m_swapNeeded=true;
            swapHeaderBytes(n1header);
        }
        fixDimensions(n1header);
        nifti1Header.setHeaderStuct(n1header);
        nifti1Header.setNeedsSwapping(m_swapNeeded);
    }
    else if(niftiVersion==2)
    {
        if(NIFTI2_NEEDS_SWAP(n2header))
        {
            m_swapNeeded = true;
            swapHeaderBytes(n2header);
        }
        fixDimensions(n2header);
        nifti2Header.setHeaderStuct(n2header);
        nifti2Header.setNeedsSwapping(m_swapNeeded);
    }
}

void NiftiHeaderIO::fixDimensions(nifti_1_header &header)
{
    for(int i = 1;i<8;i++)
    {
        if(i>header.dim[0])
        {
            header.dim[i]=1;
        }
    }
}

void NiftiHeaderIO::fixDimensions(nifti_2_header &header)
{
    for(int i = 1;i<8;i++)
    {
        if(i>header.dim[0])
        {
            header.dim[i]=1;
        }
    }
}

/**
 * readFile
 *
 * reads file from a zFile handle
 * @param header
 */
void NiftiHeaderIO::readFile(gzFile file) throw (NiftiException)
{    
    int bytes_read = 0;
    char bytes[548];
    m_swapNeeded=false;
    niftiVersion = 0;
    nifti_1_header n1header;
    nifti_2_header n2header;

    bytes_read = gzread(file,(char *)bytes, sizeof(nifti_1_header));
    memcpy((char *)&n1header, bytes, sizeof(nifti_1_header));
    if(bytes_read < (int)sizeof(nifti_1_header))
    {
        throw NiftiException("Error reading Nifti header, file is too short.");
    }
    else if(NIFTI2_VERSION(n1header)==1)
    {
        niftiVersion=1;
        fixDimensions(n1header);
    }
    else if(NIFTI2_VERSION(n1header)==2)
    {
        niftiVersion=2;
        //read the rest of the bytes
        gzread(file,(char *)(bytes + sizeof(nifti_1_header)), sizeof(nifti_2_header) - sizeof(nifti_1_header));
        memcpy((char *)&n2header, bytes, sizeof(nifti_2_header));
        fixDimensions(n2header);
    }
    else throw NiftiException("Unrecognized Nifti Version.");

    if(niftiVersion==1)
    {
        if(NIFTI2_NEEDS_SWAP(n1header))
        {
            m_swapNeeded=true;
            swapHeaderBytes(n1header);
        }
        nifti1Header = Nifti1Header();//TSC: reinitialize the header so we don't trip its "set more than once" throw
        nifti1Header.setHeaderStuct(n1header);
        nifti1Header.setNeedsSwapping(m_swapNeeded);
    }
    else if(niftiVersion==2)
    {
        if(NIFTI2_NEEDS_SWAP(n2header))
        {
            m_swapNeeded = true;
            swapHeaderBytes(n2header);
        }
        nifti2Header = Nifti2Header();//ditto
        nifti2Header.setHeaderStuct(n2header);
        nifti2Header.setNeedsSwapping(m_swapNeeded);
    }
}

void NiftiHeaderIO::swapHeaderBytes(nifti_1_header &header)
{
    ByteSwapping::swapBytes(&(header.sizeof_hdr),1);
    ByteSwapping::swapBytes(&(header.datatype),1);
    ByteSwapping::swapBytes(&(header.bitpix),1);
    ByteSwapping::swapBytes(&(header.dim[0]),8);
    ByteSwapping::swapBytes(&(header.intent_p1),1);
    ByteSwapping::swapBytes(&(header.intent_p2),1);
    ByteSwapping::swapBytes(&(header.intent_p3),1);
    ByteSwapping::swapBytes(&(header.pixdim[0]),8);
    ByteSwapping::swapBytes(&(header.vox_offset),1);
    ByteSwapping::swapBytes(&(header.scl_slope),1);
    ByteSwapping::swapBytes(&(header.scl_inter),1);
    ByteSwapping::swapBytes(&(header.cal_max),1);
    ByteSwapping::swapBytes(&(header.cal_min),1);
    ByteSwapping::swapBytes(&(header.slice_duration),1);
    ByteSwapping::swapBytes(&(header.toffset),1);
    ByteSwapping::swapBytes(&(header.slice_start),1);
    ByteSwapping::swapBytes(&(header.slice_end),1);
    ByteSwapping::swapBytes(&(header.qform_code),1);
    ByteSwapping::swapBytes(&(header.sform_code),1);
    ByteSwapping::swapBytes(&(header.quatern_b),1);
    ByteSwapping::swapBytes(&(header.quatern_c),1);
    ByteSwapping::swapBytes(&(header.quatern_d),1);
    ByteSwapping::swapBytes(&(header.qoffset_x),1);
    ByteSwapping::swapBytes(&(header.qoffset_y),1);
    ByteSwapping::swapBytes(&(header.qoffset_z),1);
    ByteSwapping::swapBytes(&(header.srow_x[0]),4);
    ByteSwapping::swapBytes(&(header.srow_y[0]),4);
    ByteSwapping::swapBytes(&(header.srow_z[0]),4);
    //ByteSwapping::swapBytes(&(header.slice_code),1);
    //ByteSwapping::swapBytes(&(header.xyzt_units),1);
    ByteSwapping::swapBytes(&(header.intent_code),1);
}

void NiftiHeaderIO::swapHeaderBytes(nifti_2_header &header)
{
    ByteSwapping::swapBytes(&(header.sizeof_hdr),1);
    ByteSwapping::swapBytes(&(header.datatype),1);
    ByteSwapping::swapBytes(&(header.bitpix),1);
    ByteSwapping::swapBytes(&(header.dim[0]),8);
    ByteSwapping::swapBytes(&(header.intent_p1),1);
    ByteSwapping::swapBytes(&(header.intent_p2),1);
    ByteSwapping::swapBytes(&(header.intent_p3),1);
    ByteSwapping::swapBytes(&(header.pixdim[0]),8);
    ByteSwapping::swapBytes(&(header.vox_offset),1);
    ByteSwapping::swapBytes(&(header.scl_slope),1);
    ByteSwapping::swapBytes(&(header.scl_inter),1);
    ByteSwapping::swapBytes(&(header.cal_max),1);
    ByteSwapping::swapBytes(&(header.cal_min),1);
    ByteSwapping::swapBytes(&(header.slice_duration),1);
    ByteSwapping::swapBytes(&(header.toffset),1);
    ByteSwapping::swapBytes(&(header.slice_start),1);
    ByteSwapping::swapBytes(&(header.slice_end),1);
    ByteSwapping::swapBytes(&(header.qform_code),1);
    ByteSwapping::swapBytes(&(header.sform_code),1);
    ByteSwapping::swapBytes(&(header.quatern_b),1);
    ByteSwapping::swapBytes(&(header.quatern_c),1);
    ByteSwapping::swapBytes(&(header.quatern_d),1);
    ByteSwapping::swapBytes(&(header.qoffset_x),1);
    ByteSwapping::swapBytes(&(header.qoffset_y),1);
    ByteSwapping::swapBytes(&(header.qoffset_z),1);
    ByteSwapping::swapBytes(&(header.srow_x[0]),4);
    ByteSwapping::swapBytes(&(header.srow_y[0]),4);
    ByteSwapping::swapBytes(&(header.srow_z[0]),4);
    //ByteSwapping::swapBytes(&(header.slice_code),1);
    //ByteSwapping::swapBytes(&(header.xyzt_units),1);
    ByteSwapping::swapBytes(&(header.intent_code),1);
}

/**
 * writeFile
 *
 * writes the nifti header to the output file handle
 * @param file
 */
void NiftiHeaderIO::writeFile(gzFile file, NIFTI_BYTE_ORDER byte_order) throw (NiftiException)
{
    uint8_t bytes[548];

    if(this->niftiVersion == 1)
    {
        //swap for write if needed
        nifti_1_header header;
        nifti1Header.getHeaderStruct(header);
        fixDimensions(header);
        if (byte_order == SWAPPED_BYTE_ORDER) swapHeaderBytes(header);
        memcpy(bytes,(char *)&header,sizeof(header));

    }
    else if(this->niftiVersion == 2)
    {
        //swap for write if needed
        nifti_2_header header;
        nifti2Header.getHeaderStruct(header);
        fixDimensions(header);
        if (byte_order == SWAPPED_BYTE_ORDER) swapHeaderBytes(header);
        memcpy(bytes,(char *)&header,sizeof(header));
    }
    else throw NiftiException("NiftiHeaderIO only currently supports Nifti versions 1 and 2.");

    if(this->niftiVersion==1) gzwrite(file,bytes,sizeof(nifti_1_header));
    else if(this->niftiVersion==2) gzwrite(file,bytes,sizeof(nifti_2_header));
}

/**
 * writeFile
 *
 * writes the nifti header to the output file handle
 * @param outputFile
 */
void NiftiHeaderIO::writeFile(QFile &file, NIFTI_BYTE_ORDER byte_order) throw (NiftiException)
{
    uint8_t bytes[548];

    if(this->niftiVersion == 1)
    {
        //swap for write if needed
        nifti_1_header header;
        nifti1Header.getHeaderStruct(header);
        fixDimensions(header);
        if (byte_order == SWAPPED_BYTE_ORDER) swapHeaderBytes(header);
        memcpy(bytes,(char *)&header,sizeof(header));

    }
    else if(this->niftiVersion == 2)
    {
        //swap for write if needed
        nifti_2_header header;
        nifti2Header.getHeaderStruct(header);
        fixDimensions(header);
        if (byte_order == SWAPPED_BYTE_ORDER) swapHeaderBytes(header);
        memcpy(bytes,(char *)&header,sizeof(header));
    }
    else throw NiftiException("NiftiHeaderIO only currently supports Nifti versions 1 and 2.");
    
    if(!file.isOpen())
    {
        if(!file.open(QIODevice::WriteOnly))
        {
            throw NiftiException("There was an error opening the file for writing.");
        }
    }
    if(this->niftiVersion == 1) file.write((char *)bytes,sizeof(nifti_1_header));
    else if(this->niftiVersion == 2) file.write((char *)bytes,sizeof(nifti_2_header));
}

/**
  *
  * returns the niftiVersion of the Header, either 1 or 2.  In the event that a Nifti file
  * hasn't been read, 0 is returned.
  * @return niftiVersion
  */
int NiftiHeaderIO::getNiftiVersion()
{
    return niftiVersion;
}

/**
  * getHeader
  *
  * gets a nifti 1 Header, throws an exception if the version does not match.
  * @param header
  **/
void NiftiHeaderIO::getHeader(Nifti1Header &header) const throw (NiftiException)
{
    if(this->niftiVersion !=1) throw NiftiException("Nifti1Header requested, this version is not loaded.");

    header = this->nifti1Header;
}

/**
  * getHeader
  *
  * gets a nifti 2 Header, throws an exception if the version does not match.
  * @param header
  **/
void NiftiHeaderIO::getHeader(Nifti2Header &header) const throw (NiftiException)
{
    //if(this->niftiVersion !=2 && this->niftiVersion != 1) throw NiftiException("Nifti1Header requested, this version is not loaded.");
    //FIXME: TSC: if this volume was created, rather than loaded, it won't have a nifti header to begin with
    if(niftiVersion == 2) {
        header = this->nifti2Header;
    }
    else {
        header = this->nifti1Header;
    }
}

/**
  * setHeader
  *
  * sets a nifti 1 Header, throws an exception if the header is invalid
  * @param header
  **/
void NiftiHeaderIO::setHeader(const Nifti1Header &header) throw (NiftiException)
{
    niftiVersion = 1;
    this->nifti1Header = header;
}

/**
  * setHeader
  *
  * sets a nifti 2 Header, throws an exception if the header is invalid
  * @param header
  **/

void NiftiHeaderIO::setHeader(const Nifti2Header &header) throw (NiftiException)
{
    niftiVersion = 2;
    this->nifti2Header = header;
}

int64_t NiftiHeaderIO::getVolumeOffset()
{
    if(niftiVersion == 1)
        return this->nifti1Header.getVolumeOffset();
    else if(niftiVersion == 2)
        return this->nifti2Header.getVolumeOffset();
    else return 0;
}

void NiftiHeaderIO::setVolumeOffset(const int64_t &offsetIn)
{
    if(niftiVersion == 1)
        this->nifti1Header.setVolumeOffset(offsetIn);
    else if(niftiVersion == 2)
        this->nifti2Header.setVolumeOffset(offsetIn);
}

int64_t NiftiHeaderIO::getExtensionsOffset()
{
    if(niftiVersion == 1)
        return 348;
    else if(niftiVersion == 2)
        return 540;
    else return 0;
}

vector<vector<float> > NiftiHeaderIO::getFSLSpace() const
{
    float pixDim[3];//don't look at me, blame analyze and flirt
    vector<int64_t> dimensions;
    vector<vector<float> > outScale;
    vector<vector<float> > sform;
    switch (niftiVersion)
    {
        case 1:
        {
            nifti1Header.getSForm(sform);
            nifti1Header.getDimensions(dimensions);
            nifti_1_header mystruct1;
            nifti1Header.getHeaderStruct(mystruct1);
            pixDim[0] = mystruct1.pixdim[1];//yes, that is really what they use, despite checking the SFORM/QFORM for flipping - ask them, not me
            pixDim[1] = mystruct1.pixdim[2];
            pixDim[2] = mystruct1.pixdim[3];
            break;
        }
        case 2:
        {
            nifti2Header.getSForm(sform);
            nifti2Header.getDimensions(dimensions);
            nifti_2_header mystruct2;
            nifti2Header.getHeaderStruct(mystruct2);
            pixDim[0] = mystruct2.pixdim[1];
            pixDim[1] = mystruct2.pixdim[2];
            pixDim[2] = mystruct2.pixdim[3];
            break;
        }
        default:
            throw NiftiException("getFSLSpace called on empty NiftiHeaderIO");
    }
    if (dimensions.size() < 3) throw NiftiException("NiftiHeaderIO has less than 3 dimensions, can't generate the FSL space for it");
    float determinant = sform[0][0] * sform[1][1] * sform[2][2] +
                        sform[0][1] * sform[1][2] * sform[2][0] +
                        sform[0][2] * sform[1][0] * sform[2][1] -
                        sform[0][2] * sform[1][1] * sform[2][0] -
                        sform[0][0] * sform[1][2] * sform[2][1] -
                        sform[0][1] * sform[1][0] * sform[2][2];//just write out the 3x3 determinant rather than packing it into a FloatMatrix first - and I haven't put a determinant function in yet
    outScale = FloatMatrix::identity(4).getMatrix();//generate a 4x4 with 0 0 0 1 last row via FloatMatrix for convenience
    if (determinant > 0.0f)
    {
        outScale[0][0] = -pixDim[0];
        outScale[0][3] = (dimensions[0] - 1) * pixDim[0];
    } else {
        outScale[0][0] = pixDim[0];
    }
    outScale[1][1] = pixDim[1];
    outScale[2][2] = pixDim[2];
    return outScale;
}

vector<vector<float> > NiftiHeaderIO::getSForm() const
{
    vector<vector<float> > outSform;
    switch (niftiVersion)
    {
        case 1:
            nifti1Header.getSForm(outSform);
            break;
        case 2:
            nifti2Header.getSForm(outSform);
            break;
        default:
            throw NiftiException("getSform called on empty NiftiHeaderIO");
    }
    return outSform;
}
