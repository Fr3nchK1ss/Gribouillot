/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef ITEM_SPIRAL_H
#define ITEM_SPIRAL_H

#include <QAbstractGraphicsShapeItem>
#include <QDomDocument>
#include <QKeyEvent>
#include <QPainterPath>
#include <QRectF>
#include <QXmlStreamWriter>

#include "gribouillotitem.h"

class Item_spiral : public QAbstractGraphicsShapeItem
{
public:
    enum { Type = SPIRAL };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return SPIRAL;
    }
    explicit Item_spiral(QColor penColor, int penWidth, QVector<QPointF> base);
    explicit Item_spiral(QDomElement e);
    ~Item_spiral(){}

    void newPen(QColor penColor, int penWidth);
    static qreal computeArcRadius(int sideNumber, QVector<QLineF> side);
    static void computeArcAngles(QLineF sideA, QLineF sideB,
                                 qreal &startAngle, qreal &spanAngle);
    void serialize2xml(QXmlStreamWriter* w);
    QString status(qreal scale);

protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    QVector<QLineF> baseSide;
    QVector<QRectF> arcsRect;
    qreal startAngles[4];
    qreal spanAngles[4];

    bool clockwise;
    qreal selectionMargin;

};

#endif
