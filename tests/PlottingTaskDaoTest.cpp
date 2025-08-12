//
// Created by etl on 25-8-12.
//
#include <gtest/gtest.h>
#include "gdal.h"
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include <string>
#include <cstdio>
#include "core/dao/PlottingTaskDao.h"

TEST(plottingTaskDaoTest, cleanCompleteTasksTest) {
    auto plottingTaskDao = PlottingTaskDao("/home/etl/.local/share/stxx/qgis-printer-server/profiles/default/stxx/qgis-printer-server.db");

    auto deleteRet = plottingTaskDao.cleanCompleteTasks("completed", 7);

    GTEST_LOG_(INFO) << "clean completeTasks: " << deleteRet;

}