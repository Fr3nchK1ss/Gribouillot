/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QDebug>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QPixmap>
#include "item_pixmap.h"


Item_pixmap::Item_pixmap(QString path, QPointF position, qreal scale):
    QGraphicsPixmapItem(QPixmap(path))
{
    QPointF center(pixmap().width()/2, pixmap().height()/2);

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    imagePath = path;
    setPos(position);
    setScale(scale);

    setTransformOriginPoint(center);
}


void Item_pixmap::serialize2xml(QXmlStreamWriter *w)
{
    w->writeStartElement("Pixmap");
    w->writeAttribute("x", QString::number(x()));
    w->writeAttribute("y", QString::number(y()));
    w->writeAttribute("scale", QString::number(scale()));
    w->writeAttribute("imagePath", imagePath);
    w->writeEndElement();

}


/**
 * @brief   return a message to be displayed in the statusBar
 */
QString Item_pixmap::status()
{
    QString zoomFactor;
    if ( plusFactor == 1.0204)
        zoomFactor = "+++";
    else if ( plusFactor == 1.005)
        zoomFactor = "++";
    else
        zoomFactor = "+";

    return QObject::tr("File: ")+imagePath
           +QObject::tr("  Scale: ")+QString::number(scale(), 'f', 2)
           +QObject::tr("  Rotation: ")
           +QString::number(rotationAngle, 'f', 1)+"°"
           +QObject::tr("  Zoom factor: ")+zoomFactor
            ;

}


/********************** Protected functions *********************/
/**
 * @brief   Give keyboard focus to this item when it become selected.
 * \warning SAME CODE as in other Gribouillot Item_. If you change this code
 *          you probably need to change the code of other Item. Would need a mixin.
 */
QVariant Item_pixmap::itemChange(GraphicsItemChange change, const QVariant &value)
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
void Item_pixmap::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        switch ( event->key() )
        {
            case Qt::Key_Left:
                rotationAngle -= 10;
                setRotation(rotationAngle);
                break;

            case Qt::Key_Right:
                rotationAngle += 10;
                setRotation(rotationAngle);
                break;
        }
    }
    else
    {
        switch ( event->key() )
        {
            case Qt::Key_Left:
                rotationAngle -= 1;
                setRotation(rotationAngle);
                break;

            case Qt::Key_Right:
                rotationAngle += 1;
                setRotation(rotationAngle);
                break;

            case Qt::Key_Plus:
                setScale( scale() * plusFactor );
                break;

            case Qt::Key_Minus:
                setScale( scale() * minusFactor );
                break;

            case Qt::Key_3:
                plusFactor = 1.0204;
                minusFactor = 0.98;
                break;

            case Qt::Key_2:
                plusFactor = 1.005;//**.995 = 0.999975
                minusFactor = 0.995;
                break;

            case Qt::Key_1:
                plusFactor = 1.0005;//**.995 = 0.999999
                minusFactor = 0.9995;
                break;


            default:
                QGraphicsItem::keyPressEvent(event);

        }
    }

    //Force status bar update
    setSelected(false);
    setSelected(true);

}


/**
 * @brief   Implements scaling the pixmap when holding MAJ + mouse wheel
 */
void Item_pixmap::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier)
    {
        /**
         * Very smooth scale factors with 2 tenth of amplitude.
         * 0.98 * 0.98 = 0.9604
         * 0.9604 * 0.98 = 0.941192
         * 0.98 * 1.0204 = 0.999992
         */
        qreal factor = (event->delta() < 0 ? minusFactor : plusFactor);

        setScale( scale() * factor );

        event->accept();

    }
    else
        QGraphicsPixmapItem::wheelEvent(event);
}