/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QAction>
#include <QDebug>
#include <QTabBar>
#include <QPushButton>

#include "smartinserttabwidget.h"

SmartInsertTabWidget::SmartInsertTabWidget(QWidget *parent):
    QTabWidget(parent)
{
}


/**
 * @brief Automatically switch to newly inserted tab.
 */
void SmartInsertTabWidget::tabInserted(int index)
{
//    qDebug() << "tab inserted at index: " << index;

    //Set newly inserted tab as current tab: fine when the insertion happens at last index.
    setCurrentIndex(index);//emits a currentChanged() signal

    //Handle Qt bug concerning tab display when insertion does not happen at last index.
    if( index < (count() - 2) )
    {
        //The call to setCurrentIndex() written above won't behave as expected
//        qDebug() << "connect debugging code";
        bugTabIndex = index;//Save the index we want to display
        connect(this, &SmartInsertTabWidget::currentChanged, this, &SmartInsertTabWidget::handleTabBug);
    }
}


/**
 * @brief   Work around a Qt lib bug when inserting a tab anywhere but in last position:
 *          Qt forces the display of the tab at last index instead of displaying the new tab! May be
 *          a looping call to setCurrentIndex() between QTabWidget and QTabBar...
 * @details Fortunately we can catch the currentChanged() signal to provide a debugging hook. It seems
 *          this signal happens after tabInserted() is done, this is why I wrote the connect() there.
 */
void SmartInsertTabWidget::handleTabBug(int newTabIndex)
{
//        qDebug() << "buggy newTabIndex: " << newTabIndex;
//        qDebug() << "forcing to: " << bugTabIndex;

        //Disconnect debugging code
        disconnect(this, &SmartInsertTabWidget::currentChanged, this, &SmartInsertTabWidget::handleTabBug);
        //Force the display of the newly inserted tab
        setCurrentIndex(bugTabIndex);

}
