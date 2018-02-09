/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */


#ifndef ITEM_POINTONRAIL_H
#define ITEM_POINTONRAIL_H


#include <QColor>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QLineF>
#include <QPen>

#include "gribouillotitem.h"
#include "qlinef58.h"

class Item_pointOnRail: public QGraphicsEllipseItem
{
public:
    enum { Type = POINT_ONRAIL };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return POINT_ONRAIL;
    }

    explicit Item_pointOnRail(qreal penWidth, QLineF line);
    ~Item_pointOnRail(){}

    QLineF getRail();

protected:
    //void mousePressEvent(QGraphicsSceneMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event) Q_DECL_OVERRIDE;

private:
    QLineF58 rail;

};

#endif // ITEM_POINTONRAIL_H
