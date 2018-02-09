/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QDebug>
#include <QPen>

#include "item_circle.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

Item_circle::Item_circle(QColor penColor, int penWidth, QPointF center,
                         qreal radius):
    /**
     * x and y of Item_Circle() defines position of the center but x and y of
     * QGraphicsEllipseItem() defines the top-left corner of the bounding Rect in
     * the own item reference system, hence the '-radius'.
     */
    QGraphicsEllipseItem(-radius, -radius, radius*2, radius*2)
{
    setPos(center);
    newPen(penColor, penWidth);

}


void Item_circle::newPen(QColor penColor, int penWidth)
{
    QPen pen(penColor, penWidth);
    pen.setCosmetic(true);//Very important to keep circles visible when zooming out.
    setPen(pen);

    selectionMargin = penWidth/2+5;
    update();
}


qreal Item_circle::getRadius() const
{
    return rect().width()/(qreal)2;
}


void Item_circle::serialize2xml(QXmlStreamWriter* w)
{
    w->writeStartElement("Circle");
    w->writeAttribute("x", QString::number(x()));
    w->writeAttribute("y", QString::number(y()));
    w->writeAttribute("radius", QString::number(getRadius()));
    w->writeAttribute("color", pen().color().name());
    w->writeAttribute("penWidth", QString::number(pen().width()));
    w->writeEndElement();
}


/**
 * @brief   return a message to be displayed in the statusBar
 */
QString Item_circle::status(qreal scale)
{
    qreal kmRadius = rect().width()/2*scale;

    return QObject::tr("Radius: ")+QString::number(kmRadius)+" km"
            +QObject::tr("    Circumference: ")+QString::number(M_PI*kmRadius)+" km";

}



/***************** protected re-implemented function *******************/
QRectF Item_circle::boundingRect() const
{
    QPointF offset(selectionMargin, selectionMargin);
    QRectF bRect(rect().topLeft() - offset,
                rect().bottomRight() + offset);

    return bRect;
}


/**
 * @brief   A collision shape for a circle. In LOCAL coordinates!
 * @details Defined as an almost closed circular band. Paste this
 *          code in 'paint' with a drawPath(path) to see the result.
 */
QPainterPath Item_circle::shape() const
{
    QLineF radius(0,0,1,0);
    QVector<QPointF> coords;

    radius.setLength(getRadius()-selectionMargin);
    coords << radius.p2();
    radius.setLength(getRadius()+selectionMargin);
    coords << radius.p2();
    radius.setAngle(359.99);
    radius.setLength(getRadius()-selectionMargin);
    coords << radius.p2();

    QPointF offset(selectionMargin, selectionMargin);
    QRectF inRect(rect().topLeft() + offset,
                 rect().bottomRight() - offset);
    QRectF outRect = boundingRect();

    QPainterPath path;
    path.moveTo(coords[0]);
    path.lineTo(coords[1]);
    path.arcTo(outRect, 0, 359.99);
    path.lineTo(coords[2]);
    path.arcTo(inRect, -0.01, -359.99);//back to coords[0]
    path.closeSubpath();

    return path;

}


void Item_circle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(pen());
    painter->drawEllipse(QPointF(0,0), getRadius(), getRadius());

    if (isSelected())
    {
        qreal inRadius = getRadius()-selectionMargin;
        qreal outRadius = getRadius()+selectionMargin;
        //draw a selection box similar to Qt's
        QPen selectPen1(Qt::black);
        selectPen1.setDashPattern(QVector<qreal>({4,2}));
        selectPen1.setCosmetic(true);

        QPen selectPen2(Qt::white);
        selectPen2.setDashOffset(2);
        selectPen2.setDashPattern(QVector<qreal>({2,4}));
        selectPen2.setCosmetic(true);

        painter->setPen(selectPen1);
        painter->drawEllipse(QPointF(), inRadius, inRadius);
        painter->drawEllipse(QPointF(), outRadius, outRadius);
        painter->setPen(selectPen2);
        painter->drawEllipse(QPointF(), inRadius, inRadius);
        painter->drawEllipse(QPointF(), outRadius, outRadius);
    }

}

