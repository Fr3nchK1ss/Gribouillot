/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include "scalebar.h"

#include <QPaintEvent>
#include <QPainter>
#include <QRectF>
#include <QPoint>
#include <QtDebug>

ScaleBar::ScaleBar(QWidget *parent) : QWidget(parent),
        barHeight(10),
        barWidth(250),
        lengthValue(0),
        lengthUnit(""),
        zoomLevel(1)
{
    /**
     * This geometry includes a starting position within the GraphicsView (30, 370) based on
     * the starting size of the Application Window. However this position can be modified if
     * the containing widget of the GraphicsView is resized (see zoomableGraphicsView).
     * +10, + 15 <=> some room for the scale legend at the bottom and on the right.
     */
    setGeometry(25, 370, barWidth+45, barHeight+15);
}


void ScaleBar::paintEvent( QPaintEvent *e )
{
    QPainter p(this);
    QFont f;
    QColor c;
    qreal hStep;
    qreal vStep;
    QString barLength;
    QString stepLength;

    f.setPixelSize(10);
    p.setFont(f);

    hStep = barWidth /4.0;
    vStep = barHeight /2.0;

    //Paint red and white rectangles
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
            if (c == Qt::red)
                c = Qt::white;
            else
                c = Qt::red;

            p.fillRect(j*hStep, i*vStep, hStep, vStep, c);
        }
        c = Qt::red;
    }
    //Add a black border around the bar
    p.setPen(Qt::black);
    p.drawRect(QRect(0, 0, barWidth-1, barHeight-1));

    //Compute the length of the bar at +/-5km, length of step at +/- 1km
    if ( lengthUnit == "km" ) {
        barLength = QString::number(qRound(lengthValue/10)*10)+"km";
        stepLength = QString::number(qRound(lengthValue/4))+"km";
    }
    else
    {   // lengthUnit == "m"
        barLength = QString::number(qRound(lengthValue/10)*10)+"m";
        stepLength = QString::number(qRound(lengthValue/4))+"m";
    }

    //Display the stepLength on the left and the barLength on the right.
    p.setPen(Qt::red);    
    p.drawText(rect(), Qt::AlignBottom | Qt::AlignLeft, "       "+stepLength);
    p.drawText(QRect(0, 0, barWidth, height()),
               Qt::AlignBottom | Qt::AlignRight, barLength+" |");

    //Cosmetic bars...
    p.drawLine(0, barHeight+2, 0, height());
    p.drawLine(barWidth/4-1, barHeight+2, barWidth/4-1, height()+2);

    //zoomLevel information
    f.setPixelSize(15);
    f.setBold(true);
    p.setFont(f);
    p.drawText(barWidth+2, 15, "x "+QString::number(zoomLevel));

    e->accept();
}



/**
 * \bref:       The map scale has changed, update the scale bar.
 * @details:    lengthValue depends on the km/px scale and the zoom ratio
 *              of the zoomableGraphicsView.
 */
void ScaleBar::scaleChanged(qreal newScale, QString unit, qreal graphicsViewZoom)
{
    lengthUnit = unit;
    /**
     * Compute the length of the red&white bar in km. Adjust to the zoom factor
     * 6of the graphics view, which implies to multiply our non-rounded result
     * by (1/graphicsViewZoom)
     */
    if (unit == "km")
        lengthValue = width()*newScale/(graphicsViewZoom);
    else //unit == "m"
        lengthValue = width()*newScale*1000/(graphicsViewZoom);

    update();
}


/**
 * \bref    The GraphicsView zoom level has changed, update scale bar accordingly.
 * @details The zoom parameter is only passed as a visual information to the user.
 */
void ScaleBar::zoomChanged(qreal factor, qreal zoom)
{
    lengthValue /= factor;
    //zoomLevel will be displayed rounded to one decimal.
    zoomLevel = ((qreal)qRound(zoom*10))/10;

    //Repaint scalebar
    update();
}

