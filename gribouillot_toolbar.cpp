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

#include <QtWidgets>
#include <QtDebug>

#include "ui_gribouillot.h"

#include "dlg_penthickness.h"
#include "gribouillot.h"


/*********************** private slots ** Scene events ****************************/
/**
 * @brief   Use mouseMove events from scene to display GPS positions
 */
void Gribouillot::newMoveOnScene(QPointF position)
{
    if (ui->actionCursorSelect->isChecked()
        && gpsEnabled
        && ui->gribTabWidget->currentIndex() == 0 )
    {
            statusBar()->showMessage(tr(" GPS coordinates : ")
                                 + gpsDialog->getFix(position));
    }
}


/**
 * @brief   Send the mouse clicks from scene BEFORE default scene processing, which
 *          is item selection.
 * @details Important for item drawing starting with "if ( scene->isOnlySelected(...) )"
 *          because a click on scene can select an item different from user's first
 *          choice.
 */
void Gribouillot::newSceneClickPreSelect(QPointF position)
{
    QPointF noCenter(-10000, -10000);
    QPointF center = noCenter;

    //Some visual helps catch the mouse
    QGraphicsItem* mG = scene->mouseGrabberItem();

    switch (currentDrawing)
    {
        case POINT_W :
            currentLayer->drawPoint(drawingColor, drawingWidth, position,
                                    ui->actionPointWeight->isChecked());
            break;

        case PARALLEL:
            if( scene->isOnlySelected({SEGMENT, LINE}, 1) )
                //A line or segment was previously chose by user
                currentLayer->drawParallel(drawingColor, drawingWidth, position,
                                           getSelectedLineF());

            /*don't reset tool so several parallels can be drawn in serie.*/
            break;

        case PERPENDICULAR:
            if( scene->isOnlySelected({SEGMENT, LINE}, 1) )
            {
                //A line or segment was previously chose by user
                Item_segment* selectedSegment = dynamic_cast<Item_segment*>(
                                                        scene->selectedItems().last());
                currentLayer->drawPerpendicular(drawingColor, drawingWidth,
                                                position, selectedSegment);

                selectedSegment->setSelected(true);//re-select user's line

                /*don't reset tool so that several perpendiculars can be drawn in serie.*/
            }
            break;

        case CIRCLE_FROMSELECTEDRADIUS:
            if( scene->isOnlySelected({SEGMENT}, 1) )
            {//A radius was previously selected. This click defines the side of the center.
                center =
                currentLayer->drawCircleFromRadius(drawingColor, drawingWidth, position,
                                                   getSelectedLineF());
                on_actionCircleSelectRadius_triggered();//reset drawing tool

            }
            break;

        case ARC:
            if (!moreCoordsNeeded(position))
            {
                Item_arcDrawer* aD = qgraphicsitem_cast<Item_arcDrawer*>(mG);
                currentLayer->drawArc(drawingColor, drawingWidth, aD->getArc());
                /*
                 * Reset drawing. Helper item, which is the non-null mouseGrabber at
                 * this point, will be deleted soon by a call to setDrawingView().
                 */
                on_actionArc_triggered();

            }
            else if( drawingCoords.size() == 2)
            {//that would be 2 coords out of 3 needed in total

                if( drawingCoords[0] != drawingCoords[1])
                {
                    //Show a visual help to draw the arc
                    Item_arcDrawer* helper = new Item_arcDrawer(drawingColor,
                                                                drawingWidth,
                                                                drawingCoords[0],
                                                                drawingCoords[1]);
                    scene->addItem(helper);
                    helper->grabMouse();//Grab mouse in order to self-draw (respond to MousePressEvent)
                }
                else
                    statusBar()->showMessage("Angle center and start points can not be the same!");
            }
            break;

        case SPIRAL:
            if (mG == nullptr)
            {
                //Show a visual help to draw the spiral
                Item_spiralDrawer* helper = new Item_spiralDrawer(drawingColor,
                                                                  drawingWidth);
                /*
                 * Unlike other figures, a spiral can be drawn with a variable number of
                 * clicks. Also the drawing ends directly by a signal from the helper,
                 * when the user presses [Enter], or after a maximum of 4 clicks.
                 */
                connect(helper, &Item_spiralDrawer::endDrawing, this, &Gribouillot::finalizeSpiral);

                scene->addItem(helper);
                scene->setFocusItem(helper);//Focus is needed to catch [Enter] key
                //Grab mouse in order to self-draw (respond to mouseMove)
                helper->grabMouse();

            }
            moreDrawingTips();//in any case move to next tip
            break;

        case NONE:
        default:
            break;
    }

    if (center != noCenter && ui->actionDrawCenter->isChecked())
        currentLayer->drawPoint(drawingColor, drawingWidth, center);

}


/**
 * @brief   Send the mouse clicks from scene AFTER default scene processing, which
 *          is item selection.
 * @details Necessary for drawings using adjustClickToPosition(), and different other
 *          cases.
 */
void Gribouillot::newSceneClickPostSelect(QPointF position)
{
    QPointF noCenter(-10000, -10000);
    QPointF center = noCenter;

    //Some visual helps are 'self-drawing' items catching the mouse
    QGraphicsItem* mG = scene->mouseGrabberItem();

    qreal scale = ui->mapTabWidget->getKmPxScale();
    QString scaleUnit = ui->mapTabWidget->getScaleUnit();
    if (scaleUnit == "m")
        scale*=1000;

    switch (currentDrawing)
    {
        case SEGMENT:
            if ( !moreCoordsNeeded(position) )
            {
                currentLayer->drawSegment(drawingColor, drawingWidth, drawingCoords);
                on_actionSegment_triggered();//reset drawing tool
            }
            break;

        case LINE:
            if ( !moreCoordsNeeded(position) )
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

        case LINE_FROMANGLE:
            if ( mG == nullptr ||
                 (mG->type() != POINT_ONRAIL && mG->type() != ARC_DRAWER))
            {
                if( scene->isOnlySelected({SEGMENT, LINE}, 1) )
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
                on_actionAngleLine_triggered();
            }
            break;

        case BISECTION:
            if( scene->isOnlySelected({SEGMENT}, 1) )
            {
                //A segment is selected by this click. Draw bisection immediately.
                currentLayer->drawBisection(drawingColor, drawingWidth, getSelectedLineF());
                on_actionBisection_triggered();//reset drawing tool
            }
            break;

        case PARALLEL:
        case PERPENDICULAR:
            if( scene->isOnlySelected({SEGMENT, LINE}, 1) )
                moreDrawingTips();//just display next hint message
            break;


        case CIRCLE_FROMCENTERPOINT:
            if( !moreCoordsNeeded (position ) )
            {
                center =
                currentLayer->drawCircleFromRadius(drawingColor, drawingWidth,
                                                   drawingCoords);
                on_actionCircleCenterPoint_triggered();//reset drawing tool
            }
            break;

        case CIRCLE_FROMSELECTEDRADIUS:
            if( scene->isOnlySelected({SEGMENT}, 1) )
                moreDrawingTips();//just display next hint message
            break;

        case CIRCLE_FROMRADIUSVALUE:
            center =
            currentLayer->drawCircleFromRadius(drawingColor, drawingWidth,
                                                position,//center coordinates
                                                scale, scaleUnit);
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

        case CIRCLE_FROMSELECTEDDIAMETER:
            if( scene->isOnlySelected({SEGMENT}, 1) )
            {
                //A diameter is selected by this click. Draw the circle immediately.
                currentLayer->drawCircleFromDiameter(drawingColor, drawingWidth,
                                                     getSelectedLineF());

                //Reset drawing tool
                on_actionCircleSelectDiameter_triggered();
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

        case ARC_FROMCIRCLE:
            if ( mG == nullptr || mG->type() != ARC_DRAWER )
            {
                if ( scene->isOnlySelected({CIRCLE},1) )
                    visualHelp_arcFromCircle();//start new ARC_DRAWER
                //else wait for user to pick a circle
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
                     * this point, will soon be deleted by a call to setDrawingView().
                     */
                    on_actionArcFromCircle_triggered();
                }
                //else wait for more coordinates
            }
            break;

        case PROTRACTOR:
            if (mG != nullptr && mG->type() == ARC_DRAWER)
            {
                //delete current protractor an dcreate a new one
                on_actionMeasureAngle_triggered();
            }
            else
            {
                Item_arcDrawer* protractor = nullptr;
                qreal windowWidth = ui->zGraphicsView->width() / ui->zGraphicsView->getZoom();

                if( scene->isOnlySelected({SEGMENT, LINE}, 1) && drawingCoords.isEmpty() )
                {//User clicked on a segment as the first side of the angle

                    //adjust click onto segment
                    QLineF s = getSelectedLineF();
                    QLineF n = s.normalVector();
                    n.translate(position-n.p1());

                    if ( n.intersects(s, &position) != QLineF::NoIntersection )
                        protractor = new Item_arcDrawer(windowWidth, position, s);

                }
                else if (!moreCoordsNeeded(position))
                {//User defines the first side of the angle with 2 clicks

                    QLineF angleSide1(drawingCoords[0], drawingCoords[1]);
                    protractor = new Item_arcDrawer(windowWidth, drawingCoords[0], angleSide1);
                }

                if (protractor != nullptr)
                {
                    connect(protractor, &Item_arcDrawer::newMeasure, this, &Gribouillot::measureAngle);
                    scene->addItem(protractor);
                    protractor->grabMouse();//from here mG != nullptr
                }
            }



        case NONE:
        default:
            break;
    }

    if (center != noCenter && ui->actionDrawCenter->isChecked())
        currentLayer->drawPoint(drawingColor, drawingWidth, center);

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
    //qDebug() << "position: " << position;
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
    else
        //This else can be reach calling moreDrawingTips()
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
 * @brief   Extract the lineF from selected item_segment or item_line
 * @details Only called after an if(inOnlySelected({SEGMENT, LINE})) to ensure
 *          that line != null
 */
QLineF58 Gribouillot::getSelectedLineF()
{
    QLineF58 selectedLineF = QLineF58();

    Item_segment* selected = dynamic_cast<Item_segment*>(scene->selectedItems().last());
    if (selected != nullptr)
        selectedLineF = static_cast<QLineF58>(selected->line());

    return selectedLineF;
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
    else if (count == 0)
        userCanNotDo("delete");

}


/**
 * @brief   select next layer item when the 'Space' key is pressed.
 */
void Gribouillot::keySpaceFromScene()
{
    currentLayer->selectNextItem();
}


/**
 * @brief   Change the drawing thickness of an item.
 */
void Gribouillot::keyTFromScene()
{
    if(!currentLayer->selectedItems().isEmpty())
    {
        Dlg_penThickness dialog("Change the width of selected items.", drawingWidth);

        if (dialog.exec() == QDialog::Accepted)
        {
            int width = dialog.getThicknessValue();//Between 1 and 30

            foreach(QGraphicsItem* item, currentLayer->selectedItems())
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
                else if (item->type() == SPIRAL)
                {
                 Item_spiral* s = qgraphicsitem_cast<Item_spiral*>(item);
                 s->newPen(s->pen().color(), width);
                }
            }
        }
    }
    else
        userCanNotDo("thickness change");
}


/**
 * @brief Reset any drawing tool when Escape key is pressed.
 */
void Gribouillot::keyEscFromScene()
{
    //NOTE: one-click tools don't need reset!
    //qDebug() << "reset!";
    switch (currentDrawing)
    {
        case LINE:
            on_actionLine_triggered();
            break;
        case SEGMENT:
            on_actionSegment_triggered();
            break;
        case PARALLEL:
            on_actionParallel_triggered();
            break;
        case PERPENDICULAR:
            on_actionPerpendicular_triggered();
            break;
        case LINE_FROMANGLE:
            on_actionAngleLine_triggered();
            break;
        case CIRCLE_FROMCENTERPOINT:
            on_actionCircleCenterPoint_triggered();
            break;
        case CIRCLE_FROMSELECTEDRADIUS:
            on_actionCircleSelectRadius_triggered();
            break;
        case CIRCLE_FROMDIAMETER:
            on_actionCircleDiameter_triggered();
            break;
        case CIRCLE_FROMTRIANGLE:
            on_actionCircleTriangle_triggered();
            break;
        case ARC:
            on_actionArc_triggered();
            break;
        case ARC_FROMCIRCLE:
            on_actionArcFromCircle_triggered();
            break;
        case PROTRACTOR:
            on_actionMeasureAngle_triggered();
            break;
        case SPIRAL:
            on_actionSpiral_triggered();
            break;
        default:
            ;

    }


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

    //Get scale info to display distances in status bar
    qreal scale = ui->mapTabWidget->getKmPxScale();
    QString scaleUnit = ui->mapTabWidget->getScaleUnit();
    if (scaleUnit == "m")
        scale*=1000;

    if (currentDrawing == NONE)
    {//Don't interfere with drawing tips

    if (selectedCount == 1 )
    {
        /*
        * When only one item is selected, display relevant information
        * about it like GPS position, length, etc.
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
            statusMsg = segment->status(scale, scaleUnit);

        }
        else if( item->type() == CIRCLE )
        {
            Item_circle* circle = qgraphicsitem_cast<Item_circle *>(item);
            statusMsg = circle->status(scale, scaleUnit);
            if(gpsEnabled)
                statusMsg += tr("    Center: ")+gpsDialog->getFix(circle->pos());

        }
        else if( item->type() == ARC )
        {
            Item_arc* arc = qgraphicsitem_cast<Item_arc *>(item);
            statusMsg = arc->status(scale, scaleUnit);

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
            statusMsg = spiral->status(scale, scaleUnit);

        }

    }
    else if (selectedCount > 1)
    {
        /*
         * When more than one item is selected, count items or display
         * some other informations in special cases.
         */
        QList<Item_point*> pointsList;
        QList<Item_segment*> linesList;
        QList<Item_circle*> circlesList;
        //...

        //Count points, lines, etc
        foreach(QGraphicsItem* item, selectedItems)
        {
            if( item->type() == POINT_W )
                pointsList << qgraphicsitem_cast<Item_point*>(item);

            else if( item->type() == SEGMENT || item->type() == LINE )
                linesList << dynamic_cast<Item_segment*>(item);

            else if( item->type() == CIRCLE )
                circlesList << qgraphicsitem_cast<Item_circle*>(item);

            //else if (item->type() == ...
        }

        if ( selectedCount == 2 && linesList.count() == 2 )
        {//Show the angle between the 2 lines and their intersection point.

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
                            +tr("° / ")
                            +QString::number((qreal)180-angle, 'g', 4)
                            +tr("°");

            if ( gpsEnabled &&
                 line1.intersects(line2,&iPoint) != QLineF::NoIntersection)
                statusMsg += "    Intersect in "+gpsDialog->getFix(iPoint);

        }
        else if ( selectedCount == 2 && linesList.count() == 1 && pointsList.count() == 1 )
        {//Show the distance between a point and a line

            QPointF p = pointsList.at(0)->scenePos();
            QLineF l = linesList.at(0)->line();
            QPointF p1 = l.p1();
            QPointF p2 = l.p2();

            /*
             * Formula is d = | (y2 - y1)*x - (x2 - x1)*y + x2y1 - y2x1 |
             *                -------------------------------------------
             *                      V[ (y2-y1)² - (x2-x1)² ]
             */
            qreal distance = qAbs( (p2.y()-p1.y()) * p.x() - (p2.x()-p1.x()) * p.y() + p2.x()*p1.y() - p2.y()*p1.x() )
                                    / qSqrt( qPow((p2.y() - p1.y()), 2) + qPow((p2.x() - p1.x()), 2) );

            statusMsg = tr("Orthogonal distance between point and line: ")
                        +QString::number(distance*scale, 'f', 1)
                        +scaleUnit;

        }
        else if ( selectedCount == 2 && pointsList.count() == 1 && circlesList.count() == 1 )
        {//Show the distance between a point and a circle

            QPointF point = pointsList.at(0)->scenePos();
            Item_circle* circle = circlesList.at(0);
            QPointF center = circle->scenePos();

            qreal distance2center = QLineF(point, center).length();
            qreal distance2border = distance2center - circle->getRadius();

            statusMsg = tr("Distance from selected point to circle center: ")
                        +QString::number(distance2center*scale, 'f', 1)
                        +scaleUnit
                        +tr("; to circle border: ")
                        +QString::number(distance2border*scale, 'f', 1)
                        +scaleUnit;
        }
        else
           statusMsg = QString::number(selectedCount)+"  selected items";

    }//selectedCount > 1
    }//currentDrawing == NONE

    statusBar()->showMessage(statusMsg , timeout);

}


/**
 * @brief   help to draw an arc from a circle
 */
void Gribouillot::visualHelp_arcFromCircle()
{
    //Get selected circle
    Item_circle* circle = qgraphicsitem_cast<Item_circle*>(
                                scene->selectedItems().last());

    //Disable any new circle selection
    scene->enableOnlyItems({});
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
    //Show a visual help to draw the arc from the circle
    Item_pointOnRail* helper = new Item_pointOnRail(drawingWidth,
                                                    getSelectedLineF());
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
    Item_arcDrawer* helper = new Item_arcDrawer(drawingColor,
                                                drawingWidth,
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
 * @brief   Send message to user when 'operation' can not be done.
 */
void Gribouillot::userCanNotDo(QString operation)
{
    if (scene->selectedItems().count() > 0)
        statusBar()->showMessage(tr("The selected items belong to a different layer! Can not perform ")
                                    +operation+".");
    else
        statusBar()->showMessage(tr("No item to ")+operation+".");

}



/*********************** private slots ** toolbar actions ******************************/
/**
 * @brief   toggle to an Arrow cursor which allows to select scene items.
 */
void Gribouillot::on_actionCursorSelect_triggered()
{
    statusBar()->clearMessage();
    clearView();
    currentDrawing = NONE;

    if (currentLayer != nullptr)
        setWorkingLayer(currentLayer);

    setCursor(Qt::ArrowCursor);
    ui->zGraphicsView->setCursor(Qt::ArrowCursor);
    ui->zGraphicsView->setDragMode(QGraphicsView::RubberBandDrag);
}


/**
 * @brief   toggle to an Hand cursor which allows to drag the view around.
 */
void Gribouillot::on_actionCursorDrag_triggered()
{
    statusBar()->clearMessage();
    clearView();
    currentDrawing = NONE;

    //Items can not be selected in Drag mode
    foreach(QGraphicsItem* item, scene->items())
        item->setEnabled(false);

    setCursor(Qt::OpenHandCursor);
    ui->zGraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

}


/**
 * @brief   call the colorDialog to select the drawing color
 * @details can be called by a user click in toolbar or suing the 'c' key to change
 *          the color of the currently selected items.
 */
void Gribouillot::on_actionChooseColor_triggered()
{
    //if at least one item from the current layer is selected, change color of selected items.
    if(!currentLayer->selectedItems().isEmpty())
    {
        const QColor color = QColorDialog::getColor(drawingColor,
                                                    this,
                                                    tr("Change the color of selected items."),
                                                    QColorDialog::DontUseNativeDialog);
        if (color.isValid())
        {
            foreach(QGraphicsItem* item, currentLayer->selectedItems())
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
                else if (item->type() == SPIRAL)
                {
                    Item_spiral* s = qgraphicsitem_cast<Item_spiral*>(item);
                    s->newPen(color, s->pen().width());

                }

             }
        }
    }
    //else change the color for future drawings.
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
 */
void Gribouillot::on_actionChooseWidth_triggered()
{
    drawingWidth = (drawingWidth+2)%8;//drawingWidth is 1 or 3 or 5 or 7
    QString iconPath = ":/Resources/Icons/draw-width-"+QString::number(drawingWidth)+".png";
    ui->actionChooseWidth->setIcon(QIcon(QPixmap(iconPath)));

}


/**
 * @brief   measuring tape in kilometers
 */
void Gribouillot::on_actionMeasureDistance_triggered()
{
        setDrawingView();
        currentDrawing = SCALERULER;

        Item_scaleRuler* scaleRuler = new Item_scaleRuler();
        connect (scaleRuler, &Item_scaleRuler::newMeasure,this, &Gribouillot::measureDistance);
        scene->addItem(scaleRuler);
        scaleRuler->grabMouse();

        statusBar()->showMessage(tr("Measure a distance."));

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
 * @brief Mesure an angle in degree/radians
 */
void Gribouillot::on_actionMeasureAngle_triggered()
{
    setDrawingView();
    currentDrawing = PROTRACTOR;

    drawingTips = {tr("Select the center of your angle or one side of the angle"),
                   tr("Select the starting point of the arc")};

    statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   shows the protractor angle in realtime in the statusBar.
 */
void Gribouillot::measureAngle(qreal angle)
{
    //Compute Angle in radians too
    qreal radians = M_PI*angle/180.0;

    statusBar()->showMessage(tr("Angle is ")
                             +QString::number(angle, 'g', 4)+ "° / "
                             +QString::number(radians, 'g', 4)+"rad");

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
    scene->clearSelection();
    currentDrawing = SEGMENT;
    drawingTips = {tr("Segment: pick up a first end."),
                   tr("Segment: pick up the other end.")};

    statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw a "infinite" line
 */
void Gribouillot::on_actionLine_triggered()
{
    setDrawingView();
    scene->clearSelection();
    currentDrawing = LINE;
    drawingTips = {tr("Line: pick up a first point."),
                   tr("Line: pick up a second point.")};

    statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw the screen-horizontal line passing by a given point
 */
void Gribouillot::on_actionHorizontalLine_triggered()
{
    setDrawingView();
    scene->clearSelection();
    currentDrawing = HORIZONTAL;

    statusBar()->showMessage(tr("Horizontal line: pick up a waypoint."));
}


/**
 * @brief   Draw the screen-vertical line passing by a given point
 */
void Gribouillot::on_actionVerticalLine_triggered()
{
    setDrawingView();
    scene->clearSelection();
    currentDrawing = VERTICAL;

    statusBar()->showMessage(tr("Vertical line: pick up a waypoint."));
}


/**
 * @brief   Draw the parallel to a line passing by a given point
 */
void Gribouillot::on_actionParallel_triggered()
{
    setDrawingView();
    scene->clearSelection();
    currentDrawing = PARALLEL;
    scene->enableOnlyItems({SEGMENT, LINE});
    drawingTips= {tr("Parallel: select the reference line."),
                  tr("Parallel: pick up a way point.")};

    statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw the perpendicular to a line passing by a given point
 */
void Gribouillot::on_actionPerpendicular_triggered()
{
    setDrawingView();
    scene->clearSelection();
    currentDrawing = PERPENDICULAR;
    scene->enableOnlyItems({SEGMENT, LINE});
    drawingTips= {tr("Perpendicular: select the reference line."),
                  tr("Perpendicular: pick up a way point.")};

    statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw the bisection of 2 points
 */
void Gribouillot::on_actionBisection_triggered()
{
    setDrawingView();
    scene->clearSelection();
    currentDrawing = BISECTION;
    scene->enableOnlyItems({SEGMENT});

    statusBar()->showMessage(tr("Bisection: select a segment."));

}


/**
 * @brief   Draw the line intersecting another line at a certain angle
 */
void Gribouillot::on_actionAngleLine_triggered()
{
    setDrawingView();
    scene->clearSelection();
    currentDrawing = LINE_FROMANGLE;
    scene->enableOnlyItems({SEGMENT, LINE});
    drawingTips= {tr("Angle: select the reference line"),
                  tr("Angle: pick up the center of the angle"),
                  tr("Angle: click to draw.")};

    statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw a circle given its center and another point
 */
void Gribouillot::on_actionCircleCenterPoint_triggered()
{
    setDrawingView();
    scene->clearSelection();
    currentDrawing = CIRCLE_FROMCENTERPOINT;
    drawingTips = {tr("Circle: define a center."),
                   tr("Circle: pick up a point of the circumference.")};

    statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw a circle given a selected radius
 */
void Gribouillot::on_actionCircleSelectRadius_triggered()
{
    setDrawingView();
    scene->clearSelection();
    currentDrawing = CIRCLE_FROMSELECTEDRADIUS;
    scene->enableOnlyItems({SEGMENT});
    drawingTips = {tr("Circle: select a segment."),
                   tr("Circle: click around the end where the center should be.")};

    statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw a circle given a center and a radius value
 */
void Gribouillot::on_actionCircleRadiusValue_triggered()
{
    setDrawingView();
    scene->clearSelection();
    currentDrawing = CIRCLE_FROMRADIUSVALUE;

    statusBar()->showMessage(tr("Circle: define a center."));
}



/**
 * @brief   Draw a circle given 2 points as a diameter
 */
void Gribouillot::on_actionCircleDiameter_triggered()
{
    setDrawingView();
    scene->clearSelection();
    currentDrawing = CIRCLE_FROMDIAMETER;
    drawingTips = {tr("Circle: define the first end of the diameter."),
                   tr("Circle: define the other end of the diameter.")};

    statusBar()->showMessage(drawingTips[0]);
}


/**
 * @brief   Draw a circle given a selected diameter
 */
void Gribouillot::on_actionCircleSelectDiameter_triggered()
{
    setDrawingView();
    scene->clearSelection();
    currentDrawing = CIRCLE_FROMSELECTEDDIAMETER;
    scene->enableOnlyItems({SEGMENT});

    statusBar()->showMessage(tr("Circle: select a segment as diameter."));

}


/**
 * @brief   Draw a circle given 3 of its points
 */
void Gribouillot::on_actionCircleTriangle_triggered()
{
    setDrawingView();
    currentDrawing = CIRCLE_FROMTRIANGLE;
    drawingTips = {tr("Circle: Pick up a first point of the circumference."),
                   tr("Circle: Pick up a second point of the circumference."),
                   tr("Circle: Pick up a third point of the circumference.")};

    if( scene->isOnlySelected({POINT_W}, 3) )
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
    scene->enableOnlyItems({CIRCLE});
    drawingTips = {tr("Arc: define the first boundary."),
                   tr("Arc: define the second boundary.")};

    if( scene->isOnlySelected({CIRCLE}, 1) )
        visualHelp_arcFromCircle();//Call helper immediately
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
    drawingTips = {tr("Arc: define the center of the arc."),
                   tr("Arc: define the starting point of the arc."),
                   tr("Arc: define the angle of the arc.")};

    statusBar()->showMessage(drawingTips[0]);

}


/**
 * @brief   Draw a 4 centers spiral, according to chosen options
 * @details Spiral options are set up in the corresponding menu.
 */
void Gribouillot::on_actionSpiral_triggered()
{
    setDrawingView();
    scene->clearSelection();
    currentDrawing = SPIRAL;
    drawingTips = {tr("Spiral: define the first center."),
                   tr("Spiral: define the second center."),
                   tr("Spiral: define the third center (or click [Enter] for a square base)"),
                   tr("Spiral: define the fourth center (use [Maj] for perpendicular sides)."),};

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
            currentLayer->drawSpiral(drawingColor, drawingWidth, aD->getCenters());
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
        //drawing aborted for mathematical reasons / wrong user input
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
