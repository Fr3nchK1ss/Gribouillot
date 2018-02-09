/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef ITEM_SPIRALDRAWER_H
#define ITEM_SPIRALDRAWER_H

#include <QObject>
#include <QAbstractGraphicsShapeItem>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QLineF>
#include <QVector>

#include "gribouillotitem.h"
#include "item_arc.h"
#include "item_spiral.h"

class Item_spiralDrawer : public QObject, public QAbstractGraphicsShapeItem
{

    Q_OBJECT

public:
    enum { Type = SPIRAL_DRAWER };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return SPIRAL_DRAWER;
    }

    explicit Item_spiralDrawer(int penWidth, QColor c, QPointF center,
                               QGraphicsItem* parent = nullptr);
    ~Item_spiralDrawer(){}

    QVector<Item_arc *> getArcs();
    Item_spiral* getSpiral();
    QVector<QPointF> getCenters();
    QVector<QLineF> getBase();

signals:
    void endDrawing(QString);

protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) Q_DECL_OVERRIDE;
    void mousePressEvent(QGraphicsSceneMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    QVector<QLineF> side;
    int remainingClicks;
    bool clockwise;

    QString computeSpiralBase();

};

#endif // ITEM_SPIRALDRAWER_H
