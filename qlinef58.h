/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef QLINEF58_H
#define QLINEF58_H

#include <QLineF>

/**
 * @brief   The QLineF58 class is QLineF with center()
 * @details From v5.8 the Qt library implements the center() function for QLineF
 *          but this lib is not available for 32 bits architecture! This class
 *          makes center() available for all arch.
 */
class QLineF58 : public QLineF
{
public:
    QLineF58();
    QLineF58(const QPointF &p1, const QPointF &p2);
    QLineF58(qreal x1, qreal y1, qreal x2, qreal y2);

    /**
     * @brief Qt5.8 implementation of center()...
     */
    QPointF center()
    {
         return 0.5 * p1() + 0.5 * p2();
    }


    QLineF58& operator=(const QLineF & lineF);
};

#endif // QLINEF58_H
