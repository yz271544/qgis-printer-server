//
// Created by Lyndon on 2025/1/29.
//

#ifndef HELLOCONTROLLER_H
#define HELLOCONTROLLER_H

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#if OATPP_VERSION_LESS_1_4_0
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#else
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"
#include "oatpp/Environment.hpp"
#endif

#include OATPP_CODEGEN_BEGIN(DTO)

class PostRequestDto : public oatpp::DTO {
    DTO_INIT(PostRequestDto, DTO)
    DTO_FIELD(String, message);
};

#include OATPP_CODEGEN_END(DTO)


#include OATPP_CODEGEN_BEGIN(ApiController)

class HelloController : public oatpp::web::server::api::ApiController {
public:
    HelloController(const std::shared_ptr<ObjectMapper>& objectMapper, const oatpp::String& routePrefix);

    static std::shared_ptr<HelloController> createShared(const std::shared_ptr<ObjectMapper>& objectMapper, const oatpp::String& routePrefix);

    ENDPOINT("POST", "/post", postEndpoint,
             BODY_DTO(Object<PostRequestDto>, requestDto)) {
        if (requestDto->message) {
            auto message = requestDto->message;
#if OATPP_VERSION_LESS_1_4_0
            OATPP_LOGD("MyController", "Received message: %s", message->c_str());
#else
            OATPP_LOGd("MyController", "Received message: %s", message->c_str());
#endif
            return createResponse(Status::CODE_200, "Message received");
        } else {
            return createResponse(Status::CODE_400, "Missing message");
        }
    }

private:
    std::shared_ptr<OutgoingResponse> createResponse(const Status& status, const char* message) {
        auto response = ResponseFactory::createResponse(status, message);
        response->putHeader(Header::CONTENT_TYPE, "text/plain");
        return response;
    }
};

#include OATPP_CODEGEN_END(ApiController)



#endif //HELLOCONTROLLER_H
