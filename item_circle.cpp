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
#include <QtMath>

#include "item_circle.h"
#include "gribouillotscene.h"

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


/**
 * @brief       Construct a circle from saved XML data
 * @details     Be sure to call with a QDomElement containing
 *              circle data!
 * @attention   see also serialize2xml
 */
Item_circle::Item_circle(QDomElement e) :
    Item_circle(QColor(e.attribute("color")),
                e.attribute("penWidth").toInt(),
                QPointF(e.attribute("x").toDouble(), e.attribute("y").toDouble()),
                e.attribute("radius").toDouble())
{}


/**
 * @brief Item_circle::newPen
 */
void Item_circle::newPen(QColor penColor, int penWidth)
{
    QPen pen(penColor, penWidth);
    pen.setCosmetic(true);//Very important to keep circles visible when zooming out.
    pen.setCapStyle(Qt::FlatCap);
    setPen(pen);

    selectionMargin = penWidth/2+5;
    update();
}


/**
 * @brief Item_circle::getRadius accessor
 */
qreal Item_circle::getRadius() const
{
    return rect().width()/(qreal)2;
}


/**
 * @brief Item_circle::serialize2xml
 */
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
QString Item_circle::status(qreal scale, QString scaleUnit)
{
    qreal scaledRadius = rect().width()/2*scale;

    return QObject::tr("Radius: ")+QString::number(scaledRadius, 'f', 1)+scaleUnit+"    "
            +QObject::tr("Circumference: ")+QString::number(M_PI*scaledRadius, 'f', 1)+scaleUnit;

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
 *          code in paint() with a drawPath(path) to see the result.
 */
QPainterPath Item_circle::shape() const
{
    QLineF radius(0,0,1,0);
    QPointF offset(selectionMargin, selectionMargin);
    QVector<QPointF> coords;

    radius.setLength(getRadius()-selectionMargin);
    coords << radius.p2();
    radius.setLength(getRadius()+selectionMargin);
    coords << radius.p2();
    radius.setAngle(359.99);
    radius.setLength(getRadius()-selectionMargin);
    coords << radius.p2();

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
        //qreal inRadius = getRadius()-selectionMargin;
        qreal outRadius = getRadius()+selectionMargin;

        foreach(QPen selectPen, dynamic_cast<GribouillotScene*>(scene())->getSelectPens())
        {
            painter->setPen(selectPen);
            painter->drawEllipse(QPointF(), outRadius, outRadius);
            //painter->drawEllipse(QPointF(), inRadius, inRadius);
        }

    }

}

