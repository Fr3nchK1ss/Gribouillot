/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef ITEM_CIRCLE_H
#define ITEM_CIRCLE_H

#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QKeyEvent>
#include <QXmlStreamWriter>

#include "gribouillotitem.h"

class Item_circle : public QGraphicsEllipseItem
{
public:
    enum { Type = CIRCLE };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return CIRCLE;
    }
    explicit Item_circle(QColor penColor, int penWidth, QPointF center, qreal radius);
    ~Item_circle(){}

    void newPen(QColor penColor, int penWidth);
    qreal getRadius() const;
    void serialize2xml(QXmlStreamWriter* w);
    QString status(qreal scale);

protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint (QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget = 0) Q_DECL_OVERRIDE;

private:
    int selectionMargin;

};

#endif // ITEM_CIRCLE_H
