//
// Created by Lyndon on 2025/1/26.
//

#include "gtest/gtest.h"
#include <QString>
#include <QList>
#include "config.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/format.h>

#include "utils/Formula.h"
#include "core/enums/PaperSpecification.h"

class PaperTest : public ::testing::Test {

};

TEST_F(PaperTest, lowerCaseA3) {
    std::string paperSpec = "a3";
    auto upperPaperSpec = Formula::toUpperCase(paperSpec);
    QString paperString = QString::fromStdString(upperPaperSpec);
    auto paper = new PaperSpecification(paperString);
    auto paperName = paper->getPaperName();
    GTEST_LOG_(INFO) << "paper name: " << paperName.toStdString();
    ASSERT_EQ(paperName, "A3");
}

TEST_F(PaperTest, lowerCaseA900) {
    std::string paperSpec = "a900";
    auto upperPaperSpec = Formula::toUpperCase(paperSpec);
    QString paperString = QString::fromStdString(upperPaperSpec);
    auto paper = new PaperSpecification(paperString);
    auto paperName = paper->getPaperName();
    GTEST_LOG_(INFO) << "paper name: " << paperName.toStdString();
    ASSERT_EQ(paperName, "A900");
}

