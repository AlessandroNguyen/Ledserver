#include "ledserver.h"
#include "config.h"



LedServer::LedServer(quint16 port, QObject *parent) : QObject(parent), m_port(port)
{
    m_gpio = new Gpio(this);
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &LedServer::myNewConnection);
}

void LedServer::start()
{
    m_server->listen(QHostAddress::Any, m_port);
}

// Client hat Verbindung zum Server aufgebaut
void LedServer::myNewConnection()
{
    m_socket = m_server->nextPendingConnection();
    connect(m_socket, &QTcpSocket::readyRead, this, &LedServer::myServerRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &LedServer::myClientDisconnect);
    m_socket ->write("Bitte Zahl von 0 bis 15 eingeben:\n");
}

// Client hat Verbindung zum Server getrennt
void LedServer::myClientDisconnect()
{
    qDebug() << "Disconnected";
    m_socket->close();

    for (int i = 0; i < 4; i++)
    {
        m_gpio->set(LEDS[i], 0);
    }
}

// Client hat eine Zahl (0...15) zum Server gesendet
void LedServer::myServerRead()
{
    while (m_socket->bytesAvailable()) {
        QString message = m_socket->readAll();
        int zahl = message.toInt();
        qDebug() << zahl;

        if (zahl >= 0 && zahl < 16)
        {
            m_socket ->write("OK \n");
            m_gpio->set(zahl);
        }

        else{
            m_socket ->write("Nicht OK \n");
        }

        m_socket ->write("Bitte Zahl von 0 bis 15 eingeben: ");

    }
}
