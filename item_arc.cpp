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

#include "dlg_arcangle.h"
#include "item_arc.h"

Item_arc::Item_arc(QPointF center, QRectF rect, qreal startAngle, qreal spanAngle,
                   QColor penColor, int penWidth)
{
    //Focusable: to respond to user's keyboard activity
    setFlag(QGraphicsItem::ItemIsFocusable);

    setPos(center);
    arcRect = rect;
    this->startAngle = startAngle;
    this->spanAngle = spanAngle;

    newPen(penColor, penWidth);//will call createBoundingPath
}


/**
 * @brief Item_arc::newPen
 */
void Item_arc::newPen(QColor penColor, int penWidth)
{
    QPen pen(penColor, penWidth);
    pen.setCosmetic(true);
    setPen(pen);

    selectionMargin = penWidth/2+5;//in px
    update();
}


/**
 * @brief Item_arc::getRadius accessor
 */
qreal Item_arc::getRadius() const
{
    return arcRect.width()/(qreal)2;

}


/**
 * @brief Item_arc::getSpanAngle accessor
 */
qreal Item_arc::getSpanAngle() const
{
    return spanAngle;

}


/**
 * @brief   return a message to be displayed in the statusBar
 */
QString Item_arc::status(qreal scale)
{
    qreal arcPixelLength = 2*M_PI*getRadius()*(spanAngle/(qreal)360);

    return "Span angle: "+QString::number(qAbs(spanAngle))+"°"
           +"  Arc length: "+QString::number(qAbs(arcPixelLength)*scale)+"km ";
}


/**
 * @brief Item_arc::serialize2xml
 */
void Item_arc::serialize2xml(QXmlStreamWriter *w)
{
    w->writeStartElement("Arc");
    w->writeAttribute("x", QString::number(x()));
    w->writeAttribute("y", QString::number(y()));
    w->writeAttribute("radius", QString::number(getRadius()));
    w->writeAttribute("startAngle", QString::number(startAngle));
    w->writeAttribute("spanAngle", QString::number(spanAngle));
    w->writeAttribute("color", pen().color().name());
    w->writeAttribute("penWidth", QString::number(pen().width()));
    w->writeEndElement();
}



/********************** Protected functions OVERRIDE *********************/
/**
 * @brief   implement virtual function boundingRect()
 */
QRectF Item_arc::boundingRect() const
{
    QPointF offset(selectionMargin, selectionMargin);
    QRectF bRect(arcRect.topLeft() - offset,
                arcRect.bottomRight() + offset);

    return bRect;

}


/**
 * @brief   A collision shape for an arc. In LOCAL coordinates!
 * @details The shape corresponds to the visual 'selection box'.
 */
QPainterPath Item_arc::shape() const
{
    QLineF radius(0,0,1,0);
    QVector<QPointF> coords;
    QPainterPath path;

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

    return path;

}


/**
 * @brief   implement virtual function paint() to draw an arc
 */
void Item_arc::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(this->pen());
    painter->drawArc(arcRect, startAngle*16, spanAngle*16);

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


/**
 * @brief   Give keyboard focus to this item when it become selected.
 * \warning SAME CODE as in other Gribouillot Item_. If you change this code
 *          you probably need to change the code of other Item. Would need a mixin.
 */
QVariant Item_arc::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged)
    {
        setFocus();
        //qDebug() << "change in focus " << this;
        return QVariant();//unused anyway
    }
    //Else, other changes
    return QGraphicsItem::itemChange(change, value);

}


/**
 * @brief   React to keyboard input. Change scale.
 * @details The item needs to have focus to receive keyboard input,
 *          see itemChange().
 */
void Item_arc::keyPressEvent(QKeyEvent *event)
{
    switch ( event->key() )
    {
        case Qt::Key_A:
        {
            Dlg_arcAngle dialog(spanAngle);
            if (dialog.exec() == QDialog::Accepted)
                spanAngle = dialog.getAngle();

        }
            update();
            break;

        default:
            QGraphicsItem::keyPressEvent(event);

    }
}






