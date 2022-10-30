#include "server.hh"

#include <string>

using namespace seastar;
using namespace net;

const int port = 5555;

//TODO: think of `seastar::repeat` (chapter 11)

TCPServer::TCPServer(ipv4& inet) : inet(inet), listener(inet.get_tcp().listen(port)) {}

void TCPServer::accept() {
    (void) listener.accept().then([this] (net::tcp<ipv4_traits>::connection conn) {
        (new TCPConnectionHandler(std::move(conn)))->handle();
        accept();
    });
}

TCPConnectionHandler::TCPConnectionHandler(net::tcp<ipv4_traits>::connection conn) : connection(std::move(conn)) {}

void TCPConnectionHandler::handle() {
    (void) connection.wait_for_data().then([this] {
        auto bytes_read = connection.read();
        if (!bytes_read.len()) {
            connection.close_write();
            return;
        }
        std::cout << "read " << bytes_read.len() << " bytes\n";
        (void) connection.send(std::move(bytes_read));
        handle();
    });
}
