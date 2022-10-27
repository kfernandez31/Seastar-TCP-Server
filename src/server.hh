#pragma once

#include <seastar/net/tcp.hh>

class Server {
public:
    virtual seastar::future<> accept() = 0;
};

class TCPServer : public Server {
private:
    seastar::net::ipv4& inet;
    seastar::net::tcp<seastar::net::ipv4_traits>::listener listener;
public:
    TCPServer(seastar::net::ipv4& inet);
    seastar::future<> accept();
};

class TCPConnectionHandler {
private:
    seastar::net::tcp<seastar::net::ipv4_traits>::connection connection;
public:
    TCPConnectionHandler(seastar::net::tcp<seastar::net::ipv4_traits>::connection conn);
    seastar::future<> handle();
};