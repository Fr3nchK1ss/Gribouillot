/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef ITEM_POINT_H
#define ITEM_POINT_H

#include <QColor>
#include <QKeyEvent>
#include <QGraphicsEllipseItem>
#include <QGraphicsProxyWidget>
#include <QPointF>
#include <QPen>
#include <QXmlStreamWriter>

#include "gribouillotitem.h"


class Item_point : public QGraphicsEllipseItem
{
public:
    enum { Type = POINT_W };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return POINT_W;
    }

    explicit Item_point(QColor brushColor, qreal penWidth, QPointF position,
                        int weight = 0);
    ~Item_point(){}

    int getWeight();
    void setWeight(int w);
    void showWeight(bool isDisplayed);

    void newBrush(QColor brushColor, int penWidth);
    void serialize2xml(QXmlStreamWriter* w);

private:
    QGraphicsProxyWidget* weightLabel;



};

#endif // ITEM_POINT_H
