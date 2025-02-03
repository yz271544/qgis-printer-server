//
// Created by etl on 2/3/25.
//

#ifndef CBOOT_NODETOMAP_H
#define CBOOT_NODETOMAP_H


#include <QVariantMap>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/parser.h>
#include <yaml-cpp/yaml.h>
#include <qdebug.h>

class NodeToMap {
public:
    static QVariantMap mapToVariantMap(const YAML::Node& config);

    static QVariant nodeToVariant(const YAML::Node& node);

    static QVariantList sequenceToVariantList(const YAML::Node& node);

    static QVariant scalarToVariant(const YAML::Node& node);
};




#endif //CBOOT_NODETOMAP_H
