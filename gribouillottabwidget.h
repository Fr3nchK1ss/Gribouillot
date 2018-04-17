#ifndef GRIBOUILLOTTABWIDGET_H
#define GRIBOUILLOTTABWIDGET_H

#include <QObject>

#include "smartinserttabwidget.h"
#include "gribouillotlayer.h"

class GribouillotTabWidget : public SmartInsertTabWidget
{
    Q_OBJECT

public:
    explicit GribouillotTabWidget(QWidget* parent);

    int insertAndDisplayTab(int index, GribouillotLayer *layer, const QString &label);

};

#endif // GRIBOUILLOTTABWIDGET_H
