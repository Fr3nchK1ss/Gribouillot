/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef DLG_CHANGEMAP_H
#define DLG_CHANGEMAP_H

#include <QDialog>

namespace Ui {
class dlg_changeMap;
}

class dlg_changeMap : public QDialog
{
    Q_OBJECT

public:
    explicit dlg_changeMap(QString mapName, QWidget *parent = 0);
    ~dlg_changeMap();

    QString getMapPath();

private slots:
    void on_mapFileBtt_clicked();

private:
    Ui::dlg_changeMap *ui;
};

#endif // DLG_CHANGEMAP_H
