/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QDebug>

#include "qlinef58.h"

QLineF58::QLineF58() :
    QLineF()
{}

QLineF58::QLineF58(const QPointF &p1, const QPointF &p2) :
    QLineF(p1, p2)
{}


QLineF58::QLineF58(qreal x1, qreal y1, qreal x2, qreal y2) :
    QLineF(x1, y1, x2, y2)
{}


/**
 * @brief Overload necessary to initialize a QLineF58 fro, for instance,
 *        a call to Item_segment->line()
 */
QLineF58& QLineF58::operator=(const QLineF &lineF)
{
    setPoints(lineF.p1(), lineF.p2());

    return *this;
}
