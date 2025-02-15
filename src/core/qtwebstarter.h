//
// Created by etl on 2025/2/15.
//

#ifndef JINGWEIPRINTER_QTWEBSTARTER_H
#define JINGWEIPRINTER_QTWEBSTARTER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QString>
#include <QByteArray>
#include <QTimer>
#include <QEventLoop>
#include <spdlog/spdlog.h>

#include "starter.h"
#include "starterregister.h"

#if defined(_WIN32) && defined(_MSC_VER)
#include "oatpp/core/base/Environment.hpp"
#endif

class QtWebStarter : public QObject, public BaseStarter{
    Q_OBJECT
public:

    QtWebStarter();

    ~QtWebStarter();

    BaseStarter* GetInstance();

    void Init(StarterContext& context);

    void Setup(StarterContext& context);

    void Start(StarterContext& context);

    void Stop(StarterContext& context);

    int PriorityGroup();

    bool StartBlocking();

    std::array<int, 4> Priority();

    std::string GetName();

    void SetBlocking(bool isBlock);

private slots:
    void newConnection();
    void readClient();
    void discardClient();

private:
    QTcpServer *server;
    QMap<QTcpSocket*, QByteArray> buffers;
    QThread *serverThread;
    bool mBlock;
    bool mStopped = false;

    void handleRequest(QTcpSocket *socket, const QByteArray &request);
    void sendResponse(QTcpSocket *socket, const QByteArray &response);
};


#endif //JINGWEIPRINTER_QTWEBSTARTER_H
