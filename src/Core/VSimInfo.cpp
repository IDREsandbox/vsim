#include "VSimInfo.h"

#include <QFileInfo>
#include <QDir>
#include <QDebug>

static QString g_app_path;
static bool g_is_bundle;
static QString g_assets_path;
static bool g_is_mac;

namespace VSimInfo {

    QString app() {
        return g_app_path;
    }
    
    bool isMacBundle() {
        return g_is_bundle;
    }

    QString assets() {
        return g_assets_path;
    }

    void initPath(QString app_path) {
        g_app_path = app_path;
        bool do_normal_assets = true;

#if __APPLE__
        qDebug() << "init path: is apple";
        QDir macos_dir = QFileInfo(app_path).dir();
	    QString plist_path = macos_dir.absoluteFilePath("../Info.plist");
        qDebug() << "plist path?" << plist_path << macos_dir;
	    if (QFileInfo(plist_path).exists()) {
            do_normal_assets = false;
            g_is_bundle = true;
		    QString bundle_root = macos_dir.absoluteFilePath("../../");
		    g_assets_path = QDir(bundle_root).absoluteFilePath("Contents/Resources/assets");
        }
#endif
        if (do_normal_assets) {
            g_is_bundle = false;
            g_assets_path = QFileInfo(app_path).dir().absoluteFilePath("assets");
        }
    }

}