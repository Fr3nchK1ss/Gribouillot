/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef DLG_AUTOSAVE_H
#define DLG_AUTOSAVE_H

#include <QDialog>

namespace Ui {
class Dlg_autosave;
}

class Dlg_autosave : public QDialog
{
    Q_OBJECT

public:
    explicit Dlg_autosave(QWidget *parent, int oldTimeout);
    ~Dlg_autosave();

    int getTimeoutValue();

private:
    Ui::Dlg_autosave *ui;
};

#endif // DLG_AUTOSAVE_H
