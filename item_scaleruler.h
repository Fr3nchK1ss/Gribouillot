/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef ITEM_SCALERULER_H
#define ITEM_SCALERULER_H

#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>


class Item_scaleRuler : public QObject, public QGraphicsLineItem
{
    Q_OBJECT

public:
    explicit Item_scaleRuler(QGraphicsItem * parent = nullptr);
    ~Item_scaleRuler(){}
    void reset();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event) Q_DECL_OVERRIDE;

private:
    QPointF origin;
    int remainingClicks;

signals:
    void newMeasure(qreal);
};

#endif // ITEM_SCALERULER_H
