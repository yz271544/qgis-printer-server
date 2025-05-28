//
// Created by etl on 4/27/25.
//
#include "gtest/gtest.h"
#include "utils/DegreeUtil.h"

TEST(DegreeUtil, test1) {
    auto d1 = DegreeUtil::calculate_opposite_side(1166.1825566352245, 36.98);
    GTEST_LOG_(INFO) << "d1:" << d1;
    auto d2 = DegreeUtil::calculate_opposite_side(1166.1825566352245 - 856.6314506862502, 36.98);
    GTEST_LOG_(INFO) << "d2:" << d2;

}
