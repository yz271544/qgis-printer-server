//
// Created by etl on 2/3/25.
//

#include "QgsUtil.h"
#include "FileUtil.h"

QgsTextFormat* QtFontUtil::createFont(
        const QString& font_family,
        double font_size,
        const QString& font_color,
        bool is_bold,
        bool is_italic,
        Qgis::TextOrientation orientation,
        double spacing)
{
    auto text_format = std::make_unique<QgsTextFormat>();
    //auto font = std::make_unique<QFont>(font_family);
    auto font = text_format->font();
    font.setFamily(font_family);
    font.setLetterSpacing(QFont::AbsoluteSpacing, spacing);
    text_format->setFont(font);
    text_format->setColor(QColor(font_color));
    text_format->setForcedBold(is_bold);
    text_format->setForcedItalic(is_italic);
    text_format->setOrientation(orientation);
    text_format->setSizeUnit(Qgis::RenderUnit::Points);
    text_format->setSize(font_size);
    return text_format.release();
}

void QgsUtil::showLayerLabel(QgsVectorLayer* layer, const QString& style) {
    layer->setLabelsEnabled(true);
    layer->setDisplayExpression("name");
    // Create label settings
    QgsPalLayerSettings label_settings;
    label_settings.fieldName = "name";
    // label_settings.placement = QgsPalLayerSettings::OverPoint;

    // Create text format for the labels
    QgsTextFormat* text_format = QtFontUtil::createFont(style, 12, QString("#000000"), false, false,
                                                        Qgis::TextOrientation::Horizontal, 0.0);

    // Apply text format to label settings
    label_settings.setFormat(*text_format);

    // Apply label settings to the layer
    auto labeling = std::make_shared<QgsVectorLayerSimpleLabeling>(label_settings);
    layer->setLabeling(labeling.get());
}

QgsVectorLayerSimpleLabeling*
QgsUtil::getLayerLabel(QVariantMap& style, const std::string& label_of_field_name) {
    QgsPalLayerSettings label_settings;
    label_settings.fieldName = label_of_field_name.c_str();
    // label_settings.placement = QgsPalLayerSettings::OverPoint;

    // Create text format for the labels
    QString font_family = style.value("font_family").toString();
    QgsTextFormat* text_format = QtFontUtil::createFont(font_family, 12, QString("#000000"), false, false,
                                                        Qgis::TextOrientation::Horizontal, 0.0);

    // Apply text format to label settings
    label_settings.setFormat(*text_format);

    // Apply label settings to the layer
    auto vectorLayerSimpleLabeling = std::make_unique<QgsVectorLayerSimpleLabeling>(label_settings);
    return vectorLayerSimpleLabeling.release();
}

float QgsUtil::d300PixelToMm(float pixel_size) {
    // 假设Web页面上的字体大小是10像素
    // pixel_size = 10
    float web_dpi = 96;
    float target_dpi = 300;

    // 转换为毫米
    auto font_size_mm = static_cast<float>((pixel_size / web_dpi) * 25.4 * PIXEL_MM_COEFFICIENT); // 先转换为毫米
    // 缩小比例
    float shrink_ratio = web_dpi / target_dpi;
    return font_size_mm * shrink_ratio; // 缩小字体
}

QgsCoordinateTransform* QgsUtil::coordinateTransformer4326To3857(QgsProject* project) {
    // Set coordinate transform
    QgsCoordinateReferenceSystem crs_4326("EPSG:4326"); // 假设 4326 是对应的EPSG代码
    QgsCoordinateReferenceSystem crs_3857("EPSG:3857"); // 假设 3857 是对应的EPSG代码
    auto transformer = std::make_unique<QgsCoordinateTransform>(crs_4326, crs_3857, project);
    return transformer.release();
}

QgsCoordinateTransform* QgsUtil::coordinateTransformer3857To4326(QgsProject* project) {
    // Set coordinate transform
    QgsCoordinateReferenceSystem crs_3857("EPSG:3857"); // 假设 3857 是对应的EPSG代码
    QgsCoordinateReferenceSystem crs_4326("EPSG:4326"); // 假设 4326 是对应的EPSG代码
    auto transformer = std::make_unique<QgsCoordinateTransform>(crs_3857, crs_4326, project);
    return transformer.release();
}

std::unique_ptr<QgsVectorLayer> QgsUtil::writePersistedLayer(
        const QString& layer_name,
        QgsVectorLayer* layer,
        const QString& project_dir,
        const QgsFields& fields,
        Qgis::WkbType qgs_wkb_type,
        const QgsCoordinateTransformContext& cts,
        const QgsCoordinateReferenceSystem& crs) {

    spdlog::debug("CRS: {}", crs.toWkt().toStdString());
    spdlog::debug("Number of features in layer: {}", layer->featureCount());
    spdlog::debug("project_dir: {}, layer_name: {}", project_dir.toStdString(), layer_name.toStdString());
    QString file_prefix = QString("%1/%2").arg(project_dir, layer_name);

    QFile dir(file_prefix);
    if (dir.exists() && !dir.isWritable()) {
        spdlog::error("the mProjectDir can't write: {}", file_prefix.toStdString());
        return nullptr;
    }

    QString file_path = QString("%1.geojson").arg(file_prefix);
    QString temp_file_path = QString("%1.tmp").arg(file_prefix);

    spdlog::debug("GeoJSON file path: {}", file_path.toStdString());

    // 删除旧文件
    if (QFile::exists(file_path)) {
        if (!QFile::remove(file_path)) {
            spdlog::debug("Failed to delete existing file: {}", file_path.toStdString());
            return nullptr;
        }
    }

    // 设置写入选项
    QgsVectorFileWriter::SaveVectorOptions options;
    options.driverName = "GeoJSON";
    options.fileEncoding = "UTF-8";
    options.includeZ = true;
    options.overrideGeometryType = qgs_wkb_type;

    // 写入临时文件
    QgsVectorFileWriter* writer = QgsVectorFileWriter::create(
            temp_file_path,
            fields,
            qgs_wkb_type,
            crs,
            cts,
            options
    );

    if (writer->hasError() != QgsVectorFileWriter::NoError) {
        std::cerr << "Error creating file writer: " << writer->errorMessage().toStdString() << std::endl;
        delete writer;
        return nullptr;
    }

    // 写入所有要素
    QgsFeatureIterator it = layer->getFeatures();
    QgsFeature feature;
    while (it.nextFeature(feature)) {
        //auto attributeMap = feature.attributeMap();
        //auto const& attr_map_json = JsonUtil::variantMapToJson(attributeMap);
        if (!writer->addFeature(feature)) {
            spdlog::error("Failed to write feature ID: {}", feature.id());
        }
    }

    writer->flushBuffer();
    delete writer;

    // 确保资源释放
    QThread::msleep(100);

    // 重命名临时文件
    QString temp_real_file_path = QString(temp_file_path).append(".geojson");
    if (!QFile::rename(temp_real_file_path, file_path)) {
        spdlog::error("Failed to rename temp file to target file, Temp file path: {}, Target file path: {}",
                      temp_file_path.toStdString(), file_path.toStdString());
        return nullptr;
    }

    spdlog::debug("Successfully wrote GeoJSON file: {}, baseName: {}", file_path.toStdString(), layer_name.toStdString());
    auto qgsVectorLayer = std::make_unique<QgsVectorLayer>(file_path, layer_name, "ogr");
    return qgsVectorLayer;
}
