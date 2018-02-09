/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef GRIBOUILLOTSCENE_H
#define GRIBOUILLOTSCENE_H

#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QColor>
#include <QWidget>

class GribouillotScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GribouillotScene(QObject *parent = nullptr);
    ~GribouillotScene(){}

signals:
    void newMouseClickPreSelect(QPointF);
    void newMouseClickPostSelect(QPointF);
    void newMouseMove(QPointF);
    void keyDeletePressed();
    void keySpacePressed();
    void keyCPressed();
    void keyTPressed();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

};

#endif // GRIBOUILLOTSCENE_H
