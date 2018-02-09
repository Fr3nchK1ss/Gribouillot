/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QDebug>
#include <QtMath>

#include "item_pointonrail.h"


Item_pointOnRail::Item_pointOnRail(qreal penWidth, QLineF line) :
    QGraphicsEllipseItem()
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations);//cosmetic
    setPen(QPen(Qt::black));
    //filling color
    setBrush(QBrush(Qt::black));
    //The point should stand out of its rail line.
    penWidth += 3;

    setRect(-penWidth/2.0, -penWidth/2.0, penWidth, penWidth);

    rail = line;
    setPos(rail.center());

}


QLineF Item_pointOnRail::getRail()
{
    return rail;
}


/**
 * @brief   The point must move along its reference 'rail' line following
 *          user input.
 */
void Item_pointOnRail::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    //Create a virtual line representing user input
    QLineF userLine(rail.p1(), event->scenePos());

    /*
     * The motion along the 'rail' corresponds to a projection of
     * the mouse cursor motion. It's basically a dot product.
     */
    qreal scalarAngle = qDegreesToRadians(rail.angleTo(userLine));
    qreal projectedLength = userLine.length()*qCos(scalarAngle);

    //Project to rail, staying within rail boundaries
    QLineF projection(rail);
    if (projectedLength < 0)
        projectedLength = 0;
    else if (projectedLength > rail.length())
        projectedLength = rail.length();

    projection.setLength(projectedLength);

    //Set the position of the point to the newly computed position
    setPos(projection.p2());
    update();

    event->accept();

}
