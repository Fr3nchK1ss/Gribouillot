#ifndef GRIBOUILLOTTABWIDGET_H
#define GRIBOUILLOTTABWIDGET_H

#include <QObject>
#include <QGraphicsSceneEvent>

#include "smartinserttabwidget.h"
#include "gribouillotlayer.h"

class GribouillotTabWidget : public SmartInsertTabWidget
{
    Q_OBJECT

public:
    explicit GribouillotTabWidget(QWidget* parent = nullptr);

    void reset();
    void setMapTab(QString mapPath, QString mapName);
    int insertAndDisplayTab(int index, GribouillotLayer *layer, const QString &label);

};

#endif // GRIBOUILLOTTABWIDGET_H
