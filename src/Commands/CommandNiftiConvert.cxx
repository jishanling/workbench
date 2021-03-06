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

#include <fstream>
#include <ostream>
#include <iostream>

#include "CommandNiftiConvert.h"
#include "FileInformation.h"
#include "NiftiFile.h"
#include "ProgramParameters.h"

using namespace caret;

/**
 * Constructor.
 */
CommandNiftiConvert::CommandNiftiConvert()
: CommandOperation("-nifti-convert",
                         "Convert a NIFTI 1 file to a NIFTI 2 file")
{
    
}

/**
 * Destructor.
 */
CommandNiftiConvert::~CommandNiftiConvert()
{
    
}

/**
 * @return The help information.
 */
AString
CommandNiftiConvert::getHelpInformation(const AString& /*programName*/)
{
    AString helpInfo = ("Convert NIFTI-1 to NIFTI-2\n"
                        "\n"
                        "Usage:  <input-nifti-file>\n"
                        "        <output-nifti-file>\n"
                        "    \n"
                        "    input-nifti-file\n"
                        "        Required input NIFTI file name.\n"
                        "    \n"
                        "    output-nifti-file\n"
                        "        Required output NIFTI file name.\n"
                        "    \n"
                        );
    return helpInfo;
}

/**
 * Execute the operation.
 * 
 * @param parameters
 *   Parameters for the operation.
 * @throws CommandException
 *   If the command failed.
 * @throws ProgramParametersException
 *   If there is an error in the parameters.
 */
void 
CommandNiftiConvert::executeOperation(ProgramParameters& parameters) throw (CommandException,
                                                               ProgramParametersException)
{
    try
    {
        const AString inputFileName = parameters.nextString("Input NIFTI File Name");
        const AString outputFileName = parameters.nextString("Output NIFTI File Name");
        NiftiFile nf(inputFileName);
        //NiftiFile nfOut;
        Nifti2Header header;
        nf.getHeader(header);
        nf.setHeader(header);
        nf.writeFile(outputFileName);
    } catch (ProgramParametersException& e) {
        throw e;
    } catch (CaretException& e) {
        throw CommandException(e);//rethrow all other caret exceptions as CommandException
    } catch (std::exception& e) {
        throw CommandException(e.what());//rethrow std::exception and derived as CommandException
    } catch (...) {
        throw CommandException("unknown exception type thrown");//throw dummy CommandException for anything else
    }
}
