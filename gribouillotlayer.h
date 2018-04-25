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
#include <QPushButton>

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
#include "qlinef58.h"

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
    QPushButton* getVisibilityBtt();
    QString newLabel(bool keepOldFile);
    QString askNewFile();

    bool contains(QGraphicsItem* item);
    QList<QGraphicsItem*> selectedItems();
    bool writeXML();
    void loadXML(QString path);

    //Points drawing
    void drawPoint(QColor penColor, int penWidth, QPointF position, bool askWeight);
    void drawPoint(QColor penColor, int penWidth, QPointF position, int weight=0);
    void displayPointWeight(bool isDisplayed);
    Item_point *computeCoM(QColor penColor, int penWidth);

    //Lines drawing
    void drawSegment(QColor penColor, int penWidth, QVector<QPointF> positions);
    void drawSegment(QColor penColor, int penWidth, QLineF segment);
    void drawLineFromSegment(QColor penColor, int penWidth, QLineF line);
    void drawLineFromSegment(QColor penColor, int penWidth, QVector<QPointF> positions);
    void drawHorizontal(QColor penColor, int penWidth, QPointF point);
    void drawVertical(QColor penColor, int penWidth, QPointF point);
    void drawParallel(QColor penColor, int penWidth, QPointF point, QLineF58 selectedLine);
    void drawPerpendicular(QColor penColor, int penWidth, QPointF point, Item_segment *selectedSegment);
    void drawBisection(QColor penColor, int penWidth, QLineF58 selectedLine);

    //Circles drawing
    void drawCircle(QColor penColor, int penWidth, QPointF center, qreal radius);
    QPointF drawCircleFromRadius(QColor penColor, int penWidth, QPointF position,
                                 QLineF58 selectedRadius);
    QPointF drawCircleFromRadius(QColor penColor, int penWidth, QVector<QPointF> positions);
    QPointF drawCircleFromRadius(QColor penColor, int penWidth, QPointF center, qreal scale,
                                 QString scaleUnit);
    QPointF drawCircleFromDiameter(QColor penColor, int penWidth, QLineF58 selectedDiameter);
    QPointF drawCircleFromDiameter(QColor penColor, int penWidth, QVector<QPointF> positions);
    QPointF drawCircleFromTriangle(QColor penColor, int penWidth);
    QPointF drawCircleFromTriangle(QColor penColor, int penWidth, QVector<QPointF> positions);

    //Arcs drawing
    QPointF drawArc(QColor penColor, int penWidth, QPointF center, qreal radius,
                    qreal startAngle, qreal spanAngle);
    QPointF drawArc(QColor penColor, int penWidth, Item_arc* arc);
    QPointF drawArcFromCircle(QColor penColor, int penWidth, Item_circle *sourceCircle, Item_arc* arc);

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
    void toggleVisibility();
    void on_opacitySlider_valueChanged(int value);
    void on_deleteLayerTlBtt_clicked();

private:
    Ui::GribouillotLayer *ui;
    QString label;
    QPushButton* visibilityBtt;
    bool areItemsVisible;
    QList<QGraphicsItem *> itemsList;

    void initLayer();
    void addItemToLayer(QGraphicsItem *item);

};

#endif // GRIBOUILLOTLAYER_H
