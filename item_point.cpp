/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QDebug>
#include <QGraphicsView>
#include <QLabel>
#include <QPainter>

#include "dlg_pointweight.h"
#include "dlg_setupgps.h"
#include "item_point.h"

Item_point::Item_point(QColor brushColor, qreal penWidth, QPointF position,
                       int weight):
    /**
     * x and y of QGraphicsEllipseItem() defines the top-left corner of the
     * bounding Rect in the own item reference system.
     */
    QGraphicsEllipseItem()
{
    /* Point is "cosmetic" in its own way */
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    //Focusable: to respond to user's keyboard activity
    setFlag(QGraphicsItem::ItemIsFocusable);

    //outline color
    setPen(QPen(Qt::black));
    newBrush(brushColor, penWidth);
    //position
    setPos(position.x(), position.y());


    /* Create a weight label associated to this point */
    QLabel *label = new QLabel(QString::number(0));
    weightLabel = new QGraphicsProxyWidget(this);
    QPalette palette;
    QFont font;

    /* Setting and displaying point weight */
    palette.setBrush(QPalette::Window, Qt::transparent);
    palette.setBrush(QPalette::WindowText, brushColor);
    label->setPalette(palette);
    font.setPixelSize(20);
    font.setBold(true);
    label->setFont(font);
    label->move(7,-2);
    weightLabel->setWidget(label);
    setWeight(weight);
    showWeight(false);//weight label hidden by default
}


void Item_point::newBrush(QColor brushColor, int penWidth)
{
    //filling color
    setBrush(QBrush(brushColor));
    //Minimum sensible width for a point is 3px
    if ( penWidth < 3)
        penWidth = 3;

    setRect(-penWidth/2.0, -penWidth/2.0, penWidth, penWidth);

}



/**
 * @brief   set a new weight for this point, stored as an item data
 */
int Item_point::getWeight()
{
    QLabel *label = dynamic_cast<QLabel*>(weightLabel->widget());

    return label->text().toInt();

}

/**
 * @brief   set a new weight for this point, stored as an item data
 */
void Item_point::setWeight(int w)
{
    QLabel *label = dynamic_cast<QLabel*>(weightLabel->widget());
    label->setText(QString::number(w));
    label->adjustSize();
}



/**
 * @brief   toggle the display of the point weight
 */
void Item_point::showWeight(bool isDisplayed)
{
    if(isDisplayed && getWeight() != 0)
        weightLabel->show();
    else
        weightLabel->hide();
}


void Item_point::serialize2xml(QXmlStreamWriter* w)
{
    w->writeStartElement("Point");
    w->writeAttribute("x", QString::number(x()));
    w->writeAttribute("y", QString::number(y()));
    w->writeAttribute("color", brush().color().name());
    w->writeAttribute("penWidth", QString::number(rect().width()));
    w->writeAttribute("weight", QString::number(getWeight()));
    w->writeEndElement();

}


/********************** Protected functions OVERRIDE *********************/
/**
 * @brief   Give keyboard focus to this item when it become selected.
 * \warning SAME CODE as in other Gribouillot Item_. If you change this code
 *          you probably need to change the code of other Item. Would need a mixin.
 */
QVariant Item_point::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged)
    {
        setFocus();
        //qDebug() << "change in focus " << this;
        return QVariant();//unused anyway
    }
    //Else, other changes
    return QGraphicsItem::itemChange(change, value);

}


/**
 * @brief   React to keyboard input. Change scale.
 * @details The item needs to have focus to receive keyboard input,
 *          see itemChange().
 */
void Item_point::keyPressEvent(QKeyEvent *event)
{
    switch ( event->key() )
    {
        case Qt::Key_W:
            {
                //static_cast blabla is to get the dialog centered in the view
                Dlg_pointWeight dialog(static_cast<QWidget *>(this->scene()->views().at(0)), getWeight() );
                if (dialog.exec() == QDialog::Accepted)
                    setWeight(dialog.getWeightValue());

            }
            break;

        default:
            QGraphicsItem::keyPressEvent(event);

    }
}
