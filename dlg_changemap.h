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
    explicit dlg_changeMap(QWidget *parent = 0);
    ~dlg_changeMap();

private:
    Ui::dlg_changeMap *ui;
};

#endif // DLG_CHANGEMAP_H
