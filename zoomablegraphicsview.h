/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef ZOOMABLEGRAPHICSVIEW_H
#define ZOOMABLEGRAPHICSVIEW_H

#include <QFrame>
#include <QGraphicsView>
#include <QEvent>

#include "scalebar.h"

/**
 * @brief Subclass QGraphicsView to implement wheelEvent
 */
class ZoomableGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ZoomableGraphicsView(QWidget *parent = 0);
    ~ZoomableGraphicsView();

    qreal getZoom();

public slots:
    void systemScaleChanged(qreal systemScale, QString unit);
    void slotCenterOn(QPointF position);

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE;
#endif

private:
    ScaleBar *scaleBar;
    qreal currentZoom;

    const double zoomOutFactor = 0.83333333;
    const double zoomInFactor = 1.2;

};

#endif // ZOOMABLEGRAPHICSVIEW_H
