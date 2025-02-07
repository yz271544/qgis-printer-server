//
// Created by Lyndon on 2025/1/26.
//

#include "gtest/gtest.h"
#include "utils/Formula.h"

TEST(blaTest, test1) {
    //arrange
    //act
    //assert
    EXPECT_EQ (Formula::bla (0),  0); //通过
    EXPECT_EQ (Formula::bla (2), 4); //通过
    EXPECT_NE (Formula::bla (4), 6); //不通过
}


TEST(blaTest, test2) {
    double num1 = 0.1 + 0.2;
    double num2 = 0.3;

    auto vv = DOUBLECOMPARENEAR(num1, num2);

    std::cout << "vv: " << vv << std::endl;

    if (vv) {
        std::cout << "The two double values are considered equal." << std::endl;
    } else {
        std::cout << "The two double values are not considered equal." << std::endl;
    }
}