/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef DLG_SETUPGPS_H
#define DLG_SETUPGPS_H

#include <QButtonGroup>
#include <QDialog>
#include <QShowEvent>
#include <QHideEvent>
#include <QGraphicsScene>
#include <QPointF>
#include <QString>
#include <QVariant>
#include <QList>

namespace Ui {
class Dlg_setupGps;
}

class Dlg_setupGps : public QDialog
{
    Q_OBJECT
public:
    explicit Dlg_setupGps(QGraphicsScene* scene, QWidget *parent);
    ~Dlg_setupGps();

    QString getFix(QPointF scenePos);

private:
    Ui::Dlg_setupGps *ui;

    void getUserFriendlyCoordinate(QString type, int l,
                        int &degree, int &min, int &sec, QString &cardinal);
    QString getUserFriendlyCoordinate(QString type, int l);

signals:
    void disableGPS();

private slots:
    void on_disablePshBtt_clicked();

};

#endif // DLG_SETUPGPS_H
