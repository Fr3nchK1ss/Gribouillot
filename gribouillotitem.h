/**
 *  @brief
 *  @details
 *  @author    Reenigne from les Sans Hulotte forum
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef GRIBOUILLOTITEM_H
#define GRIBOUILLOTITEM_H

#include <QGraphicsItem>

enum GribouillotItem {
                      NONE,
                      POINT_W = QGraphicsItem::UserType,
                      POINT_ONRAIL = QGraphicsItem::UserType + 1,

                      LINE = QGraphicsItem::UserType + 11,
                      SEGMENT = QGraphicsItem::UserType + 12,
                      PARALLEL = QGraphicsItem::UserType + 13,
                      PERPENDICULAR = QGraphicsItem::UserType + 14,
                      BISECTION = QGraphicsItem::UserType + 15,
                      HORIZONTAL = QGraphicsItem::UserType + 16,
                      VERTICAL = QGraphicsItem::UserType + 17,
                      LINE_FROMANGLE = QGraphicsItem::UserType + 18,

                      CIRCLE = QGraphicsItem::UserType + 20,
                      CIRCLE_FROMCENTERPOINT = QGraphicsItem::UserType + 21,
                      CIRCLE_FROMRADIUS = QGraphicsItem::UserType + 22,
                      CIRCLE_FROMRADIUSVALUE = QGraphicsItem::UserType + 23,
                      CIRCLE_FROM2WAYPOINTS = QGraphicsItem::UserType + 24,
                      CIRCLE_FROMDIAMETER = QGraphicsItem::UserType + 25,
                      CIRCLE_FROMTRIANGLE = QGraphicsItem::UserType + 26,

                      ARC = QGraphicsItem::UserType + 30,
                      ARC_DRAWER = QGraphicsItem::UserType + 31,
                      ARC_FROMCIRCLE = QGraphicsItem::UserType + 32,

                      SPIRAL = QGraphicsItem::UserType + 40,
                      SPIRAL_DRAWER = QGraphicsItem::UserType + 41,

                      PIXMAP = QGraphicsItem::UserType + 50
                     };


#endif // GRIBOUILLOTITEM_H
