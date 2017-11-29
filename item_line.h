/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef ITEM_LINE_H
#define ITEM_LINE_H

#include "item_segment.h"

class Item_line : public Item_segment
{
public:
    //To identify each infinite line
    static int number;

    enum { Type = LINE };
    int type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return LINE;
    }
    explicit Item_line(QColor penColor, int penWidth, QPointF points[]);
    ~Item_line(){}

    void serialize2xml(QXmlStreamWriter* w);
    QString status();

private:
    int tag;

};

#endif // ITEM_LINE_H
