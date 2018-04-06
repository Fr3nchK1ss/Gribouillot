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
#include <QStyleOptionGraphicsItem>
#include <QtMath>

#include "item_spiral.h"
#include "gribouillotscene.h"

Item_spiral::Item_spiral(QColor penColor, int penWidth, QVector<QPointF> base)
{
    qreal startAngle, spanAngle;

    setPos(base[0]);

    //compute the 4 sides from the 4 centers
    for(int i = 0; i < base.length(); ++i)
        baseSide << QLineF(base[i], base[(i+1)%4]);


    for(int i = 0; i < base.length(); ++i)
    {
        //Retrieve arcRect for baseSide[i] in zero-centered coordinates
        QRectF zeroCenteredArcRect = computeArcRect(i, baseSide);
        //Position the arcRect correctly in spiral local coordinates
        arcsRect << zeroCenteredArcRect.translated(mapFromScene(baseSide[i].p2()));

        //compute the arcs angles
        computeArcAngles(baseSide[i], baseSide[(i+1)%4], startAngle, spanAngle);
        startAngles[i] = startAngle;
        spanAngles[i] = spanAngle;
    }

    //recalculate boundingRect for proper display
    boundingRect();

    newPen(penColor, penWidth);
}


/**
 * @brief Item_spiral::newPen
 */
void Item_spiral::newPen(QColor penColor, int penWidth)
{
    QPen pen(penColor, penWidth);
    pen.setCosmetic(true);
    pen.setCapStyle(Qt::FlatCap);
    setPen(pen);

    selectionMargin = penWidth/2+5;//in px
    update();
}


/**
 * @brief   return a message to be displayed in the statusBar
 */
QString Item_spiral::status(qreal scale)
{
    return QObject::tr("Spiral:  ")+
           "side1 = "+QString::number(qAbs(baseSide[0].length())*scale, 'f', 1)+"km  "+
           "side2 = "+QString::number(qAbs(baseSide[1].length())*scale, 'f', 1)+"km  "+
           "side3 = "+QString::number(qAbs(baseSide[2].length())*scale, 'f', 1)+"km  "+
           "side4 = "+QString::number(qAbs(baseSide[3].length())*scale, 'f', 1)+"km  ";

    return QString();
}


/**
 * @brief Item_spiral::serialize2xml
 */
void Item_spiral::serialize2xml(QXmlStreamWriter *w)
{
    w->writeStartElement("Spiral");
    for(int i = 0; i < 4; i++)
    {
        w->writeAttribute("x"+QString::number(i),
                          QString::number(baseSide[i].p1().x()));

        w->writeAttribute("y"+QString::number(i),
                          QString::number(baseSide[i].p1().y()));
    }
    w->writeAttribute("color", pen().color().name());
    w->writeAttribute("penWidth", QString::number(pen().width()));
    w->writeEndElement();
}



/********************** Public static functions *********************/
/**
 * @brief   Compute the rectangle necessary to draw the arc of a
 *          specific side in coordinates centered on zero.
 * @details static public function
 */
QRectF Item_spiral::computeArcRect(int sideNumber, QVector<QLineF> side)
{
    QRectF arcRect;
    QLineF s = side[sideNumber];//For convenience

    if(!s.isNull())
    {
        /*
         * The arcRect half-width for side 'i' is the length of the
         * current side plus the length of the previous sides if any:
         * -> this is how a spiral grows!
         */
        qreal radius = s.length();
        for (int i = 0; i < sideNumber; ++i)
            radius += side[i].length();

        //Build the rectangle of proper radius
        QLineF horizontal(QPointF(0,0), QPointF(-radius,0));
        //halfLeftSide is the upper-half part of the left side of the rectangle.
        QLineF halfLeftSide = QLineF(horizontal.p2(), horizontal.p1()).normalVector();
        QPointF topLeft = halfLeftSide.p2();
        arcRect = QRectF(topLeft, QSizeF(2*radius, 2*radius));

    }

    return arcRect;
}



/**
 * @brief Compute the startAngle and spanAngle of the arc in-between sideA
 *        and sideB.
 */
void Item_spiral::computeArcAngles(QLineF sideA, QLineF sideB,
                                   qreal &startAngle, qreal &spanAngle)
{

    if(!sideA.isNull() && !sideB.isNull())
    {
        //qDebug() << "angles for side: " << side1;
        QLineF horizontal(sideA.p1(), sideA.p1()+QPointF(1,0));
        QLineF flip(sideA.p2(), sideA.p1());

        startAngle = horizontal.angleTo(flip);
        //qDebug() << "start: " << startAngle;

        spanAngle = sideA.angleTo(sideB);//counter-clockwise is Qt default
        if(spanAngle > 180)
            //the spiral goes clockwise
            spanAngle = flip.angleTo(sideB) - 180;
        //qDebug() << "span: " << spanAngle;
    }
}



/********************** Protected functions OVERRIDE *********************/
/**
 * @brief   implement virtual function boundingRect()
 */
QRectF Item_spiral::boundingRect() const
{
    //arcRect[3] is mathematically the biggest arc of the spiral
    qreal radius = arcsRect[3].width()/2.0;

    QRectF bRect( -(radius + selectionMargin), -(radius + selectionMargin),
                  2* (radius + selectionMargin), 2* (radius + selectionMargin));

    return bRect;

}


/**
 * @brief   A collision shape for the spiral. In LOCAL coordinates!
 * @details The shape corresponds to the visual 'selection box'.
 */
QPainterPath Item_spiral::shape() const
{
    QVector<QPointF> coords;
    QPainterPath path;
    QPointF offset(selectionMargin, selectionMargin);
    QRectF offsetArcRect;

    //Calculate coordinates of key points of the path
    QLineF s0(mapFromScene(baseSide[0].p2()), mapFromScene(baseSide[0].p1()));//reverse baseSide[0]
    s0.setLength(s0.length() + selectionMargin);
    coords << s0.p2();
    s0.setLength(s0.length() + arcsRect[3].width()/2 - selectionMargin);
    coords << s0.p2();

    //Move to the starting point of the path
    path.moveTo(coords[0]);

    //Outer arcs path
    for (int i = 0; i < arcsRect.size(); ++i )
    {
        offsetArcRect= QRectF(arcsRect[i].topLeft()-offset, arcsRect[i].bottomRight() + offset);
        path.arcTo(offsetArcRect,
                   startAngles[i],
                   spanAngles[i]);
    }

    //Link outer and inner arcs
    path.lineTo(coords[1]);

    //Inner arcs path
    for (int i = arcsRect.size()-1; i >= 0; --i )
    {
        //Build the arcRect of the path
        offsetArcRect= QRectF(arcsRect[i].topLeft() + offset, arcsRect[i].bottomRight() - offset);
        path.arcTo(offsetArcRect,
                   startAngles[i]+spanAngles[i],
                   -spanAngles[i]);
    }

    //Link inner and outer arcs, close path
    path.lineTo(coords[0]);
    path.closeSubpath();

    return path;

}


/**
 * @brief   implement virtual function paint() to draw a spiral as one graphic item
 * @todo    For the moment, the spiral is in fact 4 Arcs. Could be a painterPath.
 */
void Item_spiral::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(this->pen());

    for(int i = 0; i < arcsRect.size(); ++i)
    {
        painter->drawArc(arcsRect[i], startAngles[i]*16, spanAngles[i]*16);
    }

    //painter->setPen(QPen(Qt::blue));
    //painter->drawRect(boundingRect());

    if (isSelected())
    {
        foreach(QPen selectPen, dynamic_cast<GribouillotScene*>(scene())->getSelectPens())
        {
            painter->setPen(selectPen);
            painter->drawPath(shape());
        }

    }

}
