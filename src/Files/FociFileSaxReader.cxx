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
/*LICENSE_END*/

#include <sstream>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GiftiXmlElements.h"
#include "FociFile.h"
#include "FociFileSaxReader.h"
#include "Focus.h"
#include "GiftiLabelTableSaxReader.h"
#include "GiftiMetaDataSaxReader.h"
#include "StudyMetaDataLinkSet.h"
#include "StudyMetaDataLinkSetSaxReader.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectedItemSaxReader.h"

#include "XmlAttributes.h"
#include "XmlException.h"
#include "XmlUtilities.h"

using namespace caret;

/**
 * \class caret::FociFileSaxReader
 * \brief Reads a foci file using a SAX XML Parser.
 */

/**
 * constructor.
 */
FociFileSaxReader::FociFileSaxReader(FociFile* fociFile)
{
   CaretAssert(fociFile);
   m_fociFile = fociFile;
   m_state = STATE_NONE;
   m_stateStack.push(m_state);
   m_elementText = "";
   m_metaDataSaxReader = NULL;
    m_classTableSaxReader = NULL;
    m_surfaceProjectedItemSaxReader = NULL;
    m_focus = NULL;
    m_surfaceProjectedItem = NULL;
    m_studyMetaDataLinkSetSaxReader = NULL;
}

/**
 * destructor.
 */
FociFileSaxReader::~FociFileSaxReader()
{
    /*
     * If reading fails, allocated items need to be deleted.
     */
    if (m_metaDataSaxReader != NULL) {
        delete m_metaDataSaxReader;
    }
    if (m_classTableSaxReader != NULL) {
        delete m_classTableSaxReader;
    }
    if (m_surfaceProjectedItemSaxReader != NULL) {
        delete m_surfaceProjectedItemSaxReader;
    }
    if (m_surfaceProjectedItem != NULL) {
        delete m_surfaceProjectedItem;
    }
    if (m_focus != NULL) {
        delete m_focus;
    }
    if (m_studyMetaDataLinkSetSaxReader != NULL) {
        delete m_studyMetaDataLinkSetSaxReader;
    }
}


/**
 * start an element.
 */
void 
FociFileSaxReader::startElement(const AString& namespaceURI,
                                         const AString& localName,
                                         const AString& qName,
                                         const XmlAttributes& attributes)  throw (XmlSaxParserException)
{
   const STATE previousState = m_state;
   switch (m_state) {
      case STATE_NONE:
           if (qName == FociFile::XML_TAG_FOCI_FILE) {
            m_state = STATE_FOCI_FILE;
            
            //
            // Check version of file being read
            //
             const float version = attributes.getValueAsFloat(FociFile::XML_ATTRIBUTE_VERSION);
            if (version > FociFile::getFileVersion()) {
                AString msg = XmlUtilities::createInvalidVersionMessage(FociFile::getFileVersion(), 
                                                                        version);
                XmlSaxParserException e(msg);
                CaretLogThrowing(e);
                throw e;
            }
         }
         else {
             const AString msg = XmlUtilities::createInvalidRootElementMessage(FociFile::XML_TAG_FOCI_FILE,
                                                                               qName);
             XmlSaxParserException e(msg);
             CaretLogThrowing(e);
             throw e;
         }
         break;
      case STATE_FOCUS:
           if (qName == SurfaceProjectedItem::XML_TAG_SURFACE_PROJECTED_ITEM) {
               m_state = STATE_SURFACE_PROJECTED_ITEM;
               m_surfaceProjectedItem = new SurfaceProjectedItem();
               m_surfaceProjectedItemSaxReader = new SurfaceProjectedItemSaxReader(m_surfaceProjectedItem);
               m_surfaceProjectedItemSaxReader->startElement(namespaceURI, localName, qName, attributes);
           }
           else if (qName == StudyMetaDataLinkSet::XML_TAG_STUDY_META_DATA_LINK_SET) {
               m_state = STATE_STUDY_META_DATA_LINK_SET;
               m_studyMetaDataLinkSetSaxReader = new StudyMetaDataLinkSetSaxReader(m_focus->getStudyMetaDataLinkSet());
               m_studyMetaDataLinkSetSaxReader->startElement(namespaceURI, localName, qName, attributes);
           }
           break;
      case STATE_FOCI_FILE:
           if (qName == GiftiXmlElements::TAG_LABEL_TABLE) {
               m_state = STATE_CLASSES;
               GiftiLabelTable* classTable = m_fociFile->getColorTable();
               m_classTableSaxReader = new GiftiLabelTableSaxReader(classTable);
               m_classTableSaxReader->startElement(namespaceURI, localName, qName, attributes);
           }
         else if (qName == GiftiXmlElements::TAG_METADATA) {
             m_state = STATE_METADATA;
             m_metaDataSaxReader = new GiftiMetaDataSaxReader(m_fociFile->getFileMetaData());
             m_metaDataSaxReader->startElement(namespaceURI, localName, qName, attributes);
         }
         else if (qName == Focus::XML_TAG_FOCUS) {
             m_state = STATE_FOCUS;
             m_focus = new Focus();
         }
         else {
             const AString msg = XmlUtilities::createInvalidChildElementMessage(FociFile::XML_TAG_FOCI_FILE, 
                                                                                qName);
             XmlSaxParserException e(msg);
             CaretLogThrowing(e);
             throw e;
         }
         break;
      case STATE_METADATA:
           m_metaDataSaxReader->startElement(namespaceURI, localName, qName, attributes);
         break;
       case STATE_CLASSES:
           m_classTableSaxReader->startElement(namespaceURI, localName, qName, attributes);
           break;
      case STATE_SURFACE_PROJECTED_ITEM:
           m_surfaceProjectedItemSaxReader->startElement(namespaceURI, localName, qName, attributes);
           break;
       case STATE_STUDY_META_DATA_LINK_SET:
           m_studyMetaDataLinkSetSaxReader->startElement(namespaceURI, localName, qName, attributes);
           break;
   }
   
   //
   // Save previous state
   //
   m_stateStack.push(previousState);
   
   m_elementText = "";
}

/**
 * end an element.
 */
void 
FociFileSaxReader::endElement(const AString& namespaceURI,
                                       const AString& localName,
                                       const AString& qName) throw (XmlSaxParserException)
{
   switch (m_state) {
      case STATE_NONE:
         break;
       case STATE_FOCUS:
           CaretAssert(m_focus);
           if (qName == Focus::XML_TAG_FOCUS) {
               m_fociFile->addFocus(m_focus);
               m_focus = NULL;  // do not delete since added to foci file
           }
           else {
               m_focus->setElementFromText(qName, m_elementText.trimmed());
           }
           break;
      case STATE_FOCI_FILE:
         break;
      case STATE_METADATA:
           CaretAssert(m_metaDataSaxReader);
           m_metaDataSaxReader->endElement(namespaceURI, localName, qName);
           if (qName == GiftiXmlElements::TAG_METADATA) {
               delete m_metaDataSaxReader;
               m_metaDataSaxReader = NULL;
           }
         break;
       case STATE_CLASSES:
           CaretAssert(m_classTableSaxReader);
           m_classTableSaxReader->endElement(namespaceURI, localName, qName);
           if (qName == GiftiXmlElements::TAG_LABEL_TABLE) {
               delete m_classTableSaxReader;
               m_classTableSaxReader = NULL;
           }
           break;
      case STATE_SURFACE_PROJECTED_ITEM:
           CaretAssert(m_surfaceProjectedItemSaxReader);
           m_surfaceProjectedItemSaxReader->endElement(namespaceURI, localName, qName);
           if (qName == SurfaceProjectedItem::XML_TAG_SURFACE_PROJECTED_ITEM) {
               m_focus->addProjection(m_surfaceProjectedItem);
               m_surfaceProjectedItem = NULL; // do not delete since added to focus
               delete m_surfaceProjectedItemSaxReader;
               m_surfaceProjectedItemSaxReader = NULL;
           }
         break;
       case STATE_STUDY_META_DATA_LINK_SET:
           CaretAssert(m_studyMetaDataLinkSetSaxReader);
           m_studyMetaDataLinkSetSaxReader->endElement(namespaceURI, localName, qName);
           if (qName == StudyMetaDataLinkSet::XML_TAG_STUDY_META_DATA_LINK_SET) {
               delete m_studyMetaDataLinkSetSaxReader;
               m_studyMetaDataLinkSetSaxReader = NULL;
           }
   }

   //
   // Clear out for new elements
   //
   m_elementText = "";
   
   //
   // Go to previous state
   //
   if (m_stateStack.empty()) {
       throw XmlSaxParserException("State stack is empty while reading XML NiftDataFile.");
   }
   m_state = m_stateStack.top();
   m_stateStack.pop();
}

/**
 * get characters in an element.
 */
void 
FociFileSaxReader::characters(const char* ch) throw (XmlSaxParserException)
{
    if (m_metaDataSaxReader != NULL) {
        m_metaDataSaxReader->characters(ch);
    }
    else if (m_classTableSaxReader != NULL) {
        m_classTableSaxReader->characters(ch);
    }
    else if (m_surfaceProjectedItemSaxReader != NULL) {
        m_surfaceProjectedItemSaxReader->characters(ch);
    }
    else {
        m_elementText += ch;
    }
}

/**
 * a fatal error occurs.
 */
void 
FociFileSaxReader::fatalError(const XmlSaxParserException& e) throw (XmlSaxParserException)
{
/*
   std::ostringstream str;
   str << "Fatal Error at line number: " << e.getLineNumber() << "\n"
       << "Column number: " << e.getColumnNumber() << "\n"
       << "Message: " << e.whatString();
   if (errorMessage.isEmpty() == false) {
      str << "\n"
          << errorMessage;
   }
   errorMessage = str.str();
*/   
   //
   // Stop parsing
   //
   throw e;
}

// a warning occurs
void 
FociFileSaxReader::warning(const XmlSaxParserException& e) throw (XmlSaxParserException)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
FociFileSaxReader::error(const XmlSaxParserException& e) throw (XmlSaxParserException)
{   
    CaretLogSevere("XML Parser Error: " + e.whatString());
    throw e;
}

void 
FociFileSaxReader::startDocument()  throw (XmlSaxParserException)
{    
}

void 
FociFileSaxReader::endDocument() throw (XmlSaxParserException)
{
}
