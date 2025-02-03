//
// Created by Lyndon on 2025/1/26.
//

#ifndef OAPICLIENT_H
#define OAPICLIENT_H

#include <oatpp/web/client/RequestExecutor.hpp>
#include <oatpp/web/client/ApiClient.hpp>
#include <oatpp/web/protocol/http/Http.hpp>
#if OATPP_VERSION_LESS_1_3_0
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>
#else
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"
#include "oatpp/Environment.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/Types.hpp"
#endif
#include <iostream>

/* Begin ApiClient codegen */
#include OATPP_CODEGEN_BEGIN(DTO)
class LoginRequestDto : public oatpp::DTO {

    DTO_INIT(LoginRequestDto, DTO);
    DTO_FIELD(String, username);
    DTO_FIELD(String, password);
};

#include OATPP_CODEGEN_END(DTO)

/* Begin ApiClient codegen */
#include OATPP_CODEGEN_BEGIN(ApiClient)

class OApiClient : public oatpp::web::client::ApiClient {
public:
    API_CLIENT_INIT(OApiClient)

    // You can add more API calls here
    API_CALL("POST", "/api/login", doPostLogin, BODY_DTO(Object<LoginRequestDto>, body), HEADER(String, contentType, "Content-Type"), HEADER(String, userAgent, "User-Agent"))

  };

#include OATPP_CODEGEN_END(ApiClient)
/* End ApiClient codegen */


#endif //OAPICLIENT_H
