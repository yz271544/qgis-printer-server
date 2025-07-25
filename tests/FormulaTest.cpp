//
// Created by yz271 on 25-7-21.
//
#include "gtest/gtest.h"

TEST(blaTest, testdev)
{
    //std::fabs((a) - (b)) <= std::numeric_limits<double>::epsilon() * std::max(std::fabs((a)), std::fabs((b)))
    GTEST_LOG_(INFO) << std::fabs(3.14);

    double xMin = std::numeric_limits<double>::max();
    GTEST_LOG_(INFO) << xMin;

}