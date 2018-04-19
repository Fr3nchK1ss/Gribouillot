/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QDebug>
#include <QTabBar>

#include "gribouillottabwidget.h"

GribouillotTabWidget::GribouillotTabWidget(QWidget* parent) :
    SmartInsertTabWidget(parent)
{
    setTabText(0, tr("Background map"));
    tabBar()->setSelectionBehaviorOnRemove(QTabBar::SelectLeftTab);
    //setMovable(true);

    //Create a tab with a "+" icon at the end of the tabBar.
    tabBar()->addTab(QIcon(":/Resources/Icons/add-layer.png"), QString());
    int plusTabIndex = count() - 1;
    setTabEnabled(plusTabIndex, false);
    setTabToolTip(plusTabIndex, tr("Create new layer"));

}


/**
 * @brief   Insert a tab with a visibility button
 *
 */
int GribouillotTabWidget::insertAndDisplayTab(int index, GribouillotLayer *layer, const QString &label)
{
    int insertIndex = insertTab(index, layer, label);
    tabBar()->setTabButton(index, QTabBar::RightSide, layer->getVisibilityBtt());

    return insertIndex;

}








