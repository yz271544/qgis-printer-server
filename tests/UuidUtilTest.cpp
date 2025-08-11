//
// Created by etl on 25-8-11.
//

#include "gtest/gtest.h"
#include "utils/UuidUtil.h"

TEST(UuidUtil, generateTest) {
    GTEST_LOG_(INFO) << "uuid 1: " << UuidUtil::generate();
    GTEST_LOG_(INFO) << "uuid 2: " << UuidUtil::generate();
    GTEST_LOG_(INFO) << "uuid 3: " << UuidUtil::generate();
}

TEST(CurrentDate, getCurrentDate) {
    int64_t current_time = static_cast<int64_t>(time(nullptr));

    GTEST_LOG_(INFO) << std::to_string(current_time);


}