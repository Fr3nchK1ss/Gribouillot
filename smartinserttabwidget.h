/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef SMARTINSERTTABWIDGET_H
#define SMARTINSERTTABWIDGET_H

#include <QTabWidget>

class SmartInsertTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit SmartInsertTabWidget(QWidget *parent);


protected:
    void tabInserted(int index) Q_DECL_OVERRIDE;


private slots:
    void handleTabBug(int newTabIndex);

private:
    int bugTabIndex;

};

#endif // SMARTINSERTTABWIDGET_H
