/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QGraphicsItem>
#include <QDebug>
#include <QtMath>
#include <QWheelEvent>

#include "zoomablegraphicsview.h"


/**
 * We must reimplement both constructors of QGraphicsView(), because those constructors
 * may be called by the UI interface.
 */
ZoomableGraphicsView::ZoomableGraphicsView(QWidget * parent) : QGraphicsView(parent)
{
    scaleBar = new ScaleBar(this);
    currentZoom = 1;

    //see also the Designer for other UI properties of the GraphicsView.

}


ZoomableGraphicsView::~ZoomableGraphicsView()
{
    delete scaleBar;
}


qreal ZoomableGraphicsView::getZoom()
{
    return currentZoom;
}

/********************* SLOTS *********************/
/**
 * @brief   notify the red&white scaleBar of a scale change
 */
void ZoomableGraphicsView::systemScaleChanged(qreal systemScale, QString unit)
{
    scaleBar->scaleChanged(systemScale, unit, currentZoom);
}


/**
 * @brief   center the View according to Minimap double-click
 */
void ZoomableGraphicsView::slotCenterOn(QPointF position)
{
    centerOn(position);
}

/********************* Protected *********************/

void ZoomableGraphicsView::resizeEvent(QResizeEvent *e)
{
    /**
     * Move the scaleBar so:
     * - it doesn't disappear if the containing widget of the graphicsView is reduced.
     * - it doesn't appear in the middle of the view, but still in the lower left corner,
     *   if the original application window is made bigger for example.
     */
    scaleBar->move(25, height()-45);

    QGraphicsView::resizeEvent(e);
}


/**
 * @brief   Handles mouse wheel events concerning the graphicsView.
 * @details Implements zooming in or out upon holding CTRL + mouse wheel
 *          Also changes the zoom level of the red & white scalebar.
 */
#ifndef QT_NO_WHEELEVENT
void ZoomableGraphicsView::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier)
    {
        /**
         * Warning: Combining numerous successive zoom In and Out can yield to
         * side-effects due to the floating point operations involved, particularly
         * for the scaleBar. To prevent this zoomOut has high floating precision.
         */

        auto delta = e->angleDelta().y();
        double zoomFactor;
        if (delta < 0)
            zoomFactor = zoomOutFactor;
        else
            zoomFactor = zoomInFactor;

        scale(zoomFactor, zoomFactor);
        currentZoom*=zoomFactor;//qDebug() << "zoom: " << currentZoom;

        //Tell scalebar of the zoom change.
        scaleBar->zoomChanged(zoomFactor, currentZoom);

        e->accept();

    }
    else
        QGraphicsView::wheelEvent(e);

}
#endif



