/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QDebug>
#include <QLabel>
#include <QPainter>

#include "dlg_setupgps.h"
#include "item_point.h"

Item_point::Item_point(QColor brushColor, qreal penWidth, QPointF position,
                       int weight):
    /**
     * x and y of QGraphicsEllipseItem() defines the top-left corner of the
     * bounding Rect in the own item reference system.
     */
    QGraphicsEllipseItem()
{
    /* Point is "cosmetic" in its own way */
    setFlag(QGraphicsItem::ItemIgnoresTransformations);

    //outline color
    setPen(QPen(Qt::black));
    newBrush(brushColor, penWidth);
    //position
    setPos(position.x(), position.y());


    /* Create a weight label associated to this point */
    QLabel *label = new QLabel(QString::number(0));
    weightLabel = new QGraphicsProxyWidget(this);
    QPalette palette;
    QFont font;

    /* Setting and displaying point weight */
    palette.setBrush(QPalette::Window, Qt::transparent);
    palette.setBrush(QPalette::WindowText, brushColor);
    label->setPalette(palette);
    font.setPixelSize(20);
    font.setBold(true);
    label->setFont(font);
    label->move(7,-2);
    weightLabel->setWidget(label);
    setWeight(weight);
    showWeight(false);//weight label hidden by default
}


void Item_point::newBrush(QColor brushColor, int penWidth)
{
    //filling color
    setBrush(QBrush(brushColor));
    //Minimum sensible width for a point is 3px
    if ( penWidth < 3)
        penWidth = 3;

    setRect(-penWidth/2.0, -penWidth/2.0, penWidth, penWidth);

}



/**
 * @brief   set a new weight for this point, stored as an item data
 */
int Item_point::getWeight()
{
    QLabel *label = dynamic_cast<QLabel*>(weightLabel->widget());

    return label->text().toInt();

}

/**
 * @brief   set a new weight for this point, stored as an item data
 */
void Item_point::setWeight(int w)
{
    QLabel *label = dynamic_cast<QLabel*>(weightLabel->widget());
    label->setText(QString::number(w));
    label->adjustSize();
}



/**
 * @brief   toggle the display of the point weight
 */
void Item_point::showWeight(bool isDisplayed)
{
    if(isDisplayed && getWeight() != 0)
        weightLabel->show();
    else
        weightLabel->hide();
}


void Item_point::serialize2xml(QXmlStreamWriter* w)
{
    w->writeStartElement("Point");
    w->writeAttribute("x", QString::number(x()));
    w->writeAttribute("y", QString::number(y()));
    w->writeAttribute("color", brush().color().name());
    w->writeAttribute("penWidth", QString::number(rect().width()));
    w->writeAttribute("weight", QString::number(getWeight()));
    w->writeEndElement();

}

