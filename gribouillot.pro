QT += widgets xml

SOURCES += dlg_arcangle.cpp \
          dlg_autosave.cpp \
          dlg_centerofmass.cpp \
          dlg_changelayername.cpp \
          dlg_changemap.cpp \
          dlg_circleradius.cpp \
          dlg_importlayer.cpp \
          dlg_newgribproject.cpp \
          dlg_penthickness.cpp \
          dlg_pointweight.cpp \
          dlg_setupgps.cpp \
          dlg_spiral.cpp \
          gribouillot.cpp \
          gribouillot_toolbar.cpp \
          gribouillotlayer.cpp \
          gribouillotscene.cpp \
          gribouillottabwidget.cpp \
          item_arc.cpp \
          item_arcdrawer.cpp \
          item_circle.cpp \
          item_line.cpp \
          item_pixmap.cpp \
          item_point.cpp \
          item_pointonrail.cpp \
          item_scaleruler.cpp \
          item_segment.cpp \
          item_spiral.cpp \
          item_spiraldrawer.cpp \
          main.cpp \
          maptabwidget.cpp \
          minimap.cpp \
          qlinef58.cpp \
          scalebar.cpp \
          smartinserttabwidget.cpp \
          zoomablegraphicsview.cpp

HEADERS += dlg_arcangle.h \
           dlg_autosave.h \
           dlg_centerofmass.h \
           dlg_changelayername.h \
           dlg_changemap.h \
           dlg_circleradius.h \
           dlg_importlayer.h \
           dlg_newgribproject.h \
           dlg_penthickness.h \
           dlg_pointweight.h \
           dlg_setupgps.h \
           dlg_spiral.h \
           gribouillot.h \
           gribouillotitem.h \
           gribouillotlayer.h \
           gribouillotscene.h \
           gribouillottabwidget.h \
           item_arc.h \
           item_arcdrawer.h \
           item_circle.h \
           item_line.h \
           item_pixmap.h \
           item_point.h \
           item_pointonrail.h \
           item_scaleruler.h \
           item_segment.h \
           item_spiral.h \
           item_spiraldrawer.h \
           main.h \
           maptabwidget.h \
           minimap.h \
           qlinef58.h \
           scalebar.h \
           smartinserttabwidget.h \
           zoomablegraphicsview.h
FORMS += dlg_arcangle.ui \
dlg_autosave.ui \
dlg_centerofmass.ui \
dlg_changelayername.ui \
dlg_changemap.ui \
dlg_circleradius.ui \
dlg_importlayer.ui \
dlg_newgribproject.ui \
dlg_penthickness.ui \
dlg_pointweight.ui \
dlg_setupgps.ui \
dlg_spiral.ui \
gribouillot.ui \
gribouillotlayer.ui \
maptabwidget.ui

RESOURCES += gribouillot.qrc
TARGET = gribouillot

