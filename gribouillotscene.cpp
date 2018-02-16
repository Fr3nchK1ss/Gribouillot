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

#include "gribouillotitem.h"
#include "gribouillotscene.h"

GribouillotScene::GribouillotScene(QObject *parent) :
    QGraphicsScene(parent)
{
}



/**
 * @brief   Disable the specifics for all items of the scene.
 * @details When working in a layer, only the items of this layer
 *          can use "specifics". Other items have their "specifics"
 *          disabled.
 */
void GribouillotScene::disableItemsSpecifics()
{
    foreach( QGraphicsItem* item, items())
    {
        if( item->type() == PIXMAP )
            item->setFlag(QGraphicsItem::ItemIsMovable, false);
        if ( item->type() == SPIRAL)
            item->setEnabled(false);
        //to complete if necessary
    }
}



/********************* protected overloaded mouseEvents **********************/
/**
 * @brief   Send a signal (to the MainWindow) when the mouse button is pressed.
 */
void GribouillotScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit newMouseClickPreSelect(event->scenePos());

    /*
     * By default clicking on scene select the item below cursor. But in
     * SrollHandDrag mode (openHandCursor), this behaviour conflicts with
     * the GraphicsView behaviour which is "clicking to scroll around the
     * scene". So in this case, disable mousePressEvents.
     */
    if (views().at(0)->dragMode() == QGraphicsView::ScrollHandDrag)
        event->ignore();
    else
        //Default behavior (select item below cursor)
        QGraphicsScene::mousePressEvent(event);

    emit newMouseClickPostSelect(event->scenePos());

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


