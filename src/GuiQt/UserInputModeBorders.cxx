
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

#include <memory>

#define __USER_INPUT_MODE_BORDERS_DECLARE__
#include "UserInputModeBorders.h"
#undef __USER_INPUT_MODE_BORDERS_DECLARE__

#include "Border.h"
#include "BorderFile.h"
#include "BorderPropertiesEditorDialog.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainOpenGLWidget.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretLogger.h"
#include "CursorManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "SelectionItemBorderSurface.h"
#include "SelectionManager.h"
#include "Model.h"
#include "ModelSurface.h"
#include "MouseEvent.h"
#include "Surface.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "UserInputModeBordersWidget.h"
#include "UserInputModeView.h"
#include "WuQMessageBox.h"

using namespace caret;
    
/**
 * \class caret::UserInputModeBorders 
 * \brief Processing user input for Borders mode.
 *
 * Processes user input in Border mode which includes
 * drawing and editing borders.
 */

/**
 * Constructor.
 *
 * @param borderBeingDrawnByOpenGL
 *    Border that is displayed in OpenGL area when a border is being drawn
 * @param windowIndex
 *    Index of the browser window using this border processing.
 */
UserInputModeBorders::UserInputModeBorders(Border* borderBeingDrawnByOpenGL,
                                           const int32_t windowIndex)
: UserInputModeView()
{
    this->borderBeingDrawnByOpenGL = borderBeingDrawnByOpenGL;
    this->windowIndex = windowIndex;
    this->mode = MODE_DRAW;
    this->drawOperation = DRAW_OPERATION_CREATE;
    this->editOperation = EDIT_OPERATION_PROPERTIES;
    this->borderToolsWidget = new UserInputModeBordersWidget(this);
}

/**
 * Destructor.
 */
UserInputModeBorders::~UserInputModeBorders()
{
    
}

/**
 * @return The input mode enumerated type.
 */
UserInputModeBorders::UserInputMode 
UserInputModeBorders::getUserInputMode() const
{
    return UserInputReceiverInterface::BORDERS;
}

/**
 * Draw a border point at the mouse coordinates.
 */
void 
UserInputModeBorders::drawPointAtMouseXY(BrainOpenGLWidget* openGLWidget,
                                         const int32_t mouseX,
                                         const int32_t mouseY)
{
    SurfaceProjectedItem projectedItem;
    openGLWidget->performProjection(mouseX,
                                    mouseY,
                                    projectedItem);
    
    SurfaceProjectedItem* spi = new SurfaceProjectedItem();
    
    AString txt;
//    if (projectedItem.isStereotaxicXYZValid()) {
//        spi->setStereotaxicXYZ(projectedItem.getStereotaxicXYZ());
//        
//        txt += ("Projected Position: " 
//                + AString::fromNumbers(projectedItem.getStereotaxicXYZ(), 3, ","));
//    }
    
    if (projectedItem.getBarycentricProjection()->isValid()) {
        SurfaceProjectionBarycentric* bp = projectedItem.getBarycentricProjection();
        
        txt += ("\nBarycentric Position: " 
                + AString::fromNumbers(bp->getTriangleAreas(), 3, ",")
                + "   "
                + AString::fromNumbers(bp->getTriangleNodes(), 3, ","));
        
        SurfaceProjectionBarycentric* spb = spi->getBarycentricProjection();
        spb->setProjectionSurfaceNumberOfNodes(bp->getProjectionSurfaceNumberOfNodes());
        spb->setTriangleAreas(bp->getTriangleAreas());
        spb->setTriangleNodes(bp->getTriangleNodes());
        spb->setSignedDistanceAboveSurface(0.0);
        spb->setValid(true);
    }

    if (spi->isStereotaxicXYZValid()
        || spi->getBarycentricProjection()->isValid()) {
        spi->setStructure(projectedItem.getStructure());
        this->borderBeingDrawnByOpenGL->addPoint(spi);
    }
    else {
        delete spi;
    }
    
    CaretLogFiner(txt);
}

/**
 * Called when 'this' user input receiver is set
 * to receive events.
 */
void 
UserInputModeBorders::initialize()
{
    this->borderToolsWidget->updateWidget();
}

/**
 * Called when 'this' user input receiver is no
 * longer set to receive events.
 */
void 
UserInputModeBorders::finish()
{
}

/**
 * Update after borders changed.
 */
void 
UserInputModeBorders::updateAfterBordersChanged()
{
    /*
     * Need to update all graphics windows and all border controllers.
     */
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().addBorder().getPointer());
}

/**
 * @return A widget for display at the bottom of the
 * Browser Window Toolbar when this mode is active.
 * If no user-interface controls are needed, return NULL.
 * The toolbar will take ownership of the widget and
 * delete it so derived class MUST NOT delete the widget.
 */
QWidget* 
UserInputModeBorders::getWidgetForToolBar()
{
    return this->borderToolsWidget;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
UserInputModeBorders::toString() const
{
    return "UserInputModeBorders";
}

/**
 * @return the mode.
 */
UserInputModeBorders::Mode 
UserInputModeBorders::getMode() const
{
    return this->mode;
}

/**
 * Set the mode.
 * @param mode
 *    New value for mode.
 */
void 
UserInputModeBorders::setMode(const Mode mode)
{
    if (this->mode != mode) {
        this->mode = mode;
        this->borderBeingDrawnByOpenGL->clear();
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
    }
    this->borderToolsWidget->updateWidget();
}

/**
 * @return the create operation.
 */
UserInputModeBorders::DrawOperation 
UserInputModeBorders::getDrawOperation() const
{
    return this->drawOperation;
}

/**
 * Set the create operation.
 * @param createOperation
 *    New value for create operation.
 */
void 
UserInputModeBorders::setDrawOperation(const DrawOperation drawOperation)
{
    this->drawOperation = drawOperation;
    this->borderToolsWidget->updateWidget();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * @return True if the border endpoints should be highlighted, else false.
 */
bool
UserInputModeBorders::isHighlightBorderEndPoints() const
{
    if (getMode() == MODE_DRAW) {
        if (getDrawOperation() != DRAW_OPERATION_CREATE) {
            return true;
        }
    }
    return false;
}

/**
 * Finish the border that the user was drawing.
 */
void
UserInputModeBorders::drawOperationFinish()
{
    this->borderBeingDrawnByOpenGL->clear();

    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().addBorder().getPointer());
}

/**
 * Undo (remove last point) from border being drawn.
 */
void 
UserInputModeBorders::drawOperationUndo()
{
    this->borderBeingDrawnByOpenGL->removeLastPoint();
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
}

/**
 * Reset the border being drawn.
 */
void 
UserInputModeBorders::drawOperationReset()
{
    this->borderBeingDrawnByOpenGL->clear();
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
}

/**
 * @return The edit operation.
 */
UserInputModeBorders::EditOperation 
UserInputModeBorders::getEditOperation() const
{
    return this->editOperation;
}

/**
 * Set the edit operation.
 * @param editOperation
 *   New edit operation.
 */
void 
UserInputModeBorders::setEditOperation(const EditOperation editOperation)
{
    this->editOperation = editOperation;
}

/**
 * @return The cursor for display in the OpenGL widget.
 */
CursorEnum::Enum
UserInputModeBorders::getCursor() const
{

    CursorEnum::Enum cursor = CursorEnum::CURSOR_DEFAULT;
    
    switch (this->mode) {
        case MODE_DRAW:
            cursor = CursorEnum::CURSOR_DRAWING_PEN;
            break;
        case MODE_EDIT:
            cursor = CursorEnum::CURSOR_POINTING_HAND;
            break;
        case MODE_ROI:
            cursor = CursorEnum::CURSOR_POINTING_HAND;
            break;
    }
            
    return cursor;
}

/**
 * Process a mouse left click event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeBorders::mouseLeftClick(const MouseEvent& mouseEvent)
{
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    const int mouseX = mouseEvent.getX();
    const int mouseY = mouseEvent.getY();
    
    switch (this->mode) {
        case MODE_DRAW:
            this->drawPointAtMouseXY(openGLWidget,
                                     mouseX,
                                     mouseY);
            EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
            break;
        case MODE_EDIT:
            switch (this->editOperation) {
                case EDIT_OPERATION_DELETE:
                {
                        SelectionManager* idManager =
                        openGLWidget->performIdentification(mouseX,
                                                            mouseY,
                                                            true);
                        SelectionItemBorderSurface* idBorder = idManager->getSurfaceBorderIdentification();
                        if (idBorder->isValid()) {
                            BorderFile* borderFile = idBorder->getBorderFile();
                            Border* border = idBorder->getBorder();
                            borderFile->removeBorder(border);
                            this->updateAfterBordersChanged();
                        }
                    }
                    break;
                case EDIT_OPERATION_PROPERTIES:
                {
                        SelectionManager* idManager =
                        openGLWidget->performIdentification(mouseX,
                                                            mouseY,
                                                            true);
                        SelectionItemBorderSurface* idBorder = idManager->getSurfaceBorderIdentification();
                        if (idBorder->isValid()) {
                            BorderFile* borderFile = idBorder->getBorderFile();
                            Border* border = idBorder->getBorder();
                            std::auto_ptr<BorderPropertiesEditorDialog> editBorderDialog(
                                                                                         BorderPropertiesEditorDialog::newInstanceEditBorder(borderFile,
                                                                                                                                             border,
                                                                                                                                             openGLWidget));
                            if (editBorderDialog->exec() == BorderPropertiesEditorDialog::Accepted) {
                                this->updateAfterBordersChanged();
                            }
                        }
                    }
                    break;
            }
            break;
        case MODE_ROI:
        {
            SelectionManager* idManager =
            openGLWidget->performIdentification(mouseX,
                                                mouseY,
                                                true);
            SelectionItemBorderSurface* idBorder = idManager->getSurfaceBorderIdentification();
            if (idBorder->isValid()) {
                Brain* brain = idBorder->getBrain();
                Surface* surface = idBorder->getSurface();
                //BorderFile* borderFile = idBorder->getBorderFile();
                Border* border = idBorder->getBorder();
                this->borderToolsWidget->executeRoiInsideSelectedBorderOperation(brain,
                                                                                 surface,
                                                                                 border);
            }
        }
            break;
    }
}

/**
 * Process a mouse left click with Shift key event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeBorders::mouseLeftClickWithShift(const MouseEvent& /*mouseEvent*/)
{
    switch (this->mode) {
        case MODE_DRAW:
            this->borderToolsWidget->executeFinishOperation();
            break;
        case MODE_EDIT:
            break;
        case MODE_ROI:
            break;
    }
}

/**
 * Process a mouse left drag with Alt key event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeBorders::mouseLeftDragWithAlt(const MouseEvent& mouseEvent)
{
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    const int mouseX = mouseEvent.getX();
    const int mouseY = mouseEvent.getY();
    
    switch (this->mode) {
        case MODE_DRAW:
            this->drawPointAtMouseXY(openGLWidget,
                                     mouseX,
                                     mouseY);
            EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
            break;
        case MODE_EDIT:
            break;
        case MODE_ROI:
            break;
    }    
}



