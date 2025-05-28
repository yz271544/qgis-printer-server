//
// Created by Lyndon on 2025/1/26.
//

#include "gtest/gtest.h"
#include "utils/CompressUtil.h"
#include <QString>

TEST(compress, test1) {

    CompressUtil::create_zip("/lyndon/iProject/cpath/jingweiprinter/tests", "test.zip");

}

