#include "udpbroadcast.h"

UDPBroadcast::UDPBroadcast(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket(this);
    localHostName = QHostInfo::localHostName();
    setBroadcastAddresses();
    udpSocket->bind(QHostAddress::Any, 37282);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingMessages()));
}

UDPBroadcast::~UDPBroadcast()
{
    delete udpSocket;
}

void UDPBroadcast::broadcastMessage(QString message)
{
    message.prepend(localHostName + "@" + QTime::currentTime().toString() + ": ");
    auto broadcastSet = broadcastList.toSet();
    foreach (const QHostAddress &broadcast, broadcastSet) {
        qDebug() << "broadcast = " << broadcast;
        udpSocket->writeDatagram(message.toUtf8(), broadcast, 37282);
    }
}

void UDPBroadcast::readPendingMessages()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray message;
        message.resize(udpSocket->pendingDatagramSize());
        QHostAddress host;
        quint16 port;
        udpSocket->readDatagram(message.data(), message.size(), &host, &port);
        if (isLocalAddress(host)) {
            qDebug() << host << port;
        } else {
            emit notifyNewMessage(QVariant(message));
        }
    }
}

bool UDPBroadcast::isLocalAddress(const QHostAddress &addr)
{
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (addr.isEqual(address))
            return true;
    }
    return false;
}

void UDPBroadcast::setBroadcastAddresses()
{
    broadcastList.append(QHostAddress::Broadcast);
    auto allInterfaces = QNetworkInterface::allInterfaces();
    foreach (const QNetworkInterface &interface, allInterfaces) {

        bool up = interface.flags().testFlag(QNetworkInterface::IsUp);
        bool runnig = interface.flags().testFlag(QNetworkInterface::IsRunning);
        bool broadcast = interface.flags().testFlag(QNetworkInterface::CanBroadcast);
        bool noloopBack = !interface.flags().testFlag(QNetworkInterface::IsLoopBack);
        if (up && runnig && broadcast && noloopBack) {

            auto addressEntries = interface.addressEntries();
            foreach (const QNetworkAddressEntry &address, addressEntries) {

                QHostAddress broadcast = address.broadcast();
                if (!address.broadcast().isNull() && !broadcastList.contains(broadcast)) {

                    broadcastList.append(broadcast);
                }
            }
        }
    }
}
