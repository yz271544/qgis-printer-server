//
// Created by etl on 24-12-18.
//

#ifndef HELLOHANDLER_H
#define HELLOHANDLER_H

#include "oatpp/web/server/HttpRequestHandler.hpp"

#define O_UNUSED(x) (void)x;

// 自定义请求处理程序
class HelloHandler : public oatpp::web::server::HttpRequestHandler
{
public:
    // 处理传入的请求，并返回响应
    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override;
};

#endif //HELLOHANDLER_H
