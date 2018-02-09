/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef ITEM_PIXMAP_H
#define ITEM_PIXMAP_H

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QString>
#include <QXmlStreamWriter>

#include "gribouillotitem.h"


class Item_pixmap : public QGraphicsPixmapItem
{
public:
    enum { Type = PIXMAP };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return PIXMAP;
    }
    explicit Item_pixmap(QString path, QPointF position = QPointF(), qreal scale = 1);
    ~Item_pixmap(){}

    void setPath(QString path);
    void serialize2xml(QXmlStreamWriter* w);
    QString status();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QGraphicsSceneWheelEvent * event) Q_DECL_OVERRIDE;

private:
    QString imagePath;
    qreal rotationAngle = 0;
    qreal plusFactor = 1.0204;
    qreal minusFactor = 0.98;

};

#endif // ITEM_PIXMAP_H
