#ifndef VSIMINFO_H
#define VSIMINFO_H

#include <QString>

// A singleton
// initialize with initPath()
namespace VSimInfo {
    void initPath(QString app_path);

    QString app();
    QString assets();
    QString osgPluginsPath();
    bool isMacBundle();
}



#endif