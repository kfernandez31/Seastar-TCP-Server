#pragma once

#include <seastar/net/tcp.hh>
#include "store.hh"

class Server {
public:
    virtual void accept() = 0;
};

class TCPServer : public Server {
private:
    seastar::net::ipv4& inet;
    seastar::net::tcp<seastar::net::ipv4_traits>::listener listener;
    MemoryStore store;
public:
    TCPServer(seastar::net::ipv4& inet);
    MemoryStore& getStore();
    void accept();
};

class TCPConnection {
private:
    TCPServer& server;
    seastar::net::tcp<seastar::net::ipv4_traits>::connection conn;
public:
    TCPConnection(
        TCPServer& server,
        seastar::net::tcp<seastar::net::ipv4_traits>::connection conn
    );
    void accept();
};
