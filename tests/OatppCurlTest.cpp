﻿//
// Created by Lyndon on 2025/1/26.
//

#include <oatpp-curl/RequestExecutor.hpp>
#if OATPP_VERSION_LESS_1_4_0
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#else
#include <oatpp/json/ObjectMapper.hpp>
#endif
#include <oatpp/web/protocol/http/Http.hpp>
#include "gtest/gtest.h"
#include "utils/OApiClient.h"


// Test fixture for the API client
class OApiClientTest : public ::testing::Test {
private:
    constexpr static const char* TAG = "OApiClientTest";
protected:
    void SetUp() override {
        // Initialize oatpp environment
#if OATPP_VERSION_LESS_1_4_0
        oatpp::base::Environment::init();
        auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
#else
        oatpp::Environment::init();
        auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
#endif
        // Create a RequestExecutor using oatpp-curl
        auto requestExecutor = oatpp::curl::RequestExecutor::createShared("http://172.31.100.33:38089"); // "http://172.31.100.33:38089"  "http://127.0.0.1:8088"

        // Initialize the API client
        m_client = OApiClient::createShared(requestExecutor, objectMapper);

    }

    void TearDown() override {
#if OATPP_VERSION_LESS_1_4_0
        // Destroy oatpp environment
        oatpp::base::Environment::destroy();
#else
        oatpp::Environment::destroy();
#endif
    }

    std::shared_ptr<OApiClient> m_client;
};


TEST_F(OApiClientTest, PostRequestTest) {
    auto loginDto = LoginRequestDto::createShared();

    // 设置 headers
    loginDto->username = "jingwei";
    loginDto->password = "gis@123";


    // Perform the GET request
    auto response = m_client->doPostLogin(loginDto, "application/json; charset=utf-8", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36");

    // Check that the response is not null
    ASSERT_TRUE(response);

    // Check the status code (assuming the endpoint returns 200 OK)
    ASSERT_EQ(response->getStatusCode(), 200);

    // You can also check the response body if needed
    auto body = response->readBodyToString();
    std::cout << "Response Body: " << body->c_str() << std::endl;

}