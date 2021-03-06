#ifndef __CHART_SELECTION_VIEW_CONTROLLER_H__
#define __CHART_SELECTION_VIEW_CONTROLLER_H__

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

#include <QWidget>

#include "EventListenerInterface.h"

class QButtonGroup;
class QCheckBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSignalMapper;

namespace caret {

    class ChartableBrainordinateInterface;
    class ChartableMatrixInterface;
    class ChartModel;
    
    class ChartSelectionViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ChartSelectionViewController(const Qt::Orientation orientation,
                                     const int32_t browserWindowIndex,
                                     QWidget* parent);
        
        virtual ~ChartSelectionViewController();
        
    private slots:
        void selectionCheckBoxClicked(int);
        
        void selectionRadioButtonClicked(int);
        
    private:
        ChartSelectionViewController(const ChartSelectionViewController&);

        ChartSelectionViewController& operator=(const ChartSelectionViewController&);
        
    public:

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private:
        enum Mode {
            MODE_INVALID,
            MODE_BRAINORDINATE,
            MODE_MATRIX
        };
        
        // ADD_NEW_MEMBERS_HERE

        void updateSelectionViewController();
        
        ChartableBrainordinateInterface* getBrainordinateFileAtIndex(const int32_t indx);
        
        ChartableMatrixInterface* getMatrixFileAtIndex(const int32_t indx);
        
//        ChartModel* getSelectedChartModel();
        
        Mode m_mode;
        
        const int32_t m_browserWindowIndex;
        
        std::vector<QCheckBox*> m_fileEnableCheckBoxes;
        
        std::vector<QLineEdit*> m_fileNameLineEdits;
        
        std::vector<QRadioButton*> m_fileSelectionRadioButtons;
        
        QButtonGroup* m_selectionRadioButtonGroup;
        
        QGridLayout* m_gridLayout;
        
        QSignalMapper* m_signalMapperFileEnableCheckBox;
        
        QLabel* m_averageLabel;
        
        static const int COLUMN_CHECKBOX;
        static const int COLUMN_RADIOBUTTON;
        static const int COLUMN_LINE_EDIT;
        
    };
    
#ifdef __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__
    const int ChartSelectionViewController::COLUMN_CHECKBOX    = 0;
    const int ChartSelectionViewController::COLUMN_RADIOBUTTON = 1;
    const int ChartSelectionViewController::COLUMN_LINE_EDIT   = 2;
#endif // __CHART_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_SELECTION_VIEW_CONTROLLER_H__
