/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QtMath>

#include "item_segment.h"
#include "gribouillotscene.h"

Item_segment::Item_segment(QColor penColor, int penWidth, QVector<QPointF> points):
    QGraphicsLineItem(QLineF(points[0], points[1]))
{
    newPen(penColor, penWidth);

}


/**
 * @brief       Construct a segment from saved XML data
 * @details     Be sure to call with a QDomElement containing
 *              segment data!
 * @attention   see also serialize2xml
 */
Item_segment::Item_segment(QDomElement e) :
    Item_segment(QColor(e.attribute("color")),
                 e.attribute("penWidth").toInt(),
                 {QPointF(e.attribute("x0").toDouble(), e.attribute("y0").toDouble()),
                  QPointF(e.attribute("x1").toDouble(), e.attribute("y1").toDouble())})
{}


/**
 * @brief Define the drawing pen
 */
void Item_segment::newPen(QColor penColor, int penWidth)
{
    QPen pen(penColor, penWidth);
    pen.setCosmetic(true);//Very important to keep circles visible when zooming out.
    pen.setCapStyle(Qt::FlatCap);
    setPen(pen);

    createSelectionPolygon(penWidth);
    update();
}


/**
 * @brief   Create a selection box which respects the Qline's angle
 * @details Implies a new implementation of paint(), shape() and
 *          boundingRect()
 */
void Item_segment::createSelectionPolygon(int penWidth)
{
    QPolygonF nPolygon;
    int selectionMargin = penWidth+5;

    qreal radAngle = qDegreesToRadians(line().angle());
    qreal dx = selectionMargin * qSin(radAngle);
    qreal dy = selectionMargin * qCos(radAngle);
    QPointF offset1 = QPointF(dx, dy);
    QPointF offset2 = QPointF(-dx, -dy);
    nPolygon << line().p1() + offset1
             << line().p1() + offset2
             << line().p2() + offset2
             << line().p2() + offset1;
    selectionPolygon = nPolygon;

}



void Item_segment::serialize2xml(QXmlStreamWriter* w)
{
    w->writeStartElement("Segment");
    w->writeAttribute("x0", QString::number(line().x1()));
    w->writeAttribute("y0", QString::number(line().y1()));
    w->writeAttribute("x1", QString::number(line().x2()));
    w->writeAttribute("y1", QString::number(line().y2()));
    w->writeAttribute("color", pen().color().name());
    w->writeAttribute("penWidth", QString::number(pen().width()));
    w->writeEndElement();

}


/**
 * @brief   return a message to be displayed in the statusBar
 */
QString Item_segment::status(qreal scale)
{
    qreal kmLength = line().length()*scale;

    return QObject::tr("Segment with length ")+QString::number(kmLength, 'f', 1)+" km";

}


QRectF Item_segment::boundingRect() const
{
    return selectionPolygon.boundingRect();
}


QPainterPath Item_segment::shape() const
{
    QPainterPath path;
    path.addPolygon(selectionPolygon);
    return path;
}


void Item_segment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(pen());
    painter->drawLine(line());


    if (isSelected())
    {
        foreach(QPen selectPen, dynamic_cast<GribouillotScene*>(scene())->getSelectPens())
        {
            painter->setPen(selectPen);
            painter->drawPolygon(selectionPolygon);
        }

    }



}
