//
// Created by etl on 2/3/25.
//

#ifndef CBOOT_QGSUTIL_H
#define CBOOT_QGSUTIL_H


#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <spdlog/spdlog.h>
#include <QCoreApplication>
#include <QFont>
#include <iostream>
#include <memory>
#include <qgspallabeling.h>
#include <qgstextformat.h>
#include <qgsvectorfilewriter.h>
#include <qgsvectorlayer.h>
#include <qgsvectorlayerlabeling.h>
#include <qthread.h>
#include <utils/JsonUtil.h>

#if defined(_WIN32)
#include <windows.h>
#endif

class QtFontUtil {
public:
    static QgsTextFormat* createFont(const QString& font_family, int8_t font_size, const QString& font_color,
                                     bool is_bold, bool is_italic, Qgis::TextOrientation orientation, double spacing);
};


class QgsUtil {
public:
    static void showLayerLabel(QgsVectorLayer* layer, const QString& style);


    static QgsVectorLayerSimpleLabeling* getLayerLabel(QVariantMap& style, const std::string& label_of_field_name);


    static float d300PixelToMm(float pixel_size);


    static QgsCoordinateTransform* coordinateTransformer4326To3857(QgsProject* project);


    static std::unique_ptr<QgsVectorLayer> writePersistedLayer(const QString& layer_name, QgsVectorLayer* layer,
                                                               const QString& project_dir, const QgsFields& fields,
                                                               Qgis::WkbType qgs_wkb_type,
                                                               const QgsCoordinateTransformContext& cts,
                                                               const QgsCoordinateReferenceSystem& crs);
};



#endif //CBOOT_QGSUTIL_H
