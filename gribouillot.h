/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef GRIBOUILLOT_H
#define GRIBOUILLOT_H

#include <QActionGroup>
#include <QMainWindow>
#include <QColor>
#include <QGraphicsView>
#include <QSpinBox>

#include "dlg_setupgps.h"
#include "dlg_spiral.h"
#include "gribouillotlayer.h"
#include "gribouillotscene.h"
#include "item_scaleruler.h"
#include "minimap.h"


namespace Ui {
class Gribouillot;
}

class Gribouillot : public QMainWindow
{
    Q_OBJECT

public:

    explicit Gribouillot(QWidget *parent = nullptr);
    ~Gribouillot();

    void openProject(QString gribFile);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    //GUI pointers
    Ui::Gribouillot *ui;
    GribouillotScene* scene;
    Minimap *minimap;

    //Project variables
    QString currentProjectName;
    QString mapTabName;
    QString mapPath;
    QGraphicsPixmapItem *backgroundMap;

    //Tabpage variables
    GribouillotLayer *currentLayer;
    int currentTabIndex;
    int callsForAChange;    

    //GPS variables
    Dlg_setupGps* gpsDialog;
    bool gpsEnabled;

    //Spiral options
    Dlg_spiral* spiralDialog;

    //drawing variables
    QActionGroup* drawingGroup;
    QCursor drawingCursor;
    QColor drawingColor;
    int drawingWidth;

    GribouillotItem currentDrawing;
    QVector<QString> drawingTips;
    QVector<QPointF> drawingCoords;

    /**private functions */
    //UI functions
    void createPlusTab();
    void restrictToolbar();
    void fullToolbar();
    void enableItemsAndSpecifics(GribouillotLayer* layer);
    void setColorIcon(QColor color);
    void clearView();
    void setDrawingView();
    void setSelectionView();
    void uncheckDrawingGroup();
    void resetUI();
    void initGpsDialog();

    //Project functions
    void maybeSave();
    void initProject();
    void loadBackgroundMap(QString path);
    void addNewLayer(QString path = "");
    void saveLayersOrder();

    //scaleRuler
    Item_scaleRuler *scaleRuler;
    void initScaleRuler();

private slots:
    //Menu actions
    void on_actionNewProject_triggered();
    void on_actionOpenProject_triggered();
    void on_actionImport_layer_triggered();
    void saveProject();
    void on_actionExportAs_triggered();
    void on_actionQuit_triggered();
    void on_actionMinimap_toggled(bool isChecked);
    void on_actionCenterOfMass_triggered();
    void on_actionSpiralOptions_triggered();
    void on_actionAbout_Grib_triggered();

    //MapTabWidget actions
    void mapTabNameTlBttClicked();
    void blackWhiteTlBttClicked(bool isChecked);

    void gpsTlBttClicked();
    void disableGps();
    void acceptGpsDialog();
    void hideGpsDialog();

    void scaleRulerTlBttToggled(bool checked);

    //React to change in tabs selection
    void tabWidgetClicked(int newTabIndex);
    void tabPageChanged(int newTabIndex);

    //React to signals from the current layer
    void doChangeLayerName(QString label);
    void doDeleteLayer();
    void doAddItemToScene(QGraphicsItem*item);


/*************** In gribouillot_toolbar.cpp **************/
    //React to scene events
    void newMoveOnScene(QPointF position);
    void newSceneClickPreSelect(QPointF position);
    void newSceneClickPostSelect(QPointF position);
    bool moreCoordsNeeded(QPointF position);
    bool moreDrawingTips();
    bool isOnlySelected(QVector<GribouillotItem> types,
                         int targetCount = -1);
    QPointF adjustClickToPoint(QPointF pos);

    void keyDeleteFromScene();
    void keySpaceFromScene();
    void keyTFromScene();

    void sceneSelectionChanged() const;

    void visualHelp_arcFromCircle();
    void visualHelp_pointOnRail();
    void visualHelp_lineFromAngle(Item_pointOnRail* pOR);

    //Toolbars actions (linked to scene events)
    void on_actionCursorDrag_triggered();
    void on_actionCursorSelect_triggered();
    void on_actionChooseColor_triggered();
    void on_actionChooseWidth_triggered();

    void on_actionMeasureDistance_toggled(bool isChecked);
    void measureDistance(qreal distance);

    void on_actionPointWeight_toggled(bool isChecked);
    void on_actionPoint_triggered();

    void on_actionLine_triggered();
    void on_actionSegment_triggered();
    void on_actionParallel_triggered();
    void on_actionPerpendicular_triggered();
    void on_actionBisection_triggered();
    void on_actionHorizontalLine_triggered();
    void on_actionVerticalLine_triggered();
    void on_actionAngleLine_triggered();

    void on_actionCircleCenterPoint_triggered();
    void on_actionCircleCenterRadiusValue_triggered();
    void on_actionCircleRadius_triggered();
    void on_actionCirclePointsDiameter_triggered();
    void on_actionCircleDiameter_triggered();
    void on_actionCircleTriangle_triggered();

    void on_actionArcFromCircle_triggered();
    void on_actionArc_triggered();

    void on_actionSpiral_triggered();
    void finalizeSpiral(QString errorMsg);

    void on_actionLoadPicture_triggered();
};

#endif // GRIBOUILLOT_H
