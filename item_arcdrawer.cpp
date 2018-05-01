/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QColor>
#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QRectF>
#include <QStyleOptionGraphicsItem>

#include "item_arcdrawer.h"
#include "qlinef58.h"


/**
 * @brief   This arcDrawer is a helper to draw an arc from a circle
 */
Item_arcDrawer::Item_arcDrawer(Item_circle* circle) :
    QAbstractGraphicsShapeItem()
{
    QPen pen;
    pen.setCosmetic(true);
    if (circle->pen().color().lightness() > 125)
            pen.setColor(Qt::black);
    else
            pen.setColor(Qt::yellow);
    pen.setWidth(circle->pen().width());
    pen.setStyle(Qt::DashDotLine);
    setPen(pen);

    sourceCircle = circle;//save source circle

    setPos(circle->scenePos());
    radius = circle->getRadius();
    startAngle = spanAngle = 0;

    //radius of bounding RectF (!= drawing RectF)
    bRadius = radius+.2*radius;

}


/**
 * @brief   This arcDrawer is a helper to draw an arc from a center and a point
 */
Item_arcDrawer::Item_arcDrawer(QColor penColor, int penWidth, QPointF center, QPointF radiusPoint) :
    QAbstractGraphicsShapeItem()
{
    QPen pen;
    pen.setCosmetic(true);
    pen.setColor(penColor);
    pen.setWidth(penWidth -1);
    pen.setStyle(Qt::DashLine);
    setPen(pen);

    setPos(center);
    firstVector = QLineF(center, radiusPoint);
    radius = firstVector.length();

    //Compute the startAngle of this arc Drawer
    QLineF horizontalVector(center, center+QPointF(1,0));
    startAngle = horizontalVector.angleTo(firstVector);

    //radius of bounding RectF (== drawing RectF)
    bRadius = radius;

}


/**
 * @brief   This arc drawer is a helper to draw a line from another line, given an
 *          angle.
 */
Item_arcDrawer::Item_arcDrawer(QColor penColor, int penWidth, int windowSize,
                               QPointF center, QLineF sourceLine) :
    QAbstractGraphicsShapeItem()
{
    QPen pen;
    pen.setCosmetic(true);
    pen.setColor(penColor);
    pen.setWidth(penWidth);
    pen.setStyle(Qt::DotLine);
    setPen(pen);

    paintArc = false;
    setPos(center);

    firstVector = sourceLine;
    //to avoid firstVector.P1 == firstVector.P2 =>
    firstVector.setLength(firstVector.length()+1);
    firstVector.setP1(center);
    firstVector.setLength(windowSize/8.0);
    radius = firstVector.length();
    startAngle = spanAngle = 0;

    //radius of bounding RectF (== drawing RectF)
    bRadius = radius+.2*radius;

}


/**
 * @brief   This arc drawer represents a protractor. Convenience constructor.
 */
Item_arcDrawer::Item_arcDrawer(int windowSize, QPointF center, QLineF sourceLine) :
    Item_arcDrawer::Item_arcDrawer(Qt::yellow, 5, windowSize, center, sourceLine)
{
    isProtractor = true;
}


/**
 * @brief   This arc drawer helps to rotate a pixmap
 */
Item_arcDrawer::Item_arcDrawer(QPointF center, QSizeF picSize)
{
    QPen pen;
    pen.setCosmetic(true);
    pen.setColor(Qt::gray);
    pen.setWidth(5);
    pen.setStyle(Qt::DashLine);
    setPen(pen);

    setPos(center);

    qreal max= qMax(picSize.width(),picSize.height());
    firstVector = QLineF(center, center+QPointF(0,1));
    firstVector.setLength(max + 0.1*max );
    radius = firstVector.length();

    //Compute the startAngle of this arc Drawer
    QLineF horizontalVector(center, center+QPointF(1,0));
    startAngle = horizontalVector.angleTo(firstVector);//90°

    //radius of bounding RectF (== drawing RectF)
    bRadius = radius;

}


/**
 * @return  the arc part of the arcDrawer as an Item_arc
 */
Item_arc* Item_arcDrawer::getArc()
{
    if (qAbs(spanAngle) < 0.5)
        return nullptr;//Arc too small == point
    //else
    return new Item_arc(scenePos(), radius, startAngle, spanAngle);

}


/**
 * @return  the arc part of the arcDrawer as an Item_arc
 */
QLineF Item_arcDrawer::getAngledLine()
{
    return secondVector;

}


/**
 * @return  the pointer to the circle from which the arc is extracted,
 *          if any.
 */
Item_circle* Item_arcDrawer::getSourceCircle()
{
    return sourceCircle;//can be a nullptr

}


/**
 * @brief   implement virtual function boundingRect()
 */
QRectF Item_arcDrawer::boundingRect() const
{
    QRectF boundingRect = QRectF(-bRadius, -bRadius, bRadius*2, bRadius*2);
    return boundingRect;

}


/**
 * @brief   reimplement mousePressEvent for self-drawing
 * @details function is unused except for cutting an arc from a circle
 */
void Item_arcDrawer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (firstVector.isNull())
    {
        QPointF center = scenePos();

        //This click defines the firstVector
        firstVector = QLineF(center, event->scenePos());

        //Compute the startAngle of this arc Drawer
        QLineF horizontalVector(center, center+QPointF(1,0));
        startAngle = horizontalVector.angleTo(firstVector);
    }

    event->accept();
}


/**
 * @brief   reimplement mouseMoveEvent for self-drawing
 * @brief   continuously drawing secondVector for user's feedback
 */
void Item_arcDrawer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (firstVector.isNull())
    {
        mouseMovePos = event->scenePos();//save for paint function
    }
    else
    {
        secondVector = QLineF(scenePos(), event->scenePos());
        qreal newAngle = firstVector.angleTo(secondVector);

        //Reverse drawing if necessary for a more intuitive arc drawing
        if( qAbs(spanAngle) < 60 && newAngle > 300)
            startClockwise = true;
        else if( qAbs(spanAngle) > 300 && newAngle < 60)
            startClockwise = false;

        /*
         * Qt default arc drawing is counter-clockwise. If the user wants to draw
         * clockwise, we need to reverse the spanAngle
         */
        spanAngle = newAngle;//default

        if(startClockwise)
            spanAngle = -(360.0-spanAngle);//reverse
    }

    if (isProtractor)
        emit newMeasure(qAbs(spanAngle));

    update();
    event->accept();

}


/**
 * @brief   implement virtual function paint() to draw the boundaries of the arc and the unit label
 */
void Item_arcDrawer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(pen());

    //Draw first boundary of the angle
    QLineF58 firstBoundary;

    if(firstVector.isNull())
        firstBoundary = QLineF58(QPointF(0,0), mapFromScene(mouseMovePos));
    else
        firstBoundary = QLineF58(0, 0, firstVector.dx(), firstVector.dy());

    firstBoundary.setLength(bRadius);
    firstBoundary.setP1(firstBoundary.center());//visual effect
    painter->drawLine(firstBoundary);

    //Draw 2nd boundary and the angle label showing the degrees
    if(!secondVector.isNull())
    {
        secondVector.setLength(bRadius);
        QLineF58 secondBoundary(0, 0, secondVector.dx(), secondVector.dy());
        secondBoundary.setP1(secondBoundary.center());

        painter->drawLine(secondBoundary);
        if (paintArc)
            painter->drawArc(QRectF(-radius, -radius, radius*2, radius*2),
                             startAngle*16, spanAngle*16);

        //Show the value of the drawn angle, pixel size of the arc to consider!
        const QString angle = QString::number(qAbs(spanAngle), 'f', 1)+"°";
        QPen textPen;
        textPen.setColor(Qt::black);
        painter->setPen(textPen);

        QFont font;
        QPointF textPos;
        if( bRadius*.2 < 20 )//20px
        {
            font.setPixelSize(20);
            textPos = QPointF(-bRadius*.5, 0);
        }
        else
        {
            font.setPixelSize(bRadius*.2);
            textPos = QPointF(-bRadius*.2, 0);
        }
        painter->setFont(font);
        painter->drawText(textPos, angle);
    }

}



