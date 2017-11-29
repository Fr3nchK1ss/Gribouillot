/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QtMath>

#include "item_spiral.h"

Item_spiral::Item_spiral(QVector<QPointF> base, QColor penColor, int penWidth)
{
    setPos(base[0]);

    for(int i = 0; i < base.length(); ++i)
    {
        baseSide << QLineF(base[i], base[(i+1)%4]);
        //Compute arcs rectangles already
        arcsRect << computeArcRect(i, baseSide);
        boundingRect();
    }

    newPen(penColor, penWidth);//will call createBoundingPath
}


/**
 * @brief Item_spiral::newPen
 */
void Item_spiral::newPen(QColor penColor, int penWidth)
{
    QPen pen(penColor, penWidth);
    pen.setCosmetic(true);
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
           "side1 = "+QString::number(qAbs(baseSide[0].length())*scale, 'f', 2)+"km  "+
           "side2 = "+QString::number(qAbs(baseSide[1].length())*scale, 'f', 2)+"km  "+
           "side3 = "+QString::number(qAbs(baseSide[2].length())*scale, 'f', 2)+"km  "+
           "side4 = "+QString::number(qAbs(baseSide[3].length())*scale, 'f', 2)+"km  ";

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
 * @brief Compute the rectangle necessary to draw the arc of a
 *        specific side. The rectangle is in local coordinates.
 */
QRectF Item_spiral::computeArcRect(int sideNumber, QVector<QLineF> side)
{
    QRectF arcRect = QRectF();
    QLineF s = side[sideNumber];//For convenience

    if(!s.isNull())
    {
        /*
         * The rectangle half-width for side 'i' is the length of the
         * current side plus the length of the previous sides if any:
         * -> this is how a spiral grows!
         */
        qreal radius = s.length();
        for (int i = 0; i < sideNumber; ++i)
            radius += side[i].length();

        /*
         * The arc rectangle is by definition centered on 0,
         * in the local coordinates of the arc.
         */
        QLineF horizontal(QPointF(0,0), QPointF(-radius,0));
        //horizontal.setLength(-radius);


        //halfLeftSide is the upper-half part of the left side of the rectangle.
        QLineF halfLeftSide = QLineF(horizontal.p2(), horizontal.p1()).normalVector();
        QPointF topLeft = halfLeftSide.p2();
        arcRect = QRectF(topLeft, QSize(2*radius, 2*radius));

    }

    return arcRect;
}


/**
 * @brief Compute the startAngle and spanAngle of the arc in-between side1
 *        and side2.
 */
void Item_spiral::computeArcAngles(QLineF side1, QLineF side2,
                                   qreal &startAngle, qreal &spanAngle)
{

    if(!side1.isNull() && !side2.isNull())
    {
        //qDebug() << "angles for side: " << side1;

        QLineF horizontal(side1.p1(), side1.p1()+QPointF(1,0));
        QLineF flip(side1.p2(), side1.p1());

        startAngle = horizontal.angleTo(flip);
        //qDebug() << "start: " << startAngle;

        spanAngle = side1.angleTo(side2);//counter-clockwise is Qt default
        if(spanAngle > 180)
            //the spiral goes clockwise
            spanAngle = flip.angleTo(side2) - 180;
        //qDebug() << "span: " << spanAngle;
    }
}



/********************** Protected functions OVERRIDE *********************/
/**
 * @brief   implement virtual function boundingRect()
 */
QRectF Item_spiral::boundingRect() const
{
    qreal radius = 0;

    //Compute longest radius of the spiral (= sum of each side)
    for (int i = 0; i < baseSide.length(); ++i)
        radius += baseSide[i].length();
    //radius += baseSide[0].length();

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
    QLineF radius(0,0,1,0);
    QVector<QPointF> coords;
    QPainterPath path;
/*
    radius.setAngle(startAngle);
    radius.setLength(getRadius()-selectionMargin);
    coords << radius.p2();
    radius.setLength(getRadius()+selectionMargin);
    coords << radius.p2();
    radius.setAngle(startAngle+spanAngle);
    radius.setLength(getRadius()-selectionMargin);
    coords << radius.p2();

    QPointF offset(selectionMargin, selectionMargin);
    QRectF inRect(arcRect.topLeft() + offset,
                    arcRect.bottomRight() - offset);
    QRectF outRect = boundingRect();

    path.moveTo(coords[0]);
    path.lineTo(coords[1]);
    path.arcTo(outRect, startAngle, spanAngle);
    path.lineTo(coords[2]);
    path.arcTo(inRect, startAngle+spanAngle, -spanAngle);//back to coords[0]
    path.closeSubpath();


*/
    path.addRect(boundingRect());
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
    qreal startAngle, spanAngle;
    QRectF arcRect;

    painter->setPen(this->pen());

    for(int i = 0; i < baseSide.size(); ++i)
    {
        computeArcAngles(baseSide[i], baseSide[(i+1)%4], startAngle, spanAngle);
        arcRect = arcsRect[i].translated(mapFromScene(baseSide[i].p2()));
        painter->drawArc(arcRect, startAngle*16, spanAngle*16);
    }

    //painter->setPen(QPen(Qt::blue));
    //painter->drawRect(boundingRect());

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
        painter->drawPath(shape());
        painter->setPen(selectPen2);
        painter->drawPath(shape());
    }

}
