/**
 *  @brief     A software to work on maps and... find treasures!
 *  @details   This software allows for drawing on maps using layers.
 *             Mathematical tools are provided for measuring, drawing lines & circles, etc
 *  @author    Ludovic A. 
 *  @date      2015/2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */


#include <QApplication>
#include <QDebug>
#include <QImageReader>
#include <QList>
#include <QTranslator>

#include "gribouillot.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("Groupe d'Action Terminale");
    app.setApplicationName("Gribouillot");

    QTranslator translator;
    //translator.load("tr_fr");
    app.installTranslator(&translator);

    QStringList args = app.arguments();

    Gribouillot w;
    if (args.size() > 1)
    {
        QRegularExpression re(".*grib");
        if ( re.match(args.at(1)).hasMatch())
            w.openProject(args.at(1));
    }

    return app.exec();
}

/**
 * @brief   Utility function when opening an image file
 */
QString getSupportedImageFormats()
{
    QList<QByteArray> list = QImageReader::supportedImageFormats();
    QString supportedFormats;
    QString defaultFilter;

    // Create the filter list: *.jpg;;*.png etc
    for(int i=0; i<list.size(); i++)
    {
        supportedFormats += "*.";
        supportedFormats += list[i].constData();
        supportedFormats += ";;";
    }
    // Add an "All images" option to the filter
    defaultFilter = supportedFormats.split(";;").join(" ");
    defaultFilter.prepend("All images (").append(")");
    supportedFormats.append(defaultFilter);

    //qDebug() << "supportedFormats" << supportedFormats;
    return supportedFormats;
}


/**
 * @brief   Return a QString with "All images" extension.
 * @details Used with a QFileDialog to choose an image.
 */
QString getDefaultImageFilter(QString supportedFormats)
{
    int indexOfAll = supportedFormats.indexOf("All");
    QString defaultFilter = supportedFormats.mid(indexOfAll);

    //qDebug() << "defaultFilter" << defaultFilter;
    return defaultFilter;
}
