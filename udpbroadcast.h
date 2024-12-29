#ifndef UDPBROADCAST_H
#define UDPBROADCAST_H

#include <QObject>
#include <QString>
#include <QUDPSocket>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QTime>

class UDPBroadcast : public QObject
{
private:
    Q_OBJECT
    QUdpSocket *udpSocket;
    QString localHostName;
    QList<QHostAddress> broadcastList;
    void setBroadcastAddresses();

public:
    explicit UDPBroadcast(QObject *parent = 0);
    ~UDPBroadcast();

signals:
    void notifyNewMessage(QVariant message);

public slots:
    void broadcastMessage(QString message);
    void readPendingMessages();

private:
    bool isLocalAddress(const QHostAddress &add);
};

#endif // UDPBROADCAST_H
