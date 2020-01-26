/**
 *  @brief     The QMainWindow of Gribouillot
 *  @details   Implements the GraphicsView which receives the drawing events,
 *             a TabWidget for layers pages,
 *             a DockWidget for the drawing tools
 *  @author    Ludovic A. 
 *  @date      2015/2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <thread>
//#include <ctime>

#include <QtMath>
#include <QtWidgets>
#include <QtDebug>

#include "ui_gribouillot.h"
#include "ui_maptabwidget.h"

#include "dlg_autosave.h"
#include "dlg_newgribproject.h"
#include "dlg_importlayer.h"
#include "dlg_changemap.h"
#include "gribouillot.h"

#include "main.h"
#include "zoomablegraphicsview.h"


//declaring global functions defined in main.c
extern QString getSupportedImageFormats();
extern QString getDefaultImageFilter(QString);


Gribouillot::Gribouillot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Gribouillot),
    scene(new GribouillotScene),
    minimap(nullptr),
    timer(nullptr),
    currentProjectName(""),
    mapName(""),
    backgroundMap(new QGraphicsPixmapItem),

    currentTabIndex(0),

    drawingColor(Qt::black),
    drawingWidth(5),
    currentDrawing(NONE)

{
    ui->setupUi(this);
    //The spiral dialog is an extension of the interface which is shown/hidden if necessary
    spiralDialog = new Dlg_spiral(this);

    //Menu connections
    connect (ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);

    //TabWidget
    connect (ui->gribTabWidget, &SmartInsertTabWidget::currentChanged, this, &Gribouillot::switchToTabIndex);

    //mapTabWidget is an extension of the mainWindow and even a "friendly" Class.
    connect (ui->mapTabWidget->ui->mapTabNameTlBtt, &QToolButton::clicked, this, &Gribouillot::mapNameTlBttClicked);
    connect (ui->mapTabWidget->ui->blackWhiteTlBtt, &QToolButton::toggled, this, &Gribouillot::blackWhiteTlBttClicked);
    connect (ui->mapTabWidget->ui->scaleRulerTlBtt, &QToolButton::clicked, this, &Gribouillot::scaleRulerTlBttTriggered);
    connect (ui->mapTabWidget->ui->gpsTlBtt, &QToolButton::clicked, this, &Gribouillot::gpsTlBttClicked);

    //graphicsView and its drawing cursor
    ui->zGraphicsView->setScene(scene);
    drawingCursor = QCursor(QPixmap(":/Resources/Icons/cursor-drawing.png"));

    //scene
    connect (scene, &GribouillotScene::selectionChanged, this, &Gribouillot::sceneSelectionChanged);
    connect (scene, &GribouillotScene::newMouseClickPreSelect, this, &Gribouillot::newSceneClickPreSelect);
    connect (scene, &GribouillotScene::newMouseClickPostSelect, this, &Gribouillot::newSceneClickPostSelect);
    connect (scene, &GribouillotScene::newMouseMove, this, &Gribouillot::newMoveOnScene);
    connect (scene, &GribouillotScene::keyDeletePressed, this, &Gribouillot::keyDeleteFromScene);
    connect (scene, &GribouillotScene::keySpacePressed, this, &Gribouillot::keySpaceFromScene);
    connect (scene, &GribouillotScene::keyCPressed, this, &Gribouillot::on_actionChooseColor_triggered);
    connect (scene, &GribouillotScene::keyTPressed, this, &Gribouillot::keyTFromScene);
    connect (scene, &GribouillotScene::keyEscPressed, this, &Gribouillot::keyEscFromScene);

    //Connect mapTabWidget to zGraphicsView in order to update the black & white scale bar
    connect(ui->mapTabWidget, &MapTabWidget::newSystemScale, ui->zGraphicsView, &ZoomableGraphicsView::systemScaleChanged);

    drawingGroup = new QActionGroup(this);

    //Toolbar actions which are not affected by restrictToolbar()
    drawingGroup->addAction(ui->actionCursorDrag);
    drawingGroup->addAction(ui->actionCursorSelect);
    drawingGroup->addAction(ui->actionMeasureDistance);
    drawingGroup->addAction(ui->actionMeasureAngle);

    restrictedActions = drawingGroup->actions().count();//Do not move this line!

    //Toolbar actions affected by restrictToolbar()
    drawingGroup->addAction(ui->actionPoint);

    drawingGroup->addAction(ui->actionSegment);
    drawingGroup->addAction(ui->actionLine);
    drawingGroup->addAction(ui->actionHorizontalLine);
    drawingGroup->addAction(ui->actionVerticalLine);
    drawingGroup->addAction(ui->actionParallel);
    drawingGroup->addAction(ui->actionPerpendicular);
    drawingGroup->addAction(ui->actionBisection);
    drawingGroup->addAction(ui->actionAngleLine);

    drawingGroup->addAction(ui->actionCircleCenterPoint);
    drawingGroup->addAction(ui->actionCircleSelectRadius);
    drawingGroup->addAction(ui->actionCircleRadiusValue);
    drawingGroup->addAction(ui->actionCircleDiameter);
    drawingGroup->addAction(ui->actionCircleSelectDiameter);
    drawingGroup->addAction(ui->actionCircleTriangle);

    drawingGroup->addAction(ui->actionArcFromCircle);
    drawingGroup->addAction(ui->actionArc);

    drawingGroup->addAction(ui->actionSpiral);

    drawingGroup->addAction(ui->actionLoadPicture);

    //connect because mapTabWidget toolButtons can not be a part of the QActionGroup above
    connect(drawingGroup, &QActionGroup::triggered, ui->mapTabWidget, &MapTabWidget::uncheckDrawingButtons);

    //Disable everything before a first project is created
    ui->toolBar->setEnabled(false);
    ui->centralwidget->setEnabled(false);


    //TODO: implement GPS
    ui->mapTabWidget->ui->gpsTlBtt->setVisible(false);

    show();

    //load global settings
    readSettings();


}

Gribouillot::~Gribouillot()
{
    delete ui;
}




/************************* Protected functions ****************************/
/**
 * @brief   catch the MainWindow close event
 */
void Gribouillot::closeEvent(QCloseEvent *event)
{
    /*
     * Saving project must be done first, because of the 'recentProject'
     * key used in writeSettings.
     */
    if(!currentProjectName.isEmpty())
        maybeSave();//maybe save current project

    //Automatically save general settings (window position, etc)
    writeSettings();

    event->accept();

}



/************************* Private functions - UI ****************************/
/**
 * @brief   Do not allow the use of layer-specific actions.
 * @details Hard coded limit depending on the ui design. See gribouillot.ui
 */
void Gribouillot::restrictToolbar()
{
    QList<QAction*> actionsList = ui->toolBar->actions();
    for (int i=restrictedActions; i < actionsList.size(); ++i)
    {
        actionsList.at(i)->setEnabled(false);
    }

    //currentLayer has no sense since no layer is selected
    currentLayer = nullptr;
    //Default to select cursor
    setSelectionView();

}

/**
 * @brief   Enable full toolBar
 */
void Gribouillot::fullToolbar()
{
    foreach(QAction *action, ui->toolBar->actions())
    {
        action->setEnabled(true);
        //qDebug() << action;
    }
}


/**
 * @brief Typical readSettings function for global parameters
 */
void Gribouillot::readSettings()
{
    QSettings settings("GAT", "Gribouillot");

    settings.beginGroup("MainWindow");
        resize(settings.value("size", QSize(970, 600)).toSize());
        move(settings.value("pos", QPoint(100, 100)).toPoint());
    settings.endGroup();


    //Restore spiral dialog settings
    settings.beginGroup("spiral");
    spiralDialog->loadData( settings.value("constructAsOneItem", true).toBool(),
                            settings.value("baseDisplay", false).toBool(),
                            settings.value("showBaseCentersOnly", true).toBool());
    settings.endGroup();

    settings.beginGroup("preferences");
        ui->actionCopy_pixmaps->setChecked( settings.value("copyPixmaps", true).toBool() );
        autosaveTimeout = settings.value("autosave", 0).toInt();
        newSaveTimeout();
    settings.endGroup();

    if (settings.contains("recentProject"))
        openProject(settings.value("recentProject").toString());

    //Restore minimap after openProject(), when the background map is loaded.
    ui->actionMinimap->setChecked(settings.value("preferences/minimap").toBool());

}


/**
 * @brief Typical writeSettings function
 */
void Gribouillot::writeSettings()
{
    QSettings settings("GAT", "Gribouillot");

    settings.beginGroup("MainWindow");
        settings.setValue("size", size());
        settings.setValue("pos", pos());
    settings.endGroup();

    settings.beginGroup("spiral");
        settings.setValue("constructAsOneItem", spiralDialog->isOneItem());
        settings.setValue("baseDisplay", spiralDialog->isBaseDisplayed());
        settings.setValue("showBaseCentersOnly", spiralDialog->showBaseCentersOnly());
    settings.endGroup();

    settings.beginGroup("preferences");
        settings.setValue("minimap", ui->actionMinimap->isChecked());
        settings.setValue("copyPixmaps", ui->actionCopy_pixmaps->isChecked());
        settings.setValue("autosave", autosaveTimeout);
    settings.endGroup();


    QString projectFile = QDir::currentPath()+"/"+currentProjectName+".grib";
    /*
     * The current project will be the 'recent project' of the next session,
     * except if the User didn't want to save it for some reason. So we verify
     * if the current project was actually written on disk during maybeSave().
     */
    if ( QFileInfo(projectFile).exists() )
        settings.setValue("recentProject", projectFile);
    //else
        //settings.value() remains the last project saved on disk, or an empty string.

}


/**
 * @brief   Create or load a layer (load if a path is given).
 * @details Ideally, addNewLayer should only call the GribouillotLayer constructor. However
 *          the new layer interface needs to be integrated in the mainWindow interface when
 *          it comes to tab insertion, deletion or labelling.
 */
void Gribouillot::addNewLayer(QString path)
{
    GribouillotLayer *newLayer;

    if (path.isEmpty())
    {
        //A new layer is created with the +tab.
        newLayer = new GribouillotLayer(this);
    }
    else
    {
        //A new layer is loaded from an external file.
        newLayer = new GribouillotLayer(path, this);
    }

    //Insert the new layer after 'currentLayer' and make it the visible layer
    currentTabIndex = ui->gribTabWidget->insertAndDisplayTab(currentTabIndex+1,
                                                            newLayer,
                                                            newLayer->getLabel());
    currentLayer = newLayer;
    setWorkingLayer(currentLayer);

}


/**
 * @brief   Items from the working layer are fully activated.
 * @details Items from other layers can only be selected as reference
 *          items for drawing tools, but can't be deleted, moved, etc.
 */
void Gribouillot::setWorkingLayer(GribouillotLayer* layer)
{
    if( layer != nullptr )
    {
        bool belongsToLayer;

        foreach(QGraphicsItem* item, scene->items())
        {
            belongsToLayer = layer->contains(item) ? true : false;

            //enable item
            item->setEnabled(true);

            //enable or not specifics of this item
            if( item->type() == PIXMAP )
                item->setFlag(QGraphicsItem::ItemIsMovable, belongsToLayer);
            if ( item->type() == SPIRAL)
                item->setEnabled(belongsToLayer);
            //to complete if necessary
        }

        //As a working layer is defined, allow full toolBar
        fullToolbar();
    }

}


/**
 * @brief Clear the GraphicsView from drawing helps
 */
void Gribouillot::clearView()
{
    if(currentDrawing != NONE)
    {
        //Clear any temporary graphic item: drawing help, etc
        foreach(QGraphicsItem* item, scene->items())
        {
            if(item->type() == ARC_DRAWER
               || item->type() == POINT_ONRAIL
               || item->type() == SPIRAL_DRAWER
               || item->type() == SCALERULER)
            {
                //qDebug() << "clear: " << item;
                scene->removeItem(item);
                delete item;
            }
        }

        //Also reset vector used to store drawing positions
        drawingCoords.clear();
    }

}


/**
 * @brief   Set a specific T-cursor and a noDrag view for drawing on scene
 * @details Active when drawing actions are selected. By definition
 *          drawingView is exclusive of cursorDrag and cursorSelect modes.
 */
void Gribouillot::setDrawingView()
{
    clearView();

    setCursor(Qt::ArrowCursor);//Main window cursor

    //Hack because of bug using weighted points proxy widgets
    ui->zGraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);//hack
    ui->zGraphicsView->setCursor(drawingCursor);//GraphicsView cursor
    ui->zGraphicsView->setDragMode(QGraphicsView::NoDrag);
}


/**
 * @brief   Set a generic Arrow cursor for item selection, etc.
 * @details By definition selectionView is exclusive of drawingView
 */
void Gribouillot::setSelectionView()
{
    /*
     * It is not possible to manually check an action of the drawingGroup
     * (see QActionGroup doc), EXCEPT when not one action is checked yet;
     * which is the case here thanks to uncheckDrawingGroup().
     */
    uncheckDrawingGroup();
    ui->actionCursorSelect->setChecked(true);//works fine
    //Checking is not triggering: manually trigger the corresponding SLOT
    on_actionCursorSelect_triggered();
}


/**
 * @brief   Uncheck any toolbar action.
 * @details Useful when using mapTabWidget toolButtons which can not be
 *          a part of the exclusive 'drawingGroup' QActionGroup.
 */
void Gribouillot::uncheckDrawingGroup()
{

    if (drawingGroup->checkedAction() != 0)
    {
        /**
         *  An 'exclusive' actionGroup does not allow for manual checking/
         *  unchecking, so we need to setExclusive(false) first.
         */
        drawingGroup->setExclusive(false);
        drawingGroup->checkedAction()->setChecked(false);
        drawingGroup->setExclusive(true);
    }
}


/**
 * @brief   Reset the user interface when changing project
 * @details Does not reset color wheel
 */
void Gribouillot::resetUi()
{
    //reset MapTabWidget
    ui->mapTabWidget->resetUi();
    ui->gribTabWidget->reset();

    GribouillotLayer::resetLayerIndex();

    //reset scene and delete all items including backgroundMap
    scene->clear();
    scene->setSceneRect(0, 0, 0, 0);

    //new empty background map
    backgroundMap = new QGraphicsPixmapItem();
    mapName = "";

    //reset GPS dialog
    delete gpsDialog; gpsDialog = nullptr;
    gpsEnabled = false;

}


/**
 * @brief   Create a new Gps Dialog each time a new project is open/created
 * @details gpsDialog is an extension of the mainWindow in the same way than mapTabWidget is.
 */
void Gribouillot::initGpsDialog()
{
    gpsEnabled = false;
    gpsDialog = new Dlg_setupGps(scene, this);
    connect(gpsDialog, &Dlg_setupGps::accepted, this, &Gribouillot::acceptGpsDialog);
    connect(gpsDialog, &Dlg_setupGps::rejected, this, &Gribouillot::hideGpsDialog);
    connect(gpsDialog, &Dlg_setupGps::disableGPS, this, &Gribouillot::disableGps);
}


/**
 * @brief Set the icon showing the drawing color
 */
void Gribouillot::setColorIcon(QColor color)
{
    QPixmap pixmap(18, 18);
    pixmap.fill(color);
    ui->actionChooseColor->setIcon(QIcon(pixmap));
}



/********************* Private functions - Project management **********************/
/**
 * @brief   Factorize code for creating or opening a project.
 */
void Gribouillot::initProject()
{
    if(!currentProjectName.isEmpty())
    {
        maybeSave();
        //Reset interface to default settings
        resetUi();
    }
    else
    {
        //Enable interface which is disabled on start
        ui->centralwidget->setEnabled(true);
        ui->toolBar->setEnabled(true);
        restrictToolbar();

    }
}


/**
 * @brief   Load an existing Gribouillot project
 */
void Gribouillot::openProject(QString gribFile)
{
    if ( !gribFile.isEmpty() )
    {
        QFileInfo gribFileInfo = QFileInfo(gribFile);
        QString notFound ="";

        initProject();

        QDir::setCurrent(gribFileInfo.absolutePath());
        currentProjectName = gribFileInfo.baseName();
        setWindowTitle("Gribouillot - "+currentProjectName);

        QSettings settings(gribFile, QSettings::IniFormat);

        //Restore background map
        mapName = settings.value("map/mapName", "").toString();
        mapPath = settings.value("map/mapPath", "").toString();
        QFileInfo mapFileInfo = QFileInfo(mapPath);

        /*
         * Verify that mapPath is valid, the project folder may have
         * been moved around the file system!
         */
        if ( !mapFileInfo.exists() )
        {
            //map may still exist in the current dir
            QString possibleLocalFile = QDir::currentPath()+"/"+mapFileInfo.fileName();

            if ( QFile::exists(possibleLocalFile) )
                mapPath = possibleLocalFile;
            else
            {
                //can't find map file in the current Dir, ask user
                dlg_changeMap dialog(mapPath, this);
                if (dialog.exec() == QDialog::Accepted)
                    mapPath = dialog.getMapPath();
                else
                    mapPath = QString();//left empty by user
            }
        }

        if ( !mapPath.isEmpty() )
            loadBackgroundMap(mapPath);
        else
            //Load project without a background map
            QMessageBox::information(this,
                                    currentProjectName,
                                    tr("Starting without background map."),
                                    QMessageBox::Ok);

        //Restore drawing color and drawing width
        drawingColor = settings.value("drawing/color", QColor(Qt::black)).value<QColor>();
        setColorIcon(drawingColor);

        drawingWidth = settings.value("drawing/width", 5).toInt();
        QString iconString = ":/Resources/Icons/draw-width-"
                            +QString::number(drawingWidth)
                            +".png";
        ui->actionChooseWidth->setIcon(QIcon(QPixmap(iconString)));


        //Restore custom colors settings
        for (int i = 0; i < QColorDialog::customCount(); ++i)
        {
            QColorDialog::setCustomColor(i,
                settings.value("customColor/color"+QString::number(i), QColor(Qt::white)).value<QColor>());
        }


        //Restore scale spinBoxes.
        //NOTE: setting new SpinBoxes values automatically triggers a new scale computation.
        ui->mapTabWidget->ui->pxSpinBox->setValue(settings.value("scale/pxSpinBx", 0).toDouble());
        ui->mapTabWidget->ui->kmSpinBox->setValue(settings.value("scale/kmSpinBx", 0).toDouble());
        ui->mapTabWidget->ui->mKmComboBox->setCurrentIndex(settings.value("scale/mkmUnit", 0).toInt());


        /*
         * Restore GPS settings. Since initProject() was called, resetUi() was also
         * called and the gpsDialog is currently a null pointer.
         */
        initGpsDialog();

        if (settings.contains("GPS/gpsEnabled"))
        {
            if(settings.value("GPS/gpsEnabled").toBool())
            {
                gpsEnabled = true;
                //change gps icon to ON
                ui->mapTabWidget->ui->gpsTlBtt->setIcon(QIcon(QPixmap(":/Resources/Icons/gps-on.png")));
            }
        }

        //Restore layers from XML files located in current project folder
        settings.beginGroup("layersOrder");
            QMap<int, QString> layers;

            /*
             * Retrieve the indexed order of the layers with a QMap because QSettings
             * write settings values on file in alphabetical order.
             */
            foreach (QString layerKey, settings.allKeys())
            {
                int key = layerKey.toInt();
                layers[key] = settings.value(layerKey).toString()+".xml";

            }
            //qDebug() << layers;

            //Open layers in the correct order
            foreach (QString layerPath, layers)
            {
                if ( QFile::exists(layerPath) )
                    addNewLayer(layerPath);
                else
                    notFound += layerPath+", ";
            }

            if (!notFound.isEmpty())
                QMessageBox::warning(this,
                                     currentProjectName,
                                     tr("Layer(s) file(s) missing in ")
                                     +QDir::currentPath()
                                     +" : "+notFound,
                                     QMessageBox::Ok);
        settings.endGroup();

    }

}


/**
 * @brief Load a picture as background map
 */
void Gribouillot::loadBackgroundMap(QString path)
{
    if(!path.isEmpty())
    {
        if (mapName.isEmpty())
            mapName = QFileInfo(path).baseName();

        backgroundMap->setPixmap(QPixmap(path));
        ui->gribTabWidget->setMapTab(path, mapName);

        /* Limit the scrollable view to the background map*/
        QRectF viewBoundingRect = backgroundMap->boundingRect();
        ui->zGraphicsView->setSceneRect(viewBoundingRect);

        /*
         * Being the first item added to the scene, backgroundMap has the lowest
         * Z value and is always in the background.
         */
        scene->addItem(backgroundMap);

        //Refresh minimap view
        if (minimap != nullptr)
            minimap->fitInView(backgroundMap, Qt::KeepAspectRatio);
    }

}


/**
 * @brief   Save (only) the order of the layers after a layer deletion. Layers are not written on disk!!
 * @details A deleted layer has its corresponding file definitively removed from disk.
 */
void Gribouillot::saveLayersOrder()
{
    QSettings settings(currentProjectName+".grib",
                       QSettings::IniFormat);

    settings.beginGroup("layersOrder");

        settings.remove("");

        for (int i = 1; i < (ui->gribTabWidget->count() -1); ++i)
        {
            GribouillotLayer *layer = dynamic_cast<GribouillotLayer *>(ui->gribTabWidget->widget(i));
            settings.setValue(QString::number(i), layer->getLabel());
        }

    settings.endGroup();

}


/**
 * @brief   typical maybeSave() implementation asking the user to confirm the changes
 *          to the current project. Ui changes are saved separately.
 */
void Gribouillot::maybeSave()
{
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this,
                               tr("Confirm action"),
                               tr("Do you want to save your changes to project \n\"")+currentProjectName+"\"?",
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save)
        saveProject();

}



/*********************** private slots ** MainWindow Menu ****************************/
/**
 * @brief   Pop-up a dialog to create a new Grib project
 * @details A grib project is a directory containing:
 *          - a grib config file (.grib) with map path, scales, etc
 *          - the layers grib files
 */
void Gribouillot::on_actionNewProject_triggered()
{
    Dlg_newGribproject dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString newProjectPath = dialog.getProjectDirPath();

        initProject();

        //Set project path from dialog input
        QDir::setCurrent(newProjectPath);
        currentProjectName = QDir(newProjectPath).dirName();
        setWindowTitle("Gribouillot - "+currentProjectName);

        //Set map path from dialog input and copy map image if necessary
        mapPath = dialog.getMapImagePath();
        if( !mapPath.isEmpty() )
        {
            if( dialog.copyMapImageChkBx() )
            {
                QFileInfo mapFileInfo(mapPath);
                QString newPath = newProjectPath +"/"+ mapFileInfo.fileName();
                QFile::copy(mapPath, newPath);//wont overwrite if already there
                mapPath = newPath;
            }

            loadBackgroundMap(mapPath);
        }

        //Copy loaded external pictures to project folder?
        if ( dialog.copyExternalPictures() )
            ui->actionCopy_pixmaps->setChecked(true);

        //New GPS dialog
        initGpsDialog();

        statusBar()->showMessage("      "+tr("Project created successfully!"));
    }
}


/**
 * @brief   Open an existing Gribouillot project from the menuBar
 */
void Gribouillot::on_actionOpenProject_triggered()
{
    /*
     * Note: getOpenFileName will trigger a Gtk warning message on Gtk UI:
     * "GtkDialog mapped without a transient parent. This is discouraged."
     * This is because Qt source calls a native Gtk window and Gtk can't have
     * a Qt class as parent... Can't be fixed, must be ignored.
     */
    QString gribFile = QFileDialog::getOpenFileName(this,
                                                    tr("Open an existing project"),
                                                    (QDir::currentPath())+"/..",
                                                    tr("Gribouillot project (*.grib)"));
    openProject(gribFile);
}


/**
 * @brief   Import an external layer file to current project
 */
void Gribouillot::on_actionImport_layer_triggered()
{
    Dlg_importLayer dialog(currentProjectName, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString newLayer = dialog.getLayerPath();

        if( !newLayer.isEmpty() )
            addNewLayer(newLayer);

        statusBar()->showMessage("      "+tr("Layer imported successfully!"));
    }
}


/**
 * @brief   save a Gribouillot project
 * @details An unsaved project will still exist as an empty directory because of
 *          the way a new project is created.
 */
int Gribouillot::saveProject()
{
    if(!currentProjectName.isEmpty())
    {
        //NB: We already are in the project directory
        QSettings settings(currentProjectName+".grib",
                           QSettings::IniFormat);

        //Save background map path & name if any
        settings.beginGroup("map");
            settings.setValue("mapPath", mapPath);
            settings.setValue("mapName", mapName);
        settings.endGroup();

        //Save current drawing color and width
        settings.beginGroup("drawing");
            settings.setValue("color", drawingColor);
            settings.setValue("width", drawingWidth);
        settings.endGroup();

        //Save User's custom colors
        settings.beginGroup("customColors");
            for (int i = 0; i < QColorDialog::customCount(); ++i)
            {
                settings.setValue("color"+QString::number(i),
                                  QColorDialog::customColor(i));
            }
        settings.endGroup();

        //Save scale spinBoxes
        settings.beginGroup("scale");
            settings.setValue("pxSpinBx", ui->mapTabWidget->ui->pxSpinBox->value());
            settings.setValue("kmSpinBx", ui->mapTabWidget->ui->kmSpinBox->value());
            settings.setValue("mkmUnit", ui->mapTabWidget->ui->mKmComboBox->currentIndex());
        settings.endGroup();

        //Save GPS data
        settings.beginGroup("GPS");
            settings.setValue("gpsEnabled", gpsEnabled);
        settings.endGroup();

        //Save layers
        settings.beginGroup("layersOrder");
        settings.remove("");

            QStringList savedLabels;
            /*
             * Ask each layer to write its own XML file but save the tab order in the grib file.
             * Some explanations on the loop values:
             * int i = 1  and not int i = 0    becoz the first tab is no layer (mapTab)
             * count() -1 and not just count() becoz the last tab is no layer (plusTab)
             */
            for (int i = 1; i < (ui->gribTabWidget->count() -1); ++i)
            {
                GribouillotLayer *layer = dynamic_cast<GribouillotLayer *>(ui->gribTabWidget->widget(i));
                QString label = layer->getLabel();

                if ( savedLabels.contains(label) )
                {
                    //Two layers with the same label would write into the same file on disk!
                    ui->gribTabWidget->setTabIcon(i,QIcon(QPixmap(":/Resources/Icons/exclamation.png")));
                    QMessageBox::warning(this,
                             tr("Layer name conflict!"),
                             label+tr(": another layer was already saved under "
                             "this name, please change the name of the layer."),
                             QMessageBox::Ok);
                    label = layer->newLabel(true);//a modal dialog asks for new label
                    ui->gribTabWidget->setTabIcon(i, QIcon());
                    ui->gribTabWidget->setTabText(i, label);
                    //At this point the layer has a new label, we can move on
                }

                if(layer->writeXML())
                {
                    settings.setValue(QString::number(i), label);
                    savedLabels << label;
                }
            }

        settings.endGroup();

        QSettings appSettings("GAT", "Gribouillot");
        QString projectFile = QDir::currentPath()+"/"+currentProjectName+".grib";
        appSettings.setValue("recentProject", projectFile);


        //User feedback
        statusBar()->showMessage(tr("Project saved (")+
                                 QString::number(savedLabels.size())+
                                 tr(" layers saved)."));

        return savedLabels.size();

    }

    return -1;
}


/**
 * @brief Wrapper around saveProject to inform user on autosave
 */
void Gribouillot::autoSaveProject()
{
    int layersSaved = saveProject();

    if ( layersSaved != -1 )
        statusBar()->showMessage(tr("Autosave done (")
                                 +QString::number(layersSaved)
                                 +tr(" layers saved)."));

}


/**
 * @brief Start autosave timer
 */
void Gribouillot::newSaveTimeout()
{
    if( autosaveTimeout != 0)
    {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &Gribouillot::autoSaveProject);
        connect(timer, &QTimer::timeout, this, &Gribouillot::writeSettings);
        timer->start(autosaveTimeout*60000);

        ui->actionAutosave->setChecked(true);
    }
    else
        ui->actionAutosave->setChecked(false);
}


/**
 * @brief   Export the view as an image file
 */
void Gribouillot::on_actionExportAs_triggered()
{
    //calls to global functions
    QString supportedFormats = getSupportedImageFormats();
    QString defaultFilter = getDefaultImageFilter(supportedFormats);

    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Export current view"),
                                                    QDir::currentPath(),
                                                    supportedFormats,
                                                    &defaultFilter);
    if(!filename.isNull())
    {
        QPixmap pix = ui->zGraphicsView->grab();
        pix.save(filename);
    }

}


/**
 * @brief   Close Gribouillot
 */
void Gribouillot::on_actionQuit_triggered()
{
    close(); //will call closeEvent() then maybeSave()
}


/**
 * @brief   Display a Minimap to navigate through large map
 */
void Gribouillot::on_actionMinimap_toggled(bool isChecked)
{
    if (isChecked && minimap == nullptr)
    {
        minimap = new Minimap(scene, ui->zGraphicsView);
        minimap->fitInView(backgroundMap, Qt::KeepAspectRatio);
    }
    else
    {
        delete minimap;
        minimap = nullptr;
    }
}


/**
 * @brief   Tell the current layer to compute the center of Mass of its points
 */
void Gribouillot::on_actionCenterOfMass_triggered()
{
    Item_point *centerOfMass = currentLayer->computeCoM(drawingColor, drawingWidth);

    if ( centerOfMass != nullptr )
        ui->zGraphicsView->centerOn(centerOfMass);

}


/**
 * @brief   Set up the options for drawing a 4 centers spiral
 */
void Gribouillot::on_actionSpiralOptions_triggered()
{
    spiralDialog->show();
    spiralDialog->raise();
}


/**
 * @brief   Save the project automatically every x minutes
 */
void Gribouillot::on_actionAutosave_triggered()
{
    Dlg_autosave dialog(this, autosaveTimeout);
    if (dialog.exec() == QDialog::Accepted)
    {
        //Delete current timer if any
        if(timer != nullptr)
        {
            timer->disconnect();
            delete timer; timer = nullptr;
        }

        //Retrieve new timeout value
        autosaveTimeout = dialog.getTimeoutValue();
        newSaveTimeout();
    }
    else
    {
        /*
         * Specific action check / uncheck in menu, because the button
         * would otherwise toggle no matter the autosave value.
         */
        if (autosaveTimeout == 0)
            ui->actionAutosave->setChecked(false);
        else
            ui->actionAutosave->setChecked(true);
    }

}



/**
 * @brief   Display informations about Gribouillot
 */
void Gribouillot::on_actionAbout_Grib_triggered()
{
    QMessageBox::about(this,
                       tr("About Gribouillot"),
                       tr(
    "<p>Gribouillot is an open source application under GPL3 license. Please refer to "
    "<a href=\"http://www.gnu.org/licenses/\">http://www.gnu.org/licenses/</a> for the license details."
    "<p><br/>This software was created with the first intent to help people in search of the "
    "Golden Owl, a famous French treasure.<br/>"
    "Author : <b>Ludovic A.</b></p>"));
}



/*********************** private slots ** Scene events ****************************
 *
 * see gribouillot_toolbar.cpp for Scene events and toolbar actions
 *
 */


/******************* private slots ** mapTabWidget  ********************/
/**
 * @brief   Change the name of the map visible in the tabWidget
 */
void Gribouillot::mapNameTlBttClicked()
{
    QString newName = QInputDialog::getText(this,
                                         tr("Rename the map Tab"),
                                         tr("For cosmetic purpose only, the file on disk won't be renamed.\n"
                                            "Also the file name will still appear as a tooltip on mouseover."));
    if (!newName.isNull())
    {
        mapName = newName;
        ui->gribTabWidget->setMapTab(mapPath, mapName);
    }
}


/**
 * @brief   Switch the background map to color / black&white
 */
void Gribouillot::blackWhiteTlBttClicked(bool isChecked)
{
    /**
     * Lambda to turn color pixels to black&white in the given range of pixels
     * using typical luma coefficients
     */
    auto color2bw = [](QImage *color_img, QImage *bw_img, int start_x, int end_x)
    {
        QColor oldColor;
        for(int x = start_x; x < end_x; x++)
        {
            for(int y = 0; y < bw_img->height(); y++)
            {
                oldColor = QColor(color_img->pixel(x,y));
                int Y = 0.2126*oldColor.red()+ 0.7152*oldColor.green()+0.0722*oldColor.blue();
                bw_img->setPixel(x,y,qRgb(Y, Y, Y));
            }
        }
    };


    if (isChecked)
    {/* Create a grayscale image */

        QImage *mapImage = new QImage(mapPath);
        QImage *bwMapImage = new QImage(mapImage->width(), mapImage->height(), QImage::Format_ARGB32);

        //time_t start, end;
        //time(&start);
        //
        int thrd_count = std::thread::hardware_concurrency();
        //qDebug() << "Number of threads available: " << thrd_count;

        //Note: the division below can yield a remainder, handled by the main thread
        int chunk_size = mapImage->width() / thrd_count;//rounded down
        //qDebug() << "Chunk size: " << chunk_size;

        int i = 0;
        if (thrd_count > 1 && mapImage->width() > 200)
        {
            std::thread *tt = new std::thread[thrd_count - 1];

            //Launch count-1 threads, processing chunk_size pixels each
            for (/*i=0*/; i < thrd_count -1; i++)
            {
                tt[i] = std::thread(color2bw, mapImage, bwMapImage,
                                    i*chunk_size,
                                    (i+1)*chunk_size
                                    );
            }

            for (int t = 0; t < thrd_count -1; t++)
                tt[t].join();
        }
        else
            qDebug() << "Only 1 thread available or low pixel count";

        /*
         * If mono-thread then i = 0, compute entire image. Otherwise compute the last
         * chunk of the image and possible remainder.
         */
        //qDebug() << "Last chunk size: " << bwMapImage->width() - i*chunk_size;
        color2bw(mapImage, bwMapImage, i*chunk_size, bwMapImage->width());

        //
        //time(&end);
        //qDebug() << difftime(end, start) << " seconds";

        backgroundMap->setPixmap(QPixmap::fromImage(*bwMapImage));
    } else
        backgroundMap->setPixmap(QPixmap(mapPath));

}


/**
 * @brief   use a measuring tape to set the internal km/px ratio
 */
void Gribouillot::scaleRulerTlBttTriggered()
{
    uncheckDrawingGroup();
    setDrawingView();
    currentDrawing = SCALERULER;

    Item_scaleRuler* scaleRuler = new Item_scaleRuler();
    connect(scaleRuler, &Item_scaleRuler::newMeasure,ui->mapTabWidget, &MapTabWidget::newScaleMeasure);
    scene->addItem(scaleRuler);
    scaleRuler->grabMouse();//to respond to mouse clicks

}



/**
 * @brief   setup an approximate GPS system using 2 reference points
 */
void Gribouillot::gpsTlBttClicked()
{
    uncheckDrawingGroup();

    gpsDialog->show();
    gpsDialog->raise();

}

/**
 * @brief   Disable gps data computing
 */
void Gribouillot::disableGps()
{
    gpsEnabled = false;
    //change gps icon to OFF
    ui->mapTabWidget->ui->gpsTlBtt->setIcon(QIcon(QPixmap(":/Resources/Icons/gps-off.png")));
    statusBar()->showMessage(tr("GPS computation disabled."));

    hideGpsDialog();

}


/**
 * @brief   Setup the GPS system
 */
void Gribouillot::acceptGpsDialog()
{
    gpsEnabled = true;
    //change gps icon to ON
    ui->mapTabWidget->ui->gpsTlBtt->setIcon(QIcon(QPixmap(":/Resources/Icons/gps-on.png")));
    statusBar()->showMessage(tr("GPS computation enabled."));

    hideGpsDialog();
}

/**
 * @brief   restore the context after GPS setup
 */
void Gribouillot::hideGpsDialog()
{
    //Hide the dialog window
    gpsDialog->hide();
    ui->mapTabWidget->ui->gpsTlBtt->setEnabled(true);

    setSelectionView();
}



/************************ private slots ** layerTab actions ************************/
/**
 * @brief   Action upon tab clicked
 * @details Basically clicking a tab change the view to this tab, except for the +tab.
 */
void Gribouillot::tabWidgetClicked(int newTabIndex)
{
//    qDebug() << "In tabWidgetClicked";

    if ( newTabIndex != currentTabIndex )
    {
        if ( newTabIndex == -1 )
        {
//            qDebug() << "the +tab is clicked.";
            addNewLayer();
        }
        else
        {
//            qDebug() << "other tab clicked.";
            ui->gribTabWidget->setCurrentIndex(newTabIndex);//emits a currentChanged() signal
        }

    }
}


/**
 * @brief Handle change of active tab. Connected to currentChanged() signal
 *
 */
void Gribouillot::switchToTabIndex(int newTabIndex)
{
    switch (newTabIndex)
    {
        case 0:
            //mapTab is displayed, disable items
            foreach( QGraphicsItem* item, scene->items())
                item->setEnabled(false);

            restrictToolbar();
            break;

        default:
            //A graphic layer is displayed, set up the working environment
            currentLayer = dynamic_cast<GribouillotLayer *>(ui->gribTabWidget->widget(newTabIndex));
            setWorkingLayer(currentLayer);

    }

    currentTabIndex = newTabIndex;
//    qDebug() << "new currentTabIndex: " << newTabIndex;

}


/**
 * @brief   Update the name of the layer in the tabBar
 */
void Gribouillot::doChangeLayerName(QString label)
{
    ui->gribTabWidget->setTabText(currentTabIndex, label);

}


/**
 * @brief   delete a layer and its corresponding tab
 */
void Gribouillot::doDeleteLayer()
{
    GribouillotLayer* layerX = currentLayer;
    ui->gribTabWidget->removeTab(currentTabIndex);//emits a currentChanged() signal
    delete layerX;//At this point currentLayer != layerX thanks to removeTab()

    saveLayersOrder();

}

/**
 * @brief   add an item to the scene upon request from a layer
 */
void Gribouillot::doAddItemToScene(QGraphicsItem* item)
{
    scene->addItem(item);
}
