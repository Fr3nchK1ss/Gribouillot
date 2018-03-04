/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QDebug>
#include <QGraphicsScene>
#include <QtMath>
#include <QMessageBox>
#include <QPainter>

#include "item_spiraldrawer.h"

Item_spiralDrawer::Item_spiralDrawer(int penWidth, QColor c, QGraphicsItem *parent):
    QAbstractGraphicsShapeItem(parent)
{
    setFlag(QGraphicsItem::ItemIsFocusable);

    QPen pen;
    pen.setCosmetic(true);
    pen.setColor(c);
    pen.setWidth(penWidth);
    pen.setStyle(Qt::DashLine);
    setPen(pen);

    side.resize(4);
    //4 clicks for 4 centers
    remainingClicks = 4;

}


/**
 * @brief   Compute the base quadrangle out of user's clicks.
 *          Auto-compute missing centers if necessary.
 * @return  The status of the drawing if some data is missing,
 *          or a null String if we have enough data to compute
 *          a spiral.
 */
QString Item_spiralDrawer::computeSpiralBase()
{
    switch(remainingClicks)
    {
        case 2:
        {
            /*
             * Compute a square base.
             * At this point the end of side[1] is moving with
             * mouse cursor.
             */
            QLineF nV = side[0].normalVector();

            if (clockwise)
            {
                nV.setLength(-nV.length());
                side[3] = QLineF(nV.p2(), nV.p1());
                side[1] = QLineF(side[0].p2(), side[0].p1()).normalVector();
            }
            else
            {
                side[3] = QLineF(nV.p2(), nV.p1());
                side[1] = nV.translated(side[0].p2()-side[0].p1());
            }

            side[2] = QLineF(side[1].p2(), side[3].p1());
        }
            return QString();

        case 1:
            /*
             * Compute a trapezium base (may be a rectangle base).
             * At this point side[1] is entirely defined.
             */
            side[3] = QLineF(side[1].p2(), side[1].p1()).translated(side[0].p1()-side[0].p2());
            side[2] = QLineF(side[1].p2(), side[3].p1());
            return QString();

        case 0:
            /*
             * IMPORTANT: Check that the four centers given by user yield
             * a convex shape as the user's 4th click may be misplaced.
             */
            side[3] = QLineF(side[2].p2(), side[0].p1());

            if( ( clockwise && ( side[1].angleTo(side[2]) < 180 ||
                                 side[2].angleTo(side[3]) < 180 ||
                                 side[3].angleTo(side[0]) < 180) )
                ||
                (!clockwise && ( side[1].angleTo(side[2]) > 180 ||
                                 side[2].angleTo(side[3]) > 180 ||
                                 side[3].angleTo(side[0]) > 180) ))
            {
                return QString(tr("The given base is not convex, Gribouillot can not draw a spiral!"));
            }
            else
            {
                return QString();
            }

        default:
            return QString(tr("The spiral drawing needs at least 2 clicks!"));

    }
}




/**
 * @brief   Used by current layer to draw the spiral.
 * @return  The spiral as 4 independent arcs.
 */
QVector<Item_arc *> Item_spiralDrawer::getArcs()
{
    QVector<Item_arc *> arcs(4);
    qreal startAngle, spanAngle;

    for(int i = 0; i < 4; ++i)
    {
        Item_spiral::computeArcAngles(side[i], side[(i+1)%4],
                                      startAngle, spanAngle);
        arcs[i] = new Item_arc(side[i].p2(),
                               Item_spiral::computeArcRect(i, side),
                               startAngle,
                               spanAngle);
    }

    return arcs;
}


/**
 * @brief   Used by current layer to draw the spiral.
 * @return  The spiral as one single item.
 */
Item_spiral* Item_spiralDrawer::getSpiral()
{
    return new Item_spiral(getCenters());
}


/**
 * @brief   Used by current layer to draw the base centers.
 * @return  The 4 centers of the spiral.
 */
QVector<QPointF> Item_spiralDrawer::getCenters()
{
    return QVector<QPointF>({side[0].p1(),
                             side[1].p1(),
                             side[2].p1(),
                             side[2].p2()});
}


/**
 * @brief   Used by layer to draw the base quadrangle.
 * @return  The quadrangle base as 4 QLineF.
 */
QVector<QLineF> Item_spiralDrawer::getBase()
{
    return side;
}




QRectF Item_spiralDrawer::boundingRect() const
{
    qreal radius1, radius2, radius3;
    radius1 = radius2 = radius3 = 1;

    if(!side[0].isNull())
        radius1 = QLineF(scenePos(), side[0].p2()).length();
    if(!side[1].isNull())
        radius2 = QLineF(scenePos(), side[1].p2()).length();
    if(!side[2].isNull())
        radius3 = QLineF(scenePos(), side[2].p2()).length();


    qreal maxRadius = qMax(qMax(radius1,radius2), radius3);
    qreal bRectWidth = maxRadius*2 + pen().width() + 10;//10px as a margin

    return QRectF(QPointF(-bRectWidth/2, -bRectWidth/2), QSize(bRectWidth, bRectWidth));

}


void Item_spiralDrawer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(pen());

    //Draw helper lines to visualize the base quadrangle
    if(!side[0].isNull())
    {
        QPointF p1 = QPointF(0,0);//mapFromScene(side[0].p1());
        QPointF p2 = mapFromScene(side[0].p2());

        //Draw side[0]
        painter->drawLine(QLineF(p1,p2));

        /*
        painter->setPen(Qt::blue);
        painter->drawRect(Item_spiral::computeArcRect(0, side));
        painter->setPen(pen());
        */
    }

    if(!side[1].isNull())
    {
        QPointF p1 = mapFromScene(side[1].p1());
        QPointF p2 = mapFromScene(side[1].p2());

        //Draw side[1]
        painter->drawLine(QLineF(p1,p2));

        //Draw an arc hint so the user can guess the soon-to-be spiral.
        qreal arcHintAngle, spanAngle;
        Item_spiral::computeArcAngles(side[0], side[1], arcHintAngle, spanAngle);

        /*
         * Find on which side of side[0] we must draw the arc Hint, in other
         * words find if we must draw the spiral clockwise or counter-clockwise,
         */
        clockwise = ( side[0].angleTo(side[1]) > 180 );
        int clockwiseHint = clockwise ? -1 : 1;

        QRectF arcRect0 = Item_spiral::computeArcRect(0, side);
        arcRect0.translate(p1);//p1 == side[0].p2
        painter->drawArc( arcRect0, arcHintAngle*16, clockwiseHint*30*16);

        /*
        painter->setPen(Qt::blue);
        painter->drawRect(arcRect0);
        painter->setPen(pen());
        */

    }

    if(!side[2].isNull())
    {
        QPointF p1 = mapFromScene(side[2].p1());
        QPointF p2 = mapFromScene(side[2].p2());

        //Draw side[2]
        painter->drawLine(QLineF(p1,p2));
    }

    //painter->drawRect(boundingRect());

}


void Item_spiralDrawer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    remainingClicks--;

    if (remainingClicks == 3)
    {
        //The first clicks sets the first center
        setPos(event->scenePos());
    }
    else if(remainingClicks == 0)
    {
        emit endDrawing(computeSpiralBase());
    }

    event->accept();

}


void Item_spiralDrawer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    switch(remainingClicks)
    {
        case 3:
            side[0] = QLineF(scenePos(), event->scenePos());
            break;
        case 2:
            if (event->modifiers() & Qt::ShiftModifier)
            {   //Force side[1] perpendicular to side [0]
                QLineF hypotenus(side[0].p1(), event->scenePos());
                qreal angle = side[0].angleTo(hypotenus);
                qreal length = hypotenus.length()*qSin(qDegreesToRadians(angle));

                side[1] = side[0].normalVector().translated(side[0].p2()-side[0].p1());
                side[1].setLength(length);
            }
            else
            {
                side[1] = QLineF(side[0].p2(), event->scenePos());
            }

            break;
        case 1:
            if (event->modifiers() & Qt::ShiftModifier)
            {   //Force side[2] perpendicular to side [1]
                QLineF hypotenus(side[1].p1(), event->scenePos());
                qreal angle = side[1].angleTo(hypotenus);
                qreal length = hypotenus.length()*qSin(qDegreesToRadians(angle));

                side[2] = side[1].normalVector().translated(side[1].p2()-side[1].p1());
                side[2].setLength(length);
            }
            else
            {
                side[2] = QLineF(side[1].p2(), event->scenePos());
            }
            break;

        default:
            break;
    }
    event->accept();
    update();

}


void Item_spiralDrawer::keyPressEvent(QKeyEvent *event)
{
    switch ( event->key() )
    {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            emit endDrawing(computeSpiralBase());
            break;

        case Qt::Key_Escape:
            emit endDrawing(QString(tr("aborted")));
            break;

        default:
            QGraphicsItem::keyPressEvent(event);

    }
}

