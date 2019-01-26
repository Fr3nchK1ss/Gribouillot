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
    tabBar()->setSelectionBehaviorOnRemove(QTabBar::SelectLeftTab);
    //setMovable(true);

    //Create a tab with a "+" icon at the end of the tabBar.
    tabBar()->addTab(QIcon(":/Resources/Icons/add-layer.png"), QString());
    int plusTabIndex = count() - 1;
    setTabEnabled(plusTabIndex, false);
    setTabToolTip(plusTabIndex, tr("Create new layer"));

}


/**
 * @brief Delete all layers and set default texts
 * @
 *
 */
void GribouillotTabWidget::reset()
{
    /*
     * NB: This will call restrictToolbar() and select default cursor.
     * count()-2 and not just count() becoz mapTab and plusTab are kept!
     */
    QWidget* w;
    int layersCount = count()-2;//necessary because count() is modified in the loop.
    for (int i = 0; i < layersCount; ++i)
    {
        w = widget(1);//widget(1) is different each time!
        removeTab(1);//modify count()!
        delete w;
    }

    setTabText(0, tr("Background Map"));
    setTabToolTip(0, "");

}


/**
 * @brief Convenience function
 */
void GribouillotTabWidget::setMapTab(QString mapPath, QString mapName)
{
    setTabToolTip(0, mapPath);
    setTabText(0, mapName);

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








