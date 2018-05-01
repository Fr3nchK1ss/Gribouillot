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
#include <QTimer>

#include "dlg_setupgps.h"
#include "dlg_spiral.h"
#include "gribouillotlayer.h"
#include "gribouillotscene.h"
#include "item_scaleruler.h"
#include "minimap.h"
#include "qlinef58.h"


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

public slots:
    //React to signals from the current layer
    void doChangeLayerName(QString label);
    void doDeleteLayer();
    void doAddItemToScene(QGraphicsItem*item);

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    //GUI pointers
    Ui::Gribouillot *ui;
    int restrictedActions;
    GribouillotScene* scene;
    Minimap *minimap;

    //Autosave in mins
    QTimer *timer;
    int autosaveTimeout = 0;

    //Project variables
    QString currentProjectName;
    QString mapName;
    QString mapPath;
    QGraphicsPixmapItem *backgroundMap;

    //Layers variables
    GribouillotLayer *currentLayer;
    int currentTabIndex;

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
    void restrictToolbar();
    void fullToolbar();
    void readSettings();
    void writeSettings();
    void addNewLayer(QString path = "");
    void setWorkingLayer(GribouillotLayer* layer);
    void clearView();
    void setDrawingView();
    void setSelectionView();
    void uncheckDrawingGroup();
    void resetUi();
    void initGpsDialog();
    void setColorIcon(QColor color);

    //Project functions
    void initProject();
    void loadBackgroundMap(QString path);
    void maybeSave();
    void saveLayersOrder();

private slots:
    //Menu actions
    void on_actionNewProject_triggered();
    void on_actionOpenProject_triggered();
    void on_actionImport_layer_triggered();
    int saveProject();
    void autoSaveProject();
    void newSaveTimeout();
    void on_actionExportAs_triggered();
    void on_actionQuit_triggered();
    void on_actionMinimap_toggled(bool isChecked);
    void on_actionCenterOfMass_triggered();
    void on_actionSpiralOptions_triggered();
    void on_actionAutosave_triggered();
    void on_actionAbout_Grib_triggered();

    //MapTabWidget actions
    void mapNameTlBttClicked();
    void blackWhiteTlBttClicked(bool isChecked);

    void gpsTlBttClicked();
    void disableGps();
    void acceptGpsDialog();
    void hideGpsDialog();

    void scaleRulerTlBttTriggered();

    //React to change in tabs selection
    void tabWidgetClicked(int newTabIndex);
    void switchToTabIndex(int newTabIndex);


/*************** In module gribouillot_toolbar.cpp **************/
private:
    bool moreCoordsNeeded(QPointF position);
    bool moreDrawingTips();

    QLineF58 getSelectedLineF();

    void visualHelp_arcFromCircle();
    void visualHelp_pointOnRail();
    void visualHelp_lineFromAngle(Item_pointOnRail* pOR);

    void userCanNotDo(QString operation);

private slots:
    //React to scene events
    void newMoveOnScene(QPointF position);
    void newSceneClickPreSelect(QPointF position);
    void newSceneClickPostSelect(QPointF position);

    void keyDeleteFromScene();
    void keySpaceFromScene();
    void keyTFromScene();

    void sceneSelectionChanged() const;

    //Toolbars actions (linked to scene events)
    void on_actionCursorDrag_triggered();
    void on_actionCursorSelect_triggered();
    void on_actionChooseColor_triggered();
    void on_actionChooseWidth_triggered();

    void on_actionMeasureDistance_triggered();
    void measureDistance(qreal distance);

    void on_actionMeasureAngle_triggered();
    void measureAngle(qreal angle);

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
    void on_actionCircleRadiusValue_triggered();
    void on_actionCircleSelectRadius_triggered();
    void on_actionCircleDiameter_triggered();
    void on_actionCircleSelectDiameter_triggered();
    void on_actionCircleTriangle_triggered();

    void on_actionArcFromCircle_triggered();
    void on_actionArc_triggered();

    void on_actionSpiral_triggered();
    void finalizeSpiral(QString errorMsg);

    void on_actionLoadPicture_triggered();

};

#endif // GRIBOUILLOT_H
