/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QDebug>
#include <QLineF>
#include <QPen>

#include "item_line.h"

int Item_line::number = 0;

Item_line::Item_line(QColor penColor, int penWidth, QPointF points[]) :
    Item_segment(penColor, penWidth, points)
{
    tag = ++number;
    //qDebug() << "new line" << tag;
}


/********************** Public functions *********************/

void Item_line::serialize2xml(QXmlStreamWriter* w)
{
    w->writeStartElement("Line");
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
QString Item_line::status()
{
    return QObject::tr("Infinite line n°")+QString::number(tag);

}

