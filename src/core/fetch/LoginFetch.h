//
// Created by etl on 2/6/25.
//

#ifndef JINGWEIPRINTER_LOGINFETCH_H
#define JINGWEIPRINTER_LOGINFETCH_H

#include <spdlog/spdlog.h>
#include <oatpp/web/client/ApiClient.hpp>
#if OATPP_VERSION_LESS_1_4_0
#include <oatpp/core/Types.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#else
#include <oatpp/Types.hpp>
#include <oatpp/json/ObjectMapper.hpp>
#endif
#include <oatpp-curl/RequestExecutor.hpp>
#include <iostream>

#include "utils/OApiClient.h"
#include "core/handler/dto/plotting.h"

#include "config.h"

#include OATPP_CODEGEN_BEGIN(DTO)
class LoginResponseDto : public oatpp::DTO {
    DTO_INIT(LoginResponseDto, DTO)

    DTO_FIELD(Int32, code);
    DTO_FIELD(String, msg);
    DTO_FIELD(String, token, "data.token"); // 直接映射 data.token
};

#include OATPP_CODEGEN_END(DTO)

class LoginFetch {
private:
    std::shared_ptr<OApiClient> m_client;
    std::shared_ptr<oatpp::curl::RequestExecutor> m_requestExecutor;
    std::shared_ptr<OBJECTMAPPERNS::ObjectMapper> m_objectMapper;

public:
    LoginFetch(const oatpp::String& baseUrl)
            : m_requestExecutor(oatpp::curl::RequestExecutor::createShared(baseUrl))
    {
#if OATPP_VERSION_LESS_1_4_0
        oatpp::base::Environment::init();
        m_objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
#else
        oatpp::Environment::init();
        m_objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
#endif
        m_client = OApiClient::createShared(m_requestExecutor, m_objectMapper);
    }

    DTOWRAPPERNS::DTOWrapper<LoginResponseDto> fetch(
            const oatpp::Object<LoginRequestDto>& loginDto = nullptr)
    {
        // 发送 POST 请求
        auto response = m_client->doPostLogin(loginDto, "application/json; charset=utf-8", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36");

        try {
            if (response->getStatusCode() == 200) {
                DTOWRAPPERNS::DTOWrapper<LoginResponseDto> loginObj = response->readBodyToDto<oatpp::Object<LoginResponseDto>>(m_objectMapper.get());
                return loginObj;
            } else {
                SPDLOG_ERROR("Failed to fetch login response: {}", response->getStatusCode());
                auto errDTo = LoginResponseDto::createShared();
                errDTo->code = response->getStatusCode();
                errDTo->msg = "Failed to fetch login response";
                return errDTo;
            }
        } catch (const std::exception& e) {
            SPDLOG_ERROR("Failed to fetch login response: {}", e.what());
            auto errDTo = LoginResponseDto::createShared();
            errDTo->code = response->getStatusCode();
            errDTo->msg = e.what();
            return errDTo;
        }
    }
};


#endif //JINGWEIPRINTER_LOGINFETCH_H
