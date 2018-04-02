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

#include <QtMath>
#include <QtWidgets>
#include <QtDebug>

#include "ui_gribouillot.h"
#include "ui_maptabwidget.h"

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
    currentProjectName(""),
    mapTabName(""),
    backgroundMap(new QGraphicsPixmapItem),

    currentTabIndex(0),
    callsForAChange(0),

    drawingColor(Qt::black),
    drawingWidth(5),
    currentDrawing(NONE),

    scaleRuler(new Item_scaleRuler)

{
    ui->setupUi(this);

    //Menu connections
    connect (ui->actionAbout_Qt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));

    //TabWidget
    ui->tabWidget->setTabText(0, tr("Background map"));
    ui->tabWidget->tabBar()->setSelectionBehaviorOnRemove(QTabBar::SelectLeftTab);
    createPlusTab();

    //mapTabWidget is an extension of the mainWindow and even a "friendly" Class.
    connect (ui->mapTabWidget->ui->mapTabNameTlBtt, SIGNAL(clicked()), this, SLOT(mapTabNameTlBttClicked()));
    connect (ui->mapTabWidget->ui->blackWhiteTlBtt, SIGNAL(toggled(bool)), this, SLOT(blackWhiteTlBttClicked(bool)));
    connect (ui->mapTabWidget->ui->scaleRulerTlBtt, SIGNAL(toggled(bool)), this, SLOT(scaleRulerTlBttToggled(bool)));
    connect (ui->mapTabWidget->ui->gpsTlBtt, SIGNAL(clicked()), this, SLOT(gpsTlBttClicked()));

    //The spiral dialog is an extension of the interface which is shown/hidden if necessary
    spiralDialog = new Dlg_spiral(this);

    //graphicsView and its drawing cursor
    ui->zGraphicsView->setScene(scene);
    drawingCursor = QCursor(QPixmap(":/Resources/Icons/cursor-drawing.png"));

    //scene
    connect (scene, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()));
    connect (scene, SIGNAL(newMouseClickPreSelect(QPointF)), this, SLOT(newSceneClickPreSelect(QPointF)));
    connect (scene, SIGNAL(newMouseClickPostSelect(QPointF)), this, SLOT(newSceneClickPostSelect(QPointF)));
    connect (scene, SIGNAL(newMouseMove(QPointF)), this, SLOT(newMoveOnScene(QPointF)));
    connect (scene, SIGNAL(keyDeletePressed()), this, SLOT(keyDeleteFromScene()));
    connect (scene, SIGNAL(keySpacePressed()), this, SLOT(keySpaceFromScene()));
    connect (scene, SIGNAL(keyCPressed()), this, SLOT(on_actionChooseColor_triggered()));
    connect (scene, SIGNAL(keyTPressed()), this, SLOT(keyTFromScene()));

    //Connect mapTabWidget to zGraphicsView in order to update the black & white scale bar
    connect(ui->mapTabWidget, SIGNAL(newSystemScale(qreal, QString)), ui->zGraphicsView, SLOT(systemScaleChanged(qreal, QString)));

    //Toolbar
    drawingGroup = new QActionGroup(this);
    drawingGroup->addAction(ui->actionCursorDrag);
    drawingGroup->addAction(ui->actionCursorSelect);
    drawingGroup->addAction(ui->actionMeasureDistance);
    drawingGroup->addAction(ui->actionMeasureAngle);

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
    connect(drawingGroup, SIGNAL(triggered(QAction*)), ui->mapTabWidget, SLOT(uncheckDrawingButtons()));

    //Disable everything before a first project is created
    ui->toolBar->setEnabled(false);
    ui->centralwidget->setEnabled(false);

    ui->mapTabWidget->ui->gpsTlBtt->setVisible(false);//DEV
    ui->actionMeasureAngle->setVisible(false);//DEV

    show();

    openProject("/home/tyler/Grib/The Big Project/The Big Project.grib");//DEV

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
    //if(!currentProjectName.isEmpty())//DEV
      //  maybeSave();//DEV
    event->accept();
}



/************************* Private functions - UI ****************************/

/**
 * @brief   Create a tab with a "+" icon at the end of the tabBar.
 * @details When clicked, this will generate a new layer.
 */
void Gribouillot::createPlusTab()
{
    ui->tabWidget->tabBar()->addTab(QIcon(":/Resources/Icons/add-layer.svg"), QString());
    int plusTabIndex = ui->tabWidget->count() - 1;
    ui->tabWidget->setTabEnabled(plusTabIndex, false);
    ui->tabWidget->setTabToolTip(plusTabIndex, tr("Create new layer"));
}

/**
 * @brief   Do not allow the use of layer-specific actions.
 * @details Hard coded limit depending on the ui design. See gribouillot.ui
 */
void Gribouillot::restrictToolbar()
{
    QList<QAction *> actionsList = ui->toolBar->actions();
    for (int i=4; i < actionsList.size(); ++i)//4 is hardcoded
        (actionsList.at(i))->setEnabled(false);

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
 * @brief   Enable all items (from all layers) and the specifics of the items
 *          which belong to the given 'layer'.
 */
void Gribouillot::enableItemsAndSpecifics(GribouillotLayer* layer)
{
    bool specifics;

    foreach(QGraphicsItem* item, scene->items())
    {
        specifics = layer->contains(item) ? true : false;

        //enable item
        item->setEnabled(true);

        //enable or not specifics
        if( item->type() == PIXMAP )
            item->setFlag(QGraphicsItem::ItemIsMovable, specifics);
        if ( item->type() == SPIRAL)
            item->setEnabled(specifics);
        //to complete if necessary
    }
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


/**
 * @brief Clear the GraphicsView from drawing helps
 */
void Gribouillot::clearView()
{
    if(currentDrawing != NONE)
    {
        /*
         * Remove any "drawing help" item if the drawingView is
         * re-triggered or unchecked.
         */
        foreach(QGraphicsItem* item, scene->items())
        {
            if(item->type() == ARC_DRAWER
               || item->type() == POINT_ONRAIL
               || item->type() == SPIRAL_DRAWER )
            {
                scene->removeItem(item);
                delete item;
            }
        }

        drawingCoords.clear();//vector to store drawing positions
    }

    //scene->clearSelection();
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
    ui->zGraphicsView->setCursor(drawingCursor);//GraphicsView cursor
    ui->zGraphicsView->setDragMode(QGraphicsView::NoDrag);
}


/**
 * @brief   Set a generic Arrow cursor for item selection, etc.
 * @details By definition selectionView is exclusive of drawingView
 */
void Gribouillot::setSelectionView()
{
    clearView();
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
         *  An exclusive actionGroup does not allow for manual checking/
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
void Gribouillot::resetUI()
{
    //reset Menus
    ui->actionCopy_pixmaps->setChecked(true);
    ui->actionMinimap->setChecked(false);

    //reset MapTabWidget
    ui->mapTabWidget->resetUI();

    /*
     * Reset TabWidget (delete all layers).
     * NB: This will call restrictToolbar() and select default cursor.
     * count()-2 and not just count() becoz mapTab and plusTab are no layers!
     */
    int layersCount = ui->tabWidget->count()-2;//necessary, count() is modified in the for loop
    for (int i = 0; i < layersCount; ++i)
    {
        //widget(1) is different each time!
        GribouillotLayer *layer = dynamic_cast<GribouillotLayer *>(ui->tabWidget->widget(1));
        ui->tabWidget->removeTab(1);//modify ui->tabWidget->count()!
        delete layer;
    }
    GribouillotLayer::resetLayerIndex();

    //reset scene and delete all items including backgroundMap & beacons
    scene->clear();
    scene->setSceneRect(0, 0, 0, 0);

    //new empty background map
    backgroundMap = new QGraphicsPixmapItem();
    mapTabName = "";
    ui->tabWidget->setTabText(0, "Map");
    ui->tabWidget->setTabToolTip(0, "");

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
    connect(gpsDialog, SIGNAL(accepted()), this, SLOT(acceptGpsDialog()));
    connect(gpsDialog, SIGNAL(rejected()), this, SLOT(hideGpsDialog()));
    connect(gpsDialog, SIGNAL(disableGPS()), this, SLOT(disableGps()));
}



/********************* Private functions - Project management **********************/
/**
 * @brief   typical maybeSave() implementation asking the user to confirm a save
 */
void Gribouillot::maybeSave()
{
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this,
                               tr("Confirm action"),
                               tr("Do you want to save changes to project \"")+currentProjectName+"\" ?",
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save)
        saveProject();

}


/**
 * @brief   Factorize code for creating or opening a project.
 */
void Gribouillot::initProject()
{
    if(!currentProjectName.isEmpty())
    {
        maybeSave();
        //Reset interface to default settings
        resetUI();
    } else {
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
        mapPath = settings.value("map/mapPath").toString();
        mapTabName = settings.value("map/mapTabName").toString();
        QFileInfo mapFileInfo = QFileInfo(mapPath);

        /* verify that mapPath is valid, the project folder may have
         * been moved around the file system!
         */
        if ( !mapFileInfo.exists() )
        {
            //map may still exists in the current dir though
            QString possibleLocalFile = QDir::currentPath()+"/"+mapFileInfo.fileName();

            if ( QFile::exists(possibleLocalFile) )
            {
                //ok, change mapPath to current path
                mapPath = QDir::currentPath()+"/"+mapFileInfo.fileName();
            }
            else
            {
                //can't find map file in the current Dir, ask user
                dlg_changeMap dialog(mapPath, this);
                if (dialog.exec() == QDialog::Accepted)
                {
                    mapPath = dialog.getMapPath();
                    mapTabName = QFileInfo(mapPath).baseName();
                }
                else
                    mapPath = QString();//empty
            }
        }

        if ( !mapPath.isEmpty() )
        {
            loadBackgroundMap(mapPath);

            if (!mapTabName.isEmpty())
            {
                ui->tabWidget->setTabText(0, mapTabName);
                ui->tabWidget->setTabToolTip(0, mapPath);
            }
        }
        else
            //Load project without a background map
            QMessageBox::warning(this,
                                 currentProjectName,
                                 tr("No background map selected!"),
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
        //NOTE: setting new SpinBox values automatically triggers a new scale computation.
        ui->mapTabWidget->ui->pxSpinBox->setValue(settings.value("scale/pxSpinBx").toDouble());
        ui->mapTabWidget->ui->kmSpinBox->setValue(settings.value("scale/kmSpinBx").toDouble());
        ui->mapTabWidget->ui->mKmComboBox->setCurrentIndex(settings.value("scale/mkmUnit").toInt());


        /*
         * Restore GPS settings. Since initProject() was called, resetUI() was also
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


        //Display minimap if necessary TODO: project independent setting
        if(settings.value("minimap").toBool())
        {
            ui->actionMinimap->setChecked(true);
            //minimap->setGeometry(QRect(0,0,250,150));
        }


        //Restore spiral dialog settings TODO: project independent setting
        settings.beginGroup("spiral");
        spiralDialog->loadData( settings.value("constructAsOneItem").toBool(),
                                settings.value("baseDisplay").toBool(),
                                settings.value("showBaseCentersOnly").toBool());
        settings.endGroup();


        //Restore Autosave settings TODO: project independent setting
        settings.beginGroup("preferences");
        ui->actionCopy_pixmaps->setChecked( settings.value("copyPixmaps", true).toBool() );
        autosaveTime = settings.value("autosave", 0).toInt();
        settings.endGroup();


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
    ui->tabWidget->setTabText(0, QDir(path).dirName());
    backgroundMap->setPixmap(QPixmap(path));

    /** Limit the scrollable view to the background map*/
    QRectF viewBoundingRect = backgroundMap->boundingRect();
    ui->zGraphicsView->setSceneRect(viewBoundingRect);

    /**
     * Being the first item added to the scene, backgroundMap has the lowest
     * Z value and is always in the background.
     */
    scene->addItem(backgroundMap);
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

    //Insert the new layer widget into the tabWidget UI.
    ui->tabWidget->insertTab(currentTabIndex+1,
                             newLayer,
                             newLayer->getLabel());

    ui->tabWidget->setCurrentWidget(newLayer);//tabPageChanged will be called

}


/**
 * @brief   Save the order of the layers after a layer deletion. Order only!
 * @detail  A deleted layer has its corresponding file definitively removed from disk.
 */
void Gribouillot::saveLayersOrder()
{

    QSettings settings(currentProjectName+".grib",
                       QSettings::IniFormat);

    settings.beginGroup("layersOrder");
        settings.remove("");

        for (int i = 1; i < (ui->tabWidget->count() -1); ++i)
        {
            GribouillotLayer *layer = dynamic_cast<GribouillotLayer *>(ui->tabWidget->widget(i));
            settings.setValue(QString::number(i), layer->getLabel());
        }
    settings.endGroup();

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
void Gribouillot::saveProject()
{
    if(!currentProjectName.isEmpty())
    {
        //NB: We already are in the project directory
        QSettings settings(currentProjectName+".grib",
                           QSettings::IniFormat);

        //Save background map path & name if any
        settings.beginGroup("map");
            settings.setValue("mapPath", mapPath);
            settings.setValue("mapTabName", mapTabName);
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


        //save Minimap mode
        settings.setValue("minimap", ui->actionMinimap->isChecked());

        //save drawing options for 4C spiral
        settings.beginGroup("spiral");
            settings.setValue("constructAsOneItem", spiralDialog->isOneItem());
            settings.setValue("baseDisplay", spiralDialog->isBaseDisplayed());
            settings.setValue("showBaseCentersOnly", spiralDialog->showBaseCentersOnly());
        settings.endGroup();


        settings.beginGroup("preferences");
            settings.setValue("copyPixmaps", ui->actionCopy_pixmaps->isChecked());
            settings.setValue("autosave", autosaveTime);
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
            for (int i = 1; i < (ui->tabWidget->count() -1); ++i)
            {
                GribouillotLayer *layer = dynamic_cast<GribouillotLayer *>(ui->tabWidget->widget(i));
                QString label = layer->getLabel();

                if ( savedLabels.contains(label) )
                {
                    //Two layers with the same label would write into the same file on disk!
                    ui->tabWidget->setTabIcon(i,QIcon(QPixmap(":/Resources/Icons/exclamation.png")));
                    QMessageBox::warning(this,
                             tr("Layer name conflict!"),
                             label+tr(": another layer was already saved under "
                             "this name, please change the name of the layer."),
                             QMessageBox::Ok);
                    label = layer->newLabel(true);//a dialog asks for new label
                    ui->tabWidget->setTabIcon(i, QIcon());
                    ui->tabWidget->setTabText(i, label);
                    //At this point the layer has a new label, we can continue normally
                }

                if(layer->writeXML())
                {
                    settings.setValue(QString::number(i), label);
                    savedLabels << label;
                }
            }

        settings.endGroup();


        //User feedback
        statusBar()->showMessage(tr("Project saved (")+
                                 QString::number(savedLabels.size())+
                                 tr(" layers saved)."));
    }
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
        minimap->fitInView(backgroundMap);
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
    qDebug() << "triggered";

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
void Gribouillot::mapTabNameTlBttClicked()
{
    QString newName = QInputDialog::getText(this,
                                         tr("Rename the map Tab"),
                                         tr("For cosmetic purpose only, the file on disk won't be renamed.\n"
                                            "Also the file name will still appear as a tooltip on mouseover."));
    if (!newName.isNull())
    {
        mapTabName = newName;
        ui->tabWidget->setTabText(0, mapTabName);
        ui->tabWidget->setTabToolTip(0, mapPath);
    }
}


/**
 * @brief   Switch the background map to color / black&white
 */
void Gribouillot::blackWhiteTlBttClicked(bool isChecked)
{
    if (isChecked)
    {
        /** Create a grayscale image using typical luma coefficients */
        QImage *mapImage = new QImage(mapPath);
        QImage *bwMapImage = new QImage(mapImage->width(), mapImage->height(), QImage::Format_ARGB32);

        QColor oldColor;
        for(int x = 0; x<bwMapImage->width(); x++)
        {
            for(int y = 0; y<bwMapImage->height(); y++)
            {
                oldColor = QColor(mapImage->pixel(x,y));
                int Y = 0.2126*oldColor.red()+ 0.7152*oldColor.green()+0.0722*oldColor.blue();
                bwMapImage->setPixel(x,y,qRgb(Y, Y, Y));
            }
        }
        backgroundMap->setPixmap(QPixmap::fromImage(*bwMapImage));
    } else
        backgroundMap->setPixmap(QPixmap(mapPath));

}


/**
 * @brief   use a scaleRuler item to set the internal km/px ratio
 */
void Gribouillot::scaleRulerTlBttToggled(bool checked)
{
    if (checked)
    {
        uncheckDrawingGroup();
        initScaleRuler();
        connect(scaleRuler, SIGNAL(newMeasure(qreal)),ui->mapTabWidget, SLOT(newScaleMeasure(double)));
    }
    else
        scene->removeItem(scaleRuler);

}


/**
 * @brief   Init the scaleRuler to set the scale or measure a distance
 * @details Called by scaleRulerTlBttClicked() and on_actionMeasureDistance_triggered()
 */
void Gribouillot::initScaleRuler()
{
    //disconnect any previous signal
    disconnect(scaleRuler, SIGNAL(newMeasure(qreal)),0, 0);
    //reset position and drawing counter
    scaleRuler->reset();
    //addItem instead of just show/hide so the ruler is always displayed on top
    scene->addItem(scaleRuler);
    /*
     * Grab the mouse to receive events directly, no need to set currentDrawing.
     * See item_scaleRuler protected functions (mousePressEvent...) for more details.
     */
    scaleRuler->grabMouse();

    setDrawingView();

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
    //qDebug() << "In tabWidgetClicked";

    if ( newTabIndex != currentTabIndex )
    {
        if ( newTabIndex == -1 )
        {
            //qDebug() << "the +tab is clicked.";

            /**
             * Because of QTabBar event system, the tabWidget always switch to the last
             * tab upon a new tab insertion, despite setCurrentWidget() in AddNewLayer.
             * It is fine only if the new tab is inserted in last position. Otherwise we
             * need to hack a bit with this 'callsForAChange'. See also tabPageChanged().
             */
            if ( ! (currentTabIndex == ui->tabWidget->count()-2) )
            {
                //qDebug() << "The new tab is not inserted in last position.";
                callsForAChange = 2;//initialize the hack
            }
            addNewLayer();
        }
        //else
            //A normal tab change occurs. It is handled in tabPageChanged().
    }
}


/**
 * @brief   The displayed tab has changed and currentTabIndex must be updated.
 * @details If a layer is now displayed, enable its graphical items for editing.
 */
void Gribouillot::tabPageChanged(int newTabIndex)
{
    //qDebug() << "In tabPageChanged with newTabIndex: " << newTabIndex;

    //Disengage points weights display
    ui->actionPointWeight->setChecked(false);

    switch (newTabIndex)
    {
        case 0:
            //mapTab is displayed

            /* disable items */
            foreach( QGraphicsItem* item, scene->items())
                item->setEnabled(false);

            restrictToolbar();
            break;

        default:
            //A layer is displayed

            /* enable items and specifics for the current layer */
            currentLayer = dynamic_cast<GribouillotLayer *>(ui->tabWidget->currentWidget());
            enableItemsAndSpecifics(currentLayer);

            fullToolbar();
    }

    /**
     * Forcing the display to the proper tab in the case of tab insertion.
     * This is a HACK because of a bug (or feature?) in the Qt library
     * The value of callsForAChange is either 0, 1 or 2.
     */
    switch (callsForAChange)
    {
        case 1:
            --callsForAChange;
            //qDebug() << "tabPageChanged was wrongfully called by QTabBar";
            //qDebug() << "Forcing the display of the proper currentTabIndex \n{";
            ui->tabWidget->setCurrentIndex(currentTabIndex);
            //qDebug() << "}//forced";
            break;

        case 2:
            //tabPageChanged is called a 1rst time from AddNewLayer(), decrease counter...
            --callsForAChange;
            //...and proceed normally to default:
        default:
            currentTabIndex = newTabIndex;
    }

    //qDebug() << "End of tabPageChanged, currentTabIndex updated to: " << currentTabIndex;
}


/**
 * @brief   Update the name of the layer in the tabBar
 */
void Gribouillot::doChangeLayerName(QString label)
{
    ui->tabWidget->setTabText(currentTabIndex, label);

}


/**
 * @brief   delete a layer and its corresponding tab
 */
void Gribouillot::doDeleteLayer()
{
    GribouillotLayer* layerX = currentLayer;
    ui->tabWidget->removeTab(currentTabIndex);
    //After removeTab() we have currentLayer != layerX
    delete layerX;

    saveLayersOrder();
}

/**
 * @brief   add an item to the scene upon request from a layer
 */
void Gribouillot::doAddItemToScene(QGraphicsItem* item)
{
    scene->addItem(item);
}
