//
// Created by etl on 25-8-14.
//
#if OATPP_VERSION_LESS_1_4_0
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#else
#include <oatpp/json/ObjectMapper.hpp>
#endif
#include "../lib/googletest/googletest/include/gtest/internal/gtest-port.h"
#include "gtest/gtest.h"

TEST(OatppTypeTest, FloatTest) {
    auto floatValue1 = oatpp::Float32(113.7491420219648);
    auto floatValue2 = oatpp::Float32(113.7491420219648);
    auto primitiveEq = floatValue1 == floatValue2;
    GTEST_LOG_(INFO) << "primitiveEq:" << primitiveEq;
    //auto isEqual = oatpp::Float32::equals(floatValue1, floatValue2);

    //GTEST_LOG_(INFO) << "isEqual:" << isEqual << " primitiveEq:" << primitiveEq;



}