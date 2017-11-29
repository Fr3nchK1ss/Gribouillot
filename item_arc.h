/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef ITEM_ARC_H
#define ITEM_ARC_H

#include <QAbstractGraphicsShapeItem>
#include <QKeyEvent>
#include <QPainterPath>
#include <QXmlStreamWriter>

#include "gribouillotitem.h"

class Item_arc : public QAbstractGraphicsShapeItem
{
public:
    enum { Type = ARC };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return ARC;
    }
    explicit Item_arc(QPointF center, QRectF rect, qreal startAngle, qreal spanAngle,
                      QColor penColor = Qt::black, int penWidth = 3);
    ~Item_arc(){}

    void newPen(QColor penColor, int penWidth);
    qreal getRadius() const;
    qreal getSpanAngle() const;
    void serialize2xml(QXmlStreamWriter* w);
    QString status(qreal scale);

protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;


private:
    QPointF startPoint;
    qreal startAngle;
    qreal spanAngle;
    QRectF arcRect;

    qreal selectionMargin;

};



#endif // ITEM_ARC_H
