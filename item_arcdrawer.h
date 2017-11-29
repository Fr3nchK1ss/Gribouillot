/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef ITEM_ARCDRAWER_H
#define ITEM_ARCDRAWER_H

#include <QAbstractGraphicsShapeItem>
#include <QGraphicsSceneMouseEvent>

#include "item_arc.h"
#include "item_circle.h"
#include "item_line.h"
#include "gribouillotitem.h"


class Item_arcDrawer : public QAbstractGraphicsShapeItem
{
public:
    enum { Type = ARC_DRAWER };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return ARC_DRAWER;
    }

    explicit Item_arcDrawer(Item_circle* circle);
    explicit Item_arcDrawer(int penWidth, QColor c, QPointF center, QPointF radiusPoint);
    explicit Item_arcDrawer(int penWidth, QColor c, int windowSize, QPointF center,
                            QLineF sourceLine);
    explicit Item_arcDrawer(QPointF center, QSizeF picSize);
    ~Item_arcDrawer(){}

    Item_arc* getArc();
    Item_circle* getSourceCircle();
    QLineF getAngledLine();

protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) Q_DECL_OVERRIDE;
    void mousePressEvent(QGraphicsSceneMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event) Q_DECL_OVERRIDE;

private:
    qreal startAngle;
    qreal spanAngle;
    qreal radius;
    qreal bRadius;

    QLineF firstVector;
    QLineF secondVector;
    bool paintOnly1rstBoundary;
    bool paintArc;
    bool startClockwise = false;

    Item_circle* savedCircle = nullptr;

};

#endif // ITEM_ARCDRAWER_H
