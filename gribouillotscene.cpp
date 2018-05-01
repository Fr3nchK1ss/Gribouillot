/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QColorDialog>
#include <QDebug>
#include <QGraphicsView>
#include <QPen>

#include "gribouillotscene.h"

GribouillotScene::GribouillotScene(QObject *parent) :
    QGraphicsScene(parent)
{
    //Pens similar to Qt's selection pens
    QPen selectPen0(Qt::black);
    selectPen0.setDashPattern(QVector<qreal>({4,2}));
    selectPen0.setCosmetic(true);

    QPen selectPen1(Qt::white);
    selectPen1.setDashOffset(2);
    selectPen1.setDashPattern(QVector<qreal>({2,4}));
    selectPen1.setCosmetic(true);

    selectPens << selectPen0;
    selectPens << selectPen1;

}


/**
 * @brief   Enable items of types "onlyTypes".
 * @details Items become fully enabled.
 */
void GribouillotScene::enableOnlyItems(QList<GribouillotItem> types)
{
    foreach(QGraphicsItem* item, items() )
    {
        if(types.contains((GribouillotItem) item->type()) )
            item->setEnabled(true);
        else
            item->setEnabled(false);
    }

}



/**
 * @brief   Utility function to find out if a number of specific items are selected
 */
bool GribouillotScene::isOnlySelected(QVector<GribouillotItem> types, int targetCount)
{
    int sceneCount = 0;

    foreach(QGraphicsItem* item, selectedItems())
    {
        if ( types.contains((GribouillotItem)item->type()) )
            sceneCount++;
    }

    if (sceneCount == targetCount)
    {
        //we have the exact target number of items of allowed types
        return true;
    }

    return false;

}


/**
 * @brief Return the pens used for drawing selection boxes
 */
QVector<QPen> GribouillotScene::getSelectPens()
{
    return selectPens;
}



/********************* protected overloaded mouseEvents **********************/
/**
 * @brief   Send a signal (to the MainWindow) when the mouse button is pressed.
 */
void GribouillotScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF scenePos = event->scenePos();
    //qDebug() << "scenePos() " << scenePos;

    /*
     * A point is represented on scene by an ellipse with a radius of a few pixels.
     * So if the user clicks on any part of a point, we assume he meant to click on
     * the center of the point.
     */
    QGraphicsItem* item = itemAt(event->scenePos(), views().at(0)->transform());
    if ( item != nullptr && item->type() == POINT_W )
    {
        scenePos = item->scenePos();
        //qDebug() << "adjustClick2Point " << scenePos;
    }

    emit newMouseClickPreSelect(scenePos);

    /*
     * By default clicking on scene select the item below cursor. But in
     * SrollHandDrag mode (openHandCursor), this behaviour conflicts with
     * the GraphicsView behaviour which is "clicking to scroll around the
     * scene". So in this case, disable mousePressEvents.
     */
    if (views().at(0)->dragMode() == QGraphicsView::ScrollHandDrag)
        event->ignore();
    else
        /*
         * Default behavior (select item below cursor)
         * IMPORTANT: also forwards the event to mouse grabbing item.
         */
        QGraphicsScene::mousePressEvent(event);

    emit newMouseClickPostSelect(scenePos);

}


/**
 * @brief   Send a signal (to the MainWindow) when the mouse button is moved.
 */
void GribouillotScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit newMouseMove(event->scenePos());
    QGraphicsScene::mouseMoveEvent(event);
}


/**
 * @brief   React to keyboard input targeting scene items
 * @details Delete: delete one or many items
 *          Space:    select next item
 *          c:        change the color of selected items
 *          t:        change the thickness of selected items
 */
void GribouillotScene::keyPressEvent(QKeyEvent *e)
{
    switch ( e->key() )
    {
        case Qt::Key_Delete:
            //send a signal to remove items from the layer->itemslist
            emit keyDeletePressed();
            break;

        case Qt::Key_Space:
            //Tell the current layer to select its next item if any
            emit keySpacePressed();
            break;

        case Qt::Key_C:
            //change Color
            emit keyCPressed();
            break;

        case Qt::Key_T:
            //change thickness of the outlines of the selected items
            emit keyTPressed();
            break;

    }

    e->accept();
    QGraphicsScene::keyPressEvent(e);
}


