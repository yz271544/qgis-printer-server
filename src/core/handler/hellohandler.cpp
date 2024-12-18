//
// Created by etl on 24-12-19.
//

#include "hellohandler.h"
#include "oatpp/web/server/HttpRequestHandler.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> HelloHandler::handle(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& request) {
    O_UNUSED(request);

    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::BufferBody>("Hello, World! This is oatpp_demo!", oatpp::web::protocol::http::Header::Value::CONTENT_TYPE_APPLICATION_JSON);
    return oatpp::web::protocol::http::outgoing::Response::createShared(oatpp::web::protocol::http::Status::CODE_200, body);
}