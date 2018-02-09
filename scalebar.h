/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef SCALEBAR_H
#define SCALEBAR_H

#include <QWidget>
#include <QString>

class ScaleBar : public QWidget
{
    Q_OBJECT
public:
    explicit ScaleBar(QWidget *parent);
    ~ScaleBar(){}

    void scaleChanged(qreal newScale, QString unit, qreal graphicsViewZoom);
    void zoomChanged(qreal factor, qreal zoom);

protected:
    void paintEvent( QPaintEvent *e );

private:
    int barHeight;
    int barWidth;
    qreal lengthValue;
    QString lengthUnit;
    qreal zoomLevel;

};

#endif // SCALEBAR_H
