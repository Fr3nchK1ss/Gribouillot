/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */


#ifndef GRIBOUILLOTLAYER_H
#define GRIBOUILLOTLAYER_H

#include <QGraphicsItem>
#include <QMainWindow>

#include "item_arc.h"
#include "item_arcdrawer.h"
#include "item_circle.h"
#include "item_pixmap.h"
#include "item_point.h"
#include "item_pointonrail.h"
#include "item_line.h"
#include "item_segment.h"
#include "item_spiral.h"
#include "item_spiraldrawer.h"

namespace Ui {
class GribouillotLayer;
}

class GribouillotLayer : public QWidget
{
    Q_OBJECT

static int layer_index;

public:
    explicit GribouillotLayer(QMainWindow *parent = nullptr);
    explicit GribouillotLayer(QString path, QMainWindow *parent);
    ~GribouillotLayer();

    static void resetLayerIndex(){layer_index = 0;}
    QString getLabel();
    QString newLabel(bool keepOldFile);
    QString askNewFile();

    void enableItems(QList<GribouillotItem> onlyTypes = QList<GribouillotItem>());
    bool contains(QGraphicsItem* item);
    QList<QGraphicsItem*> selectedItems();
    bool writeXML();
    void loadXML(QString path);

    //Utility for drawing
    QLineF58 getSelectedLineF();

    //Points drawing
    void drawPoint(QColor penColor, int penWidth, QPointF position, bool askWeight);
    void drawPoint(QColor penColor, int penWidth, QPointF position, int weight=0);
    void displayPointWeight(bool isDisplayed);
    Item_point *computeCoM(QColor penColor, int penWidth);

    //Lines drawing
    void drawSegment(QColor penColor, int penWidth, QPointF points[]);
    void drawSegment(QColor penColor, int penWidth, QVector<QPointF> positions);
    void drawSegment(QColor penColor, int penWidth, QLineF segment);
    void drawLine(QColor penColor, int penWidth, QPointF points[]);
    void drawLineFromSegment(QColor penColor, int penWidth, QLineF line);
    void drawLineFromSegment(QColor penColor, int penWidth, QVector<QPointF> positions);
    void drawHorizontal(QColor penColor, int penWidth, QPointF point);
    void drawVertical(QColor penColor, int penWidth, QPointF point);
    void drawParallel(QColor penColor, int penWidth, QPointF point);
    void drawPerpendicular(QColor penColor, int penWidth, QPointF point);
    void drawBisection(QColor penColor, int penWidth);

    //Circles drawing
    void drawCircle(QColor penColor, int penWidth, QPointF center, qreal radius);
    QPointF drawCircleFromRadius(QColor penColor, int penWidth, QPointF position);
    QPointF drawCircleFromRadius(QColor penColor, int penWidth, QVector<QPointF> positions);
    QPointF drawCircleFromRadius(QColor penColor, int penWidth, QPointF center, qreal scale);
    QPointF drawCircleFromDiameter(QColor penColor, int penWidth);
    QPointF drawCircleFromDiameter(QColor penColor, int penWidth, QVector<QPointF> positions);
    QPointF drawCircleFromTriangle(QColor penColor, int penWidth);
    QPointF drawCircleFromTriangle(QColor penColor, int penWidth, QVector<QPointF> positions);

    //Arcs drawing
    QPointF drawArc(QColor penColor, int penWidth, QPointF center, qreal radius,
                    qreal startAngle, qreal spanAngle);
    QPointF drawArc(QColor penColor, int penWidth, Item_arc* arc);
    QPointF drawArcFromCircle(QColor penColor, int penWidth, Item_circle *sourceCircle,
                           Item_arc* arc);

    //Spiral drawing
    void drawSpiral(QColor penColor, int penWidth, QVector<QPointF> centers);

    //Pixmap drawing
    void drawPixmap(bool doCopy);

public slots:
    int deleteSelectedItems();
    void selectNextItem();

signals:
    void newLayerName(QString label);
    void deleteLayer();
    void addItemToScene(QGraphicsItem*);

private slots:
    void on_layerNameTlBtt_clicked();
    void on_visibilityCheckBox_stateChanged(int arg1);
    void on_opacitySlider_valueChanged(int value);
    void on_deleteLayerTlBtt_clicked();

private:
    Ui::GribouillotLayer *ui;
    QString label;
    QList<QGraphicsItem *> itemsList;

    void initLayer();
    void addItemToLayer(QGraphicsItem *item);
    void addPointToLayer(QGraphicsItem *item);

};

#endif // GRIBOUILLOTLAYER_H
