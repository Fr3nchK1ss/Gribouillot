/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef DLG_NEWGRIBPROJECT_H
#define DLG_NEWGRIBPROJECT_H

#include <QDialog>

namespace Ui {
class Dlg_newGribproject;
}

class Dlg_newGribproject : public QDialog
{
    Q_OBJECT

public:
    explicit Dlg_newGribproject(QWidget *parent = nullptr);
    ~Dlg_newGribproject();

    QString getProjectDirPath();
    QString getMapImagePath();
    bool copyMapImageChkBx();
    bool copyExternalPictures();

private:
    Ui::Dlg_newGribproject *ui;

private slots:
    void mapImagePath();
    void projectDirPath();
    void accept();
};

#endif // DLG_NEWGRIBPROJECT_H
