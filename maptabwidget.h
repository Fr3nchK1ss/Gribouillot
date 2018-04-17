/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef MAPTABWIDGET_H
#define MAPTABWIDGET_H

#include <QAction>
#include <QString>
#include <QWidget>

#include "gribouillot.h"
#include "dlg_setupgps.h"

namespace Ui {
class MapTabWidget;
}

/**
 * @brief   MapTabWidget is the UI element which manages the background map
 * @details MapTabWidget should be designed in the Gribouillot::ui, but it is not possible with
 *          the current limitations of Qt-Designer. So MapTabWidget is the "promoted" QWidget of the
 *          Gribouillot::ui->gribTabWidget. Then Gribouillot is made "friendly" class to access the
 *          MapTabWidget::ui as an integral part of its interface.
 */
class MapTabWidget : public QWidget
{
    Q_OBJECT
    //Gribouillot can access the private ui because mapTab is an integral part of the mainWindow
    friend class Gribouillot;

public:
    explicit MapTabWidget(QWidget *parent = nullptr);
    ~MapTabWidget();
    qreal getKmPxScale();

public slots:
    void newScaleMeasure(double);
    void uncheckDrawingButtons();

signals:
    void newSystemScale(qreal, QString);

private:
    Ui::MapTabWidget *ui;
    qreal kmPxScale;

    void resetUI();
    void computeKmPxScale(double km, double px);

private slots:
    void newPxValue(double);
    void newKmValue(double);
    void newScaleUnit();
};

#endif // MAPTABWIDGET_H
