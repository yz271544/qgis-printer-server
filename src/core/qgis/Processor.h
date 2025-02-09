//
// Created by etl on 2/7/25.
//

#ifndef JINGWEIPRINTER_PROCESSOR_H
#define JINGWEIPRINTER_PROCESSOR_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <future>
#include <functional>
#include <yaml-cpp/yaml.h>
#include <QFile>
#include <QJsonDocument>

#include "core/error/exceptions.h"
#include "core/enums/PaperSpecification.h"
#include "core/fetch/PlottingFetch.h"
#include "utils/Formula.h"
#include "utils/NodeToMap.h"
#include "utils/CompressUtil.h"
#include "core/qgis/layout/JwLayout.h"
#include "core/qgis/layout/JwLayout3D.h"
#include "App.h"

// 由于 Python 中的 Dict 是一个类似字典的结构，在 C++ 中可以使用 std::unordered_map 来模拟
// 这里简单定义一个别名方便使用
template<typename K, typename V>
using Dict = std::unordered_map<K, V>;

class Processor {
private:
    bool m_enable_3d = true;
    bool m_verbose = false;
    std::shared_ptr<YAML::Node> m_config;
    std::unique_ptr<PlottingFetch> m_plotting_fetch;
    std::unique_ptr<App> m_app;
    std::unique_ptr<QVariantMap> m_setting_image_spec;
    QString m_export_prefix;
public:
    // 构造函数
    Processor(QList<QString> argvList, std::shared_ptr<YAML::Node>& config);

    // 析构函数
    ~Processor();

    // 异步获取绘图数据的函数
    std::future<DTOWRAPPERNS::DTOWrapper<PlottingRespDto>>
    fetchPlotting(const oatpp::String &token, const oatpp::String &scene_type,
                  DTOWRAPPERNS::DTOWrapper<TopicMapData> &topic_map_data);


    // 检查和处理闭合几何图形的函数
    void checkDealWithClosedGeometry(const DTOWRAPPERNS::DTOWrapper<GeoPolygonJsonDto> &geojson);

    // 异步处理绘图数据的函数
    std::future<DTOWRAPPERNS::DTOWrapper<ResponseDto>> processByPlottingWeb(
            const oatpp::String &token, const DTOWRAPPERNS::DTOWrapper<PlottingDto> &plottingDto);

    // 绘制图层
    void plottingLayers(const DTOWRAPPERNS::DTOWrapper<PlottingRespDto> &plotting_data);

    // 添加2d布局
    void add_layout(QgsMapCanvas *canvas,
                    const QString &layout_name,
                    const DTOWRAPPERNS::DTOWrapper<PlottingDto> &plottingWeb,
                    const QMap<QString, QVariant> &image_spec,
                    PaperSpecification available_paper,
                    bool write_qpt,
                    const QVector<QString> &removeLayerNames,
                    const QVector<QString> &removeLayerPrefixs);

    // 添加3d布局
    void add_3d_layout(QgsMapCanvas *canvas,
                    const QString &layout_name,
                    const DTOWRAPPERNS::DTOWrapper<PlottingDto> &plottingWeb,
                    const QMap<QString, QVariant> &image_spec,
                    PaperSpecification available_paper,
                    bool write_qpt,
                    const QVector<QString> &removeLayerNames,
                    const QVector<QString> &removeLayerPrefixs);

    // 获取图像子目录
    std::string get_image_sub_dir(const std::string &layout_name) {
        // 这里只是简单的占位实现，实际需要根据具体需求实现
        return "image_sub_dir_" + layout_name;
    }

    // 异步导出图像
    std::future<void>
    export_image(const std::string &scene_name, const std::string &layout_name, const std::string &image_sub_dir,
                 const std::string &paper_name) {
        // 使用 std::async 来实现异步操作
        return std::async(std::launch::async, [this, scene_name, layout_name, image_sub_dir, paper_name]() {
            // 这里只是简单的占位实现，实际需要根据具体需求实现
            std::cout << "Exporting image with scene_name: " << scene_name << ", layout_name: " << layout_name
                      << ", image_sub_dir: " << image_sub_dir << ", paper_name: " << paper_name << std::endl;
        });
    }

    // 压缩项目的静态方法
    std::string zip_project(const QString &scene_name);

    // 按颜色分组圆的函数
    Dict<std::string, Dict<std::string, std::vector<std::vector<double>>>> _grouped_circle_by_color_grouped(
            const Dict<std::string, int> &grouped_color,
            const std::vector<std::vector<double>> &polygon_geometry_coordinates_list,
            const std::vector<int> &polygon_geometry_properties_radius,
            const std::vector<std::vector<int>> &style_percents,
            const std::vector<std::vector<std::string>> &areas_color_list,
            const std::vector<std::vector<double>> &areas_opacity_list) {
        Dict<std::string, Dict<std::string, std::vector<std::vector<double>>>> result;
        // 这里只是简单的占位实现，实际需要根据具体需求实现
        return result;
    }

    // 按相同颜色分组的函数
    Dict<std::string, Dict<std::string, std::vector<std::string>>>
    _single_color_group(const std::vector<std::string> &name_list,
                        const std::vector<std::string> &geometry_coordinates_list,
                        const std::vector<std::string> &style_list) {
        Dict<std::string, Dict<std::string, std::vector<std::string>>> result;
        // 这里只是简单的占位实现，实际需要根据具体需求实现
        return result;
    }
};


#endif //JINGWEIPRINTER_PROCESSOR_H
