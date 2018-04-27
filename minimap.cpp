/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include "minimap.h"

#include <QDebug>

Minimap::Minimap(QGraphicsScene *scene, ZoomableGraphicsView *mainView) :
    QGraphicsView(mainView)
{
    setScene(scene);

    setMinimumSize(150, 150);
    setMaximumSize(250, 250);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setCursor(Qt::CrossCursor);
    setToolTip(tr("Double-click to jump to a new position."));
    show();

    connect(this, &Minimap::centerMap, mainView, &ZoomableGraphicsView::slotCenterOn);
}

Minimap::~Minimap()
{
    disconnect();
}


void Minimap::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit centerMap(mapToScene(event->pos()));
}

/** Reimplement protected functions to ignore all other user actions*/
void Minimap::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
}

void Minimap::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

void Minimap::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
}



