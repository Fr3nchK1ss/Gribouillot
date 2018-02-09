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

Item_segment::Item_segment(QColor penColor, int penWidth, QPointF points[]):
    QGraphicsLineItem(QLineF(points[0], points[1]))
{
    newPen(penColor, penWidth);
    //call createSelectionPolygon
}


/**
 * @brief   Create a selection box which respects the Qline's angle
 * @details Implies a new implementation of paint(), shape() and
 *          boundingRect()
 */
void Item_segment::createSelectionPolygon(){
    QPolygonF nPolygon;
    int selectionOffset = pen().width()+5;

    qreal radAngle = qDegreesToRadians(line().angle());
    qreal dx = selectionOffset * qSin(radAngle);
    qreal dy = selectionOffset * qCos(radAngle);
    QPointF offset1 = QPointF(dx, dy);
    QPointF offset2 = QPointF(-dx, -dy);
    nPolygon << line().p1() + offset1
             << line().p1() + offset2
             << line().p2() + offset2
             << line().p2() + offset1;
    selectionPolygon = nPolygon;

    update();

}


void Item_segment::newPen(QColor penColor, int penWidth)
{
    QPen pen(penColor, penWidth);
    pen.setCosmetic(true);//Very important to keep circles visible when zooming out.
    setPen(pen);

    createSelectionPolygon();//depends on penWidth
}


void Item_segment::serialize2xml(QXmlStreamWriter* w)
{
    w->writeStartElement("Segment");
    w->writeAttribute("x1", QString::number(line().x1()));
    w->writeAttribute("y1", QString::number(line().y1()));
    w->writeAttribute("x2", QString::number(line().x2()));
    w->writeAttribute("y2", QString::number(line().y2()));
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

    return QObject::tr("Segment with length ")+QString::number(kmLength)+" km";

}


QRectF Item_segment::boundingRect() const
{
    return selectionPolygon.boundingRect();
}


QPainterPath Item_segment::shape() const
{
    QPainterPath ret;
    ret.addPolygon(selectionPolygon);
    return ret;
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
        //draw a selection box similar to Qt's
        QPen selectPen1(Qt::black);
        selectPen1.setDashPattern(QVector<qreal>({4,2}));
        selectPen1.setCosmetic(true);

        QPen selectPen2(Qt::white);
        selectPen2.setDashOffset(2);
        selectPen2.setDashPattern(QVector<qreal>({2,4}));
        selectPen2.setCosmetic(true);

        painter->setPen(selectPen1);
        painter->drawPolygon(selectionPolygon);
        painter->setPen(selectPen2);
        painter->drawPolygon(selectionPolygon);
    }

}
