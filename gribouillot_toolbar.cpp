/**
 *  @brief     The QMainWindow of Gribouillot
 *  @details   Implements the GraphicsView which receives the drawing events,
 *             a TabWidget for layers pages,
 *             a DockWidget for the drawing tools
 *  @author    Ludovic A. 
 *  @date      2015/2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QtWidgets>
#include <QtDebug>

#include "ui_gribouillot.h"

#include "dlg_penthickness.h"
#include "gribouillot.h"


/*********************** private slots ** Scene events ****************************/
/**
 * @brief   Send the mouse clicks from scene BEFORE default scene processing.
 * @details Important for item drawing starting with "if ( isOnlySelected(...) )"
 *          because the scene default processing is selecting a new item. All drawings
 *          which do not depend on selection are also made here.
 */
void Gribouillot::newSceneClickPreSelect(QPointF position)
{
    QPointF noCenter(-10000, -10000);
    QPointF center = noCenter;

    //Some visual helps are 'self-drawing' items catching the mouse
    QGraphicsItem* mG = scene->mouseGrabberItem();

    switch (currentDrawing)
    {
        case POINT_W :
            currentLayer->drawPoint(drawingColor, drawingWidth, position,
                                    ui->actionPointWeight->isChecked());
            break;

        case SEGMENT:
            if (!moreCoordsNeeded(position))
            {
                currentLayer->drawSegment(drawingColor, drawingWidth, drawingCoords);
                on_actionSegment_triggered();//reset drawing tool
            }
            break;

        case LINE:
            if (!moreCoordsNeeded(position))
            {
                currentLayer->drawLineFromSegment(drawingColor, drawingWidth, drawingCoords);
                on_actionLine_triggered();//reset drawing tool
            }
            break;

        case HORIZONTAL:
            currentLayer->drawHorizontal(drawingColor, drawingWidth, position);
            break;

        case VERTICAL:
            currentLayer->drawVertical(drawingColor, drawingWidth, position);
            break;

        case PARALLEL:
            if( isOnlySelected({SEGMENT, LINE}, 1) )
            {//A line or segment was previously chose by user
                currentLayer->drawParallel(drawingColor, drawingWidth, position);

                on_actionParallel_triggered();//reset drawing tool
            }
            else
                statusBar()->showMessage(drawingTips[0]);

            break;

        case PERPENDICULAR:
            if( isOnlySelected({SEGMENT, LINE}, 1) )
            {//A line or segment was previously chose by user
                currentLayer->drawPerpendicular(drawingColor, drawingWidth, position);
            }
            else
                statusBar()->showMessage(drawingTips[0]);

            break;

        case CIRCLE_FROMRADIUS:
            if( isOnlySelected({SEGMENT}, 1) )
            {//A radius was previously selected. This click defines a side.
                center =
                currentLayer->drawCircleFromRadius(drawingColor, drawingWidth,
                                                    position);

                //Reset selection (selected segment) and drawing tool
                scene->clearSelection();
                on_actionCircleCenterPoint_triggered();
            }
            else if (!moreCoordsNeeded(position))
            {//Another way: the radius corresponds to 2 hand-selected points.
                center =
                currentLayer->drawCircleFromRadius(drawingColor, drawingWidth,
                                                    drawingCoords);

                on_actionCircleCenterPoint_triggered();//reset drawing tool
            }
            break;

        case CIRCLE_FROMRADIUSVALUE:
            center =
            currentLayer->drawCircleFromRadius(drawingColor, drawingWidth,
                                                 position,//center coordinates
                                                 ui->mapTabWidget->getKmPxScale());
            break;

        case CIRCLE_FROMDIAMETER:
            if (!moreCoordsNeeded(position))
            {
                center =
                currentLayer->drawCircleFromDiameter(drawingColor, drawingWidth,
                                                    drawingCoords);

                on_actionCircleDiameter_triggered();//reset drawing tool
            }
            break;

        case CIRCLE_FROMTRIANGLE:
            if (!moreCoordsNeeded(position))
            {
                center =
                currentLayer->drawCircleFromTriangle(drawingColor, drawingWidth,
                                                    drawingCoords);

                on_actionCircleTriangle_triggered();//reset drawing tool
            }
            break;

        case ARC:
            if ( mG != nullptr && mG->type() == ARC_DRAWER)
            {//this LAST click indicates the end of the arc drawing.

                Item_arcDrawer* aD = qgraphicsitem_cast<Item_arcDrawer*>(mG);
                currentLayer->drawArc(drawingColor, drawingWidth, aD->getArc());
                /*
                 * Reset drawing. Helper item, which is the non-null mouseGrabber at
                 * this point, will be deleted soon by a call to setDrawingView().
                 */
                on_actionArc_triggered();
            }
            else if (!moreCoordsNeeded(position))
            {
                if(drawingCoords[0] != drawingCoords[1])
                {
                    //Show a visual help to draw the arc from the circle
                    Item_arcDrawer* helper = new Item_arcDrawer(drawingWidth,
                                                                drawingColor,
                                                                drawingCoords[0],
                                                                drawingCoords[1]);
                    //The scene, not the current layer, must take care of this helper
                    scene->addItem(helper);
                    //Grab mouse in order to self-draw (respond to mouseMove)
                    helper->grabMouse();
                }
                else
                    ui->statusBar->showMessage("Same position selected twice!");
            }
            break;

        case SPIRAL:
            if (mG == nullptr)
            {
                //Show a visual help to draw the spiral
                Item_spiralDrawer* helper = new Item_spiralDrawer(drawingWidth,
                                                                  drawingColor,
                                                                  position);
                /*
                 * Unlike other figures, a spiral can be drawn with a variable number of
                 * clicks. Also the drawing ends directly by a signal from the helper,
                 * when the user presses [Enter] or reaches the maximum number of 4 clicks.
                 */
                connect(helper, &Item_spiralDrawer::endDrawing, this, &Gribouillot::finalizeSpiral);

                scene->addItem(helper);
                scene->setFocusItem(helper);//Focus is needed to catch [Enter] key
                helper->grabMouse();

            }
            moreDrawingTips();//move to next tip
            break;

        case NONE:
        default:
            break;
    }

    if (center != noCenter && ui->actionDrawCenter->isChecked())
        currentLayer->drawPoint(drawingColor, drawingWidth, center);

}


/**
 * @brief   Send the mouse clicks from scene AFTER default scene processing.
 * @details Important for item drawing starting with "if ( isOnlySelected(...) )"
 *          because he scene default processing is selecting a new item.
 */
void Gribouillot::newSceneClickPostSelect(QPointF position)
{
    //Some visual helps are 'self-drawing' items catching the mouse
    QGraphicsItem* mG = scene->mouseGrabberItem();

    switch (currentDrawing)
    {
        case LINE_FROMANGLE:
            if ( mG == nullptr ||
                 (mG->type() != POINT_ONRAIL && mG->type() != ARC_DRAWER))
            {
                if( isOnlySelected({SEGMENT, LINE}, 1) )
                {
                    /*
                     * A line or segment is selected by this click.
                     * Start visual help to find a starting point.
                     */
                    ui->zGraphicsView->setCursor(Qt::BlankCursor);
                    visualHelp_pointOnRail();//mG->type() == POINT_ONRAIL
                }
            }
            else if (mG->type() == POINT_ONRAIL)
            {
                ui->zGraphicsView->setCursor(drawingCursor);
                Item_pointOnRail* pOR =
                        qgraphicsitem_cast<Item_pointOnRail*>(mG);
                /*
                 * This click validates a starting point. Start visual
                 * help to define the angle of the new line.
                 */
                visualHelp_lineFromAngle(pOR);//mG->type() == ARC_DRAWER

            }
            else if (mG->type() == ARC_DRAWER)
            {
                //Finalize drawing
                Item_arcDrawer* aD = qgraphicsitem_cast<Item_arcDrawer*>(mG);
                currentLayer->drawLineFromSegment(drawingColor, drawingWidth,
                                                  aD->getAngledLine());
                /*
                 * Reset drawing. Helper item, which is the non-null mouseGrabber at
                 * this point, will be deleted soon by a call to setDrawingView().
                 */
                scene->clearSelection();//necessary so user can select another line.
                on_actionAngleLine_triggered();
            }
            break;


        case BISECTION:
            if( isOnlySelected({SEGMENT}, 1) )
            {
                //A segment is selected by this click. Draw bisection immediately.
                currentLayer->drawBisection(drawingColor, drawingWidth);

                //Reset selection (selected segment) and drawing tool
                scene->clearSelection();
                on_actionBisection_triggered();
            }
            break;


        case ARC_FROMCIRCLE:
            if ( mG == nullptr || mG->type() != ARC_DRAWER )
            {
                if ( isOnlySelected({CIRCLE},1) )
                    visualHelp_arcFromCircle();//mouse grabbed
            }
            else//mG == ARC_DRAWER
            {
                if(!moreCoordsNeeded(position))
                {
                    //Finalize drawing
                    Item_arcDrawer* aD = qgraphicsitem_cast<Item_arcDrawer*>(mG);
                    currentLayer->drawArcFromCircle(drawingColor, drawingWidth,
                                                    aD->getSourceCircle(),
                                                    aD->getArc());
                    /*
                     * Reset drawing. Helper item, which is the non-null mouseGrabber at
                     * this point, will be deleted soon by a call to setDrawingView().
                     */
                    on_actionArcFromCircle_triggered();
                }
            }
            //else wait for user to pick a circle OR for more Coordinates
            break;

        case NONE:
        default:
            break;
    }

}


/**
 * @brief   help to draw an arc from a circle
 */
void Gribouillot::visualHelp_arcFromCircle()
{
    //Get selected circle
    Item_circle* circle = qgraphicsitem_cast<Item_circle*>(
                                scene->selectedItems().last());

    //Show a visual help to draw the arc from the circle
    Item_arcDrawer* helper = new Item_arcDrawer(circle);
    //The scene, not the current layer, must take care of this helper
    scene->addItem(helper);
    //Grab mouse in order to self-draw (respond to mouseMove)
    helper->grabMouse();
    //Inform what to do next
    statusBar()->showMessage(drawingTips[0]);
}


/**
 * @brief   help to define a point on a line
 */
void Gribouillot::visualHelp_pointOnRail()
{
    //Get selected circle
    Item_segment* segment = dynamic_cast<Item_segment*>(
                                scene->selectedItems().last());

    //Show a visual help to draw the arc from the circle
    Item_pointOnRail* helper = new Item_pointOnRail(drawingWidth,
                                                    segment->line());
    //The scene, not the current layer, must take care of this helper
    scene->addItem(helper);
    //Grab mouse in order to self-draw (respond to mouseMove)
    helper->grabMouse();
    //Inform what to do next
    statusBar()->showMessage(drawingTips[1]);
}


/**
 * @brief   help to draw a line from another line and an angle
 */
void Gribouillot::visualHelp_lineFromAngle(Item_pointOnRail* pOR)
{
    qreal windowWidth = ui->zGraphicsView->width() / ui->zGraphicsView->getZoom();
    Item_arcDrawer* helper = new Item_arcDrawer(drawingWidth,
                                                drawingColor,
                                                windowWidth,
                                                pOR->scenePos(),
                                                pOR->getRail());
    //destroy pointOnRail item
    scene->removeItem(pOR); delete pOR;
    //Add the new visual help (arcDrawer) to scene
    scene->addItem(helper);
    helper->grabMouse();
    //Inform user what to do next
    statusBar()->showMessage(drawingTips[2]);
}


/**
 * @brief   Utility function to register user's clicks positions and
 *          display associated statusBar message when more than one
 *          position is need for a drawing.
 */
bool Gribouillot::moreCoordsNeeded(QPointF position)
{
    /*
     * Since we have one tip for each position needed, the call to
     * drawingTips.size() gives the total number of position needed.
     * The number of already registered positions is given by the size
     * of the QVector 'drawingCoords', which we use as an index.
     *
     * '+ 1' below becoz the first tip is always displayed before the
     * first call to this function in any case.
     */
    int tipIndex = drawingCoords.size() + 1;
    //qDebug() << tipIndex << drawingCoords.size() << drawingTips.size();

    if( tipIndex < drawingTips.size() )
    {
        drawingCoords.append(position);
        //show the next tip
        statusBar()->showMessage(drawingTips.at(tipIndex));
        return true;
    }
    else if (tipIndex == drawingTips.size())
    {
        drawingCoords.append(position);
        return false;
    }
    else//to avoid compiler warnings, should never be reached
        return false;
}


/**
 * @brief   Convenience function when clicked position is not important
 *          for the current drawing (for example: self-drawing items).
 */
bool Gribouillot::moreDrawingTips()
{
    return moreCoordsNeeded(QPointF());
}



/**
 * @brief   Utility function to find out if specific items are selected
 * @details if targetCount != -1 (default), an exact count is required
 */
bool Gribouillot::isOnlySelected(QVector<GribouillotItem> types, int targetCount)
{
    QList<QGraphicsItem*> selected = scene->selectedItems();
    int sceneCount = 0;

    foreach(QGraphicsItem* item, selected)
    {
        if ( types.contains((GribouillotItem)item->type()) )
            sceneCount++;
    }

    if (sceneCount == selected.size())
    {//All selected items are of correct types

        if(targetCount != -1)
        {//An exact count of items is required
            if( sceneCount == targetCount)
                return true;//ok
            else
                return false;
        }
        else
            return true;
    }
    else//Some selected items are not of the correct type
        return false;

}


/**
 * @brief   Use mouseMove events from scene to display GPS positions
 */
void Gribouillot::newMoveOnScene(QPointF position)
{
    if (ui->actionCursorSelect->isChecked() == true
        && gpsEnabled
        && ui->tabWidget->currentIndex() == 0 )
    {
            statusBar()->showMessage(tr(" GPS coordinates : ")
                                 + gpsDialog->getFix(position));
    }
}


/**
 * @brief   Delete selected items when the 'Del' key is pressed.
 */
void Gribouillot::keyDeleteFromScene()
{
    int count = currentLayer->deleteSelectedItems();
    if (count > 0)
        statusBar()->showMessage(QString::number(count) +
                                 tr(" item(s) deleted."));
}


/**
 * @brief   select next layer item when the 'Space' key is pressed.
 */
void Gribouillot::keySpaceFromScene()
{
    currentLayer->selectNextItem();
}


/**
 * @brief   Display different information about the currently selected items.
 * @details This function can be easily expanded, needs a separate file.
 */
void Gribouillot::sceneSelectionChanged() const
{
    QString statusMsg = statusBar()->currentMessage();
    int timeout = 0;
    QList<QGraphicsItem*> selectedItems = scene->selectedItems();
    int selectedCount = selectedItems.size();

    if (selectedCount == 1)
    {/*
      * When only one item is selected, display differents information
      * about it like GPS coordinates.
      */
        QGraphicsItem* item = selectedItems.first();

        if( item->type() == POINT_W )
        {
            //Item_point* point = qgraphicsitem_cast<Item_point *>(item);
            if(gpsEnabled)
                statusMsg = gpsDialog->getFix(item->pos());
        }
        else if( item->type() == LINE )
        {
            Item_line* line = qgraphicsitem_cast<Item_line *>(item);
            statusMsg = line->status();

        }
        else if( item->type() == SEGMENT )
        {
            Item_segment* segment = qgraphicsitem_cast<Item_segment *>(item);
            statusMsg = segment->status(ui->mapTabWidget->getKmPxScale());

        }
        else if( item->type() == CIRCLE )
        {
            Item_circle* circle = qgraphicsitem_cast<Item_circle *>(item);
            statusMsg = circle->status(ui->mapTabWidget->getKmPxScale());
            if(gpsEnabled)
                statusMsg += tr("    Center: ")+gpsDialog->getFix(circle->pos());

        }
        else if( item->type() == ARC )
        {
            Item_arc* arc = qgraphicsitem_cast<Item_arc *>(item);
            statusMsg = arc->status(ui->mapTabWidget->getKmPxScale());

        }
        else if( item->type() == PIXMAP )
        {
            Item_pixmap* pixmap = qgraphicsitem_cast<Item_pixmap *>(item);
            statusMsg = pixmap->status();
            timeout = 10000;//in millisecs

        }
        else if( item->type() == SPIRAL )
        {
            Item_spiral* spiral = qgraphicsitem_cast<Item_spiral *>(item);
            statusMsg = spiral->status(ui->mapTabWidget->getKmPxScale());

        }

    }
    else if (selectedCount > 1)
    {
    /*
     * When more than one item is selected, count items or display
     * some other informations in special cases.
     */
        int points, circles, lines, arcs, pixmaps;
        points = circles = lines = arcs = pixmaps = 0;

        //Variables which may be needed
        QList<Item_segment*> linesList;
        qreal xSum, ySum;
        xSum = ySum = 0;

        //Count points, lines, etc
        foreach(QGraphicsItem* item, selectedItems)
        {
            if( item->type() == POINT_W )
            {
                //May be needed to calculate the center of several points:
                Item_point* p = qgraphicsitem_cast<Item_point*>(item);
                xSum += p->x();
                ySum += p->y();

                ++points;
            }
            else if( item->type() == SEGMENT )
            {
                linesList << qgraphicsitem_cast<Item_segment*>(item);
                lines++;
            }
            else if( item->type() == LINE )
            {
                linesList << dynamic_cast<Item_segment*>(item);
                lines++;
            }
            //else if( item->type() == CIRCLE )...
        }


     /* Display specific informations related to counted items */

        if ( gpsEnabled && points == selectedCount)
        {//Only points are selected, calculate their center

            QPointF center(xSum/(qreal)selectedCount, ySum/(qreal)selectedCount);

            statusMsg = tr("Center of the ")+QString::number(selectedCount)
                        +tr(" selected points: ")
                        +gpsDialog->getFix(center);

        }
        else if ( lines == 2 && selectedCount == 2 )
        {//Calculate the angle between the 2 lines and their intersection point.

            QPointF iPoint;
            QLineF line1 = linesList.at(0)->line();
            QLineF line2 = linesList.at(1)->line();

            qreal angle = line1.angleTo(line2);
            if (angle > 180)
                angle = (qreal)360 - angle;//Human readable

            if(angle == 0 || angle == 180)
                statusMsg = tr("The 2 lines are strictly parallel.");

            else if(angle == 90)
                statusMsg = tr("The 2 lines are strictly perpendicular.");

            else
                statusMsg = tr("Angle between the 2 lines: ")
                            +QString::number(angle, 'g', 4)
                            +tr("/")
                            +QString::number((qreal)180-angle, 'g', 4)
                            +tr(" degrees");

            if ( gpsEnabled &&
                 line1.intersect(line2,&iPoint) != QLineF::NoIntersection)
                statusMsg += "    Intersect in "+gpsDialog->getFix(iPoint);

        }
        else
           statusMsg = QString::number(selectedCount)+"  selected items";

    }

    /*
     * Specific actions and statusMsg depending on current drawing.
     */
    if (currentDrawing == PARALLEL || currentDrawing == PERPENDICULAR)
        //At this point statusMsg can only be a line or segment status
        statusMsg = "( "+statusMsg+" selected) "+drawingTips[0];
    else if (currentDrawing == ARC_FROMCIRCLE)
        //Don't go in the way of the drawing messages
        statusMsg = statusBar()->currentMessage();

    statusBar()->showMessage(statusMsg , timeout);

}



/*********************** private slots ** toolbar actions ******************************/
/**
 * @brief   toggle to an Arrow cursor which allows to select scene items.
 */
void Gribouillot::on_actionCursorSelect_triggered()
{
    currentDrawing = NONE;
    statusBar()->clearMessage();
    //Items may have been disabled by drawing functions
    if(currentLayer != nullptr)
        currentLayer->enableItems();

    setCursor(Qt::ArrowCursor);
    ui->zGraphicsView->setCursor(Qt::ArrowCursor);//necessary
    ui->zGraphicsView->setDragMode(QGraphicsView::RubberBandDrag);

}


/**
 * @brief   toggle to an Hand cursor which allows to drag the view around.
 */
void Gribouillot::on_actionCursorDrag_triggered()
{
    currentDrawing = NONE;
    statusBar()->clearMessage();
    //Items may have been disabled by drawing functions
    if(currentLayer != nullptr)
        currentLayer->disableItems();

    setCursor(Qt::OpenHandCursor);
    ui->zGraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
}


/**
 * @brief   call the colorDialog to select the drawing color
 * @details can be called by user using the click or the 'c' key to change
 *          the color of the currently selected items.
 */
void Gribouillot::on_actionChooseColor_triggered()
{
    if(!scene->selectedItems().isEmpty())
    {
        const QColor color = QColorDialog::getColor(drawingColor,
                                                    this,
                                                    tr("Change the color of selected items."),
                                                    QColorDialog::DontUseNativeDialog);
        if (color.isValid())
        {
            foreach(QGraphicsItem* item, scene->selectedItems())
            {
                if(item->type() == POINT_W )
                {
                    Item_point* p = qgraphicsitem_cast<Item_point*>(item);
                    p->newBrush(color, p->rect().width());
                }
                else if (item->type() == LINE || item->type() == SEGMENT)
                {
                    Item_segment* s = dynamic_cast<Item_segment*>(item);
                    s->newPen(color, s->pen().width());
                }
                else if (item->type() == CIRCLE)
                {
                    Item_circle* c = qgraphicsitem_cast<Item_circle*>(item);
                    c->newPen(color, c->pen().width());
                }
                else if (item->type() == ARC)
                {
                    Item_arc* a = qgraphicsitem_cast<Item_arc*>(item);
                    a->newPen(color, a->pen().width());
                }

             }
        }
    }
    else
    {
        const QColor color = QColorDialog::getColor(drawingColor,
                                                    this,
                                                    tr("Pick a new drawing color"),
                                                    QColorDialog::DontUseNativeDialog);
        if (color.isValid())
        {
            setColorIcon(color);
            drawingColor = color;
        }
    }

}


/**
 * @brief   switch the drawing width to 3, 5, 7px
 * @details can be called by user using the click or the 't' key to change
 *          the width of the currently selected items.
 */
void Gribouillot::on_actionChooseWidth_triggered()
{
    if(!scene->selectedItems().isEmpty())
    {
        Dlg_penThickness dialog("Change the width of selected items.", drawingWidth);

        if (dialog.exec() == QDialog::Accepted)
        {
             int width = dialog.getThicknessValue();//Between 1 and 30

             foreach(QGraphicsItem* item, scene->selectedItems())
             {
                 if(item->type() == POINT_W )
                 {
                     Item_point* p = qgraphicsitem_cast<Item_point*>(item);
                     p->newBrush(p->brush().color(), width);
                 }
                 else if (item->type() == LINE || item->type() == SEGMENT)
                 {
                     Item_segment* l = dynamic_cast<Item_segment*>(item);
                     l->newPen(l->pen().color(), width);
                 }
                 else if (item->type() == CIRCLE)
                 {
                     Item_circle* c = qgraphicsitem_cast<Item_circle*>(item);
                     c->newPen(c->pen().color(), width);
                 }
                 else if (item->type() == ARC)
                 {
                     Item_arc* a = qgraphicsitem_cast<Item_arc*>(item);
                     a->newPen(a->pen().color(), width);
                 }

              }
        }
    }
    else
    {
        drawingWidth = (drawingWidth+2)%8;//drawingWidth is 1 or 3 or 5 or 7
        QString iconPath = ":/Resources/Icons/draw-width-"+QString::number(drawingWidth)+".png";
        ui->actionChooseWidth->setIcon(QIcon(QPixmap(iconPath)));

    }

}


/**
 * @brief   measuring tape in kilometers
 */
void Gribouillot::on_actionMeasureDistance_toggled(bool isChecked)
{
    if (isChecked)
    {
        currentDrawing = NONE;
        statusBar()->showMessage(tr("Measure a distance."));
        initScaleRuler();
        connect(scaleRuler, SIGNAL(newMeasure(qreal)),this, SLOT(measureDistance(qreal)));
    }
    else
        scene->removeItem(scaleRuler);
}

/**
 * @brief   shows the scale ruler measure in realtime in the statusBar.
 */
void Gribouillot::measureDistance(qreal distance)
{
    statusBar()->showMessage(tr("Distance is ")
                             + QString::number(distance*ui->mapTabWidget->getKmPxScale())
                             + " km");
}



/**
 * @brief   Dislay the weights of the points on scene
 */
void Gribouillot::on_actionPointWeight_toggled(bool isChecked)
{
    currentLayer->displayPointWeight(isChecked);
}


/**
 * @brief   Draw a point
 */
void Gribouillot::on_actionPoint_triggered()
{
    setDrawingView();
    currentDrawing = POINT_W;
    statusBar()->showMessage(tr("Pick up a position."));
}


/**
 * @brief   Draw a segment
 */
void Gribouillot::on_actionSegment_triggered()
{
    setDrawingView();
    currentDrawing = SEGMENT;
    drawingTips = {tr("Segment: pick up a first point."),
                   tr("Pick up the other end of the segment.")};

    statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw a "infinite" line
 */
void Gribouillot::on_actionLine_triggered()
{
    setDrawingView();
    currentDrawing = LINE;
    drawingTips = {tr("Line: pick up a first point."),
                   tr("Pick up a second point on the line.")};

    statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw the screen-horizontal line passing by a given point
 */
void Gribouillot::on_actionHorizontalLine_triggered()
{
    setDrawingView();
    currentDrawing = HORIZONTAL;
    statusBar()->showMessage(tr("Pick up a crossing point for the horizontal line."));
}


/**
 * @brief   Draw the screen-vertical line passing by a given point
 */
void Gribouillot::on_actionVerticalLine_triggered()
{
    setDrawingView();
    currentDrawing = VERTICAL;
    statusBar()->showMessage(tr("Pick up a crossing point for the vertical line."));
}


/**
 * @brief   Draw the parallel to a line passing by a given point
 */
void Gribouillot::on_actionParallel_triggered()
{
    setDrawingView();
    currentDrawing = PARALLEL;
    currentLayer->enableItems({SEGMENT, LINE});
    drawingTips= {tr("Parallel: select the reference line"),
                  tr("Pick up a crossing point")};

    if( isOnlySelected({SEGMENT, LINE}, 1) )
        statusBar()->showMessage(drawingTips[1]);
    else
        statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw the perpendicular to a line passing by a given point
 */
void Gribouillot::on_actionPerpendicular_triggered()
{
    setDrawingView();
    currentDrawing = PERPENDICULAR;
    currentLayer->enableItems({SEGMENT, LINE});
    drawingTips= {tr("Perpendicular: select the reference line"),
                  tr("Pick up a crossing point")};

    if( isOnlySelected({SEGMENT, LINE}, 1) )
        statusBar()->showMessage(drawingTips[1]);
    else
        statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw the bisection of 2 points
 */
void Gribouillot::on_actionBisection_triggered()
{
    setDrawingView();
    currentDrawing = BISECTION;
    currentLayer->enableItems({SEGMENT});
    drawingTips= {tr("Bisection: select a segment ")};

    if( isOnlySelected({SEGMENT}, 1) )
    {
        currentLayer->drawBisection(drawingColor, drawingWidth);
        //Reset selection (selected segment) and drawing
        scene->clearSelection();
        statusBar()->showMessage(drawingTips[0]);
    }
    else
        statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw the line intersecting another line at a certain angle
 */
void Gribouillot::on_actionAngleLine_triggered()
{
    setDrawingView();
    currentDrawing = LINE_FROMANGLE;
    currentLayer->enableItems({SEGMENT, LINE});
    drawingTips= {tr("Select the reference line"),
                  tr("Pick up the center of the angle"),
                  tr("Click to draw the line.")};

    if( isOnlySelected({SEGMENT, LINE}, 1) )
    {
        ui->zGraphicsView->setCursor(Qt::BlankCursor);
        visualHelp_pointOnRail();
        statusBar()->showMessage(drawingTips[1]);
    }
    else
        statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw a circle given its center and another point
 */
void Gribouillot::on_actionCircleCenterPoint_triggered()
{
    setDrawingView();
    currentDrawing = CIRCLE_FROMRADIUS;
    currentLayer->enableItems({SEGMENT});
    drawingTips = {tr("Circle: define a center."),
                   tr("Pick up a point of the circumference.")};

    if( isOnlySelected({SEGMENT}, 1) )
    {
        /**
         * A segment is selected as a radius, next click will give
         * the side of the segment on which to draw the circle.
         */
        statusBar()->showMessage(tr("Click around the side of the segment "
            "where the center should be."));
    }
    else
        statusBar()->showMessage(drawingTips[0]);


}


/**
 * @brief   Draw a circle given its center and radius
 */
void Gribouillot::on_actionCircleCenterRadius_triggered()
{
    setDrawingView();
    currentDrawing = CIRCLE_FROMRADIUSVALUE;
    statusBar()->showMessage(tr("Circle: define a center."));
}


/**
 * @brief   Draw a circle given one of its diameter
 */
void Gribouillot::on_actionCircleDiameter_triggered()
{
    setDrawingView();
    currentDrawing = CIRCLE_FROMDIAMETER;
    currentLayer->enableItems({SEGMENT});
    drawingTips = {tr("Circle: Define the starting point of the diameter."),
                   tr("Pick up the other end of the diameter.")};

    if( isOnlySelected({SEGMENT}, 1) )
    {//A segment is selected, take it as diameter and draw immediately
        currentLayer->drawCircleFromDiameter(drawingColor, drawingWidth);
        //Reset selection (selected diameter) and drawing
        scene->clearSelection();
        on_actionCircleDiameter_triggered();
    }
    else
        //The user must give the diameter's start and end points.
        statusBar()->showMessage(drawingTips[0]);
}


/**
 * @brief   Draw a circle given 3 of its points
 */
void Gribouillot::on_actionCircleTriangle_triggered()
{
    setDrawingView();
    currentDrawing = CIRCLE_FROMTRIANGLE;
    currentLayer->enableItems({POINT_W});
    drawingTips = {tr("Circle: Pick up a first point of the circumference."),
                   tr("Pick up a second point of the circumference."),
                   tr("Pick up a third point of the circumference.")};

    if( isOnlySelected({POINT_W}, 3) )
    {//Three points are selected, draw immediately
        currentLayer->drawCircleFromTriangle(drawingColor, drawingWidth);
        //Reset selection (selected points) and drawing
        scene->clearSelection();
        on_actionCircleTriangle_triggered();
    }
    else
        //The user must pick up 3 points
        statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw an arc by cutting out a circle
 * @details Will use a visual 'helper' item grabbing the mouse events
 */
void Gribouillot::on_actionArcFromCircle_triggered()
{
    setDrawingView();
    currentDrawing = ARC_FROMCIRCLE;
    currentLayer->enableItems({CIRCLE});
    drawingTips = {tr("Define the first boundary."),
                   tr("Define the second boundary.")};

    if( isOnlySelected({CIRCLE}, 1) )
       //Call helper immediately
        visualHelp_arcFromCircle();
    else
        statusBar()->showMessage(tr("Arc: Choose a circle to cut out."));
}



/**
 * @brief   Draw an arc from a center, a starting point and an angle
 */
void Gribouillot::on_actionArc_triggered()
{
    setDrawingView();
    currentDrawing = ARC;
    drawingTips = {tr("Define the center of the arc."),
                   tr("Define the starting point of the arc.")};

    statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw a 4 centers spiral, according to chosen options
 * @details Spiral options are set up in the corresponding menu.
 */
void Gribouillot::on_actionSpiral_triggered()
{
    setDrawingView();
    currentDrawing = SPIRAL;

    drawingTips = {tr("Define the first center."),
                   tr("Define the second center."),
                   tr("Define the third center."),
                   tr("Define the fourth center."),};

    statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Slot called by Item_spiralDrawer to finalize a spiral drawing.
 * @details The user has manually picked 4 centers or has pressed [Enter]
 *          after picking out 2 or 3 centers. Draw spiral according to
 *          options from the spiral dialog.
 */
void Gribouillot::finalizeSpiral(QString errorMsg)
{
    QGraphicsItem* mG = scene->mouseGrabberItem();
    Item_spiralDrawer* aD = qgraphicsitem_cast<Item_spiralDrawer*>(mG);

    if(errorMsg.isNull())
    {
        if(spiralDialog->isOneItem())
        {
            currentLayer->drawSpiral(drawingColor, drawingWidth, aD->getSpiral());
        }
        else
        {
            foreach(Item_arc* a, aD->getArcs())
                currentLayer->drawArc(drawingColor, drawingWidth, a);
        }

        if(spiralDialog->isBaseDisplayed())
        {
            if(spiralDialog->showBaseCentersOnly())
            {
                foreach(QPointF center, aD->getCenters())
                    currentLayer->drawPoint(drawingColor, drawingWidth, center);
            }
            else
                foreach(QLineF baseSide, aD->getBase())
                    currentLayer->drawSegment(drawingColor, drawingWidth, baseSide);
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Drawing error"), errorMsg, QMessageBox::Ok);
    }

    scene->removeItem(aD); delete aD;
    scene->update();//seems necessary!

    //Reset drawing
    on_actionSpiral_triggered();

}


/**
 * @brief   load a picture into current layer
 */
void Gribouillot::on_actionLoadPicture_triggered()
{
    setSelectionView();
    statusBar()->showMessage(tr("Importing a picture..."));
    currentLayer->drawPixmap(ui->actionCopy_pixmaps->isChecked());
}
