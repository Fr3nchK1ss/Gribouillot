/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef MINIMAP_H
#define MINIMAP_H

#include <QGraphicsScene>
#include <QMouseEvent>
#include <QPointF>

#include "zoomablegraphicsview.h"

class Minimap : public QGraphicsView
{
    Q_OBJECT

public:
    explicit Minimap(QGraphicsScene* scene, ZoomableGraphicsView *mainView);
    ~Minimap();

protected:
    void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

signals:
    void centerMap(QPointF);

};

#endif // MINIMAP_H
