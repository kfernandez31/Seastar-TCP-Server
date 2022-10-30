//TODO: think of `seastar::repeat` (chapter 11)

#include "server.hh"
#include "parsing.hh"
#include "validation.hh"

using namespace seastar;
using namespace net;

const int port = 5555;

static sstring to_sstring(const packet& p) {
    sstring res = uninitialized_string(p.len());
    auto i = res.begin();
    for (auto& frag : p.fragments()) {
        i = std::copy(frag.base, frag.base + frag.size, i);
    }
    return res;
}

static packet to_packet(const sstring& s) {
    return packet(s.c_str(), s.length());
}

TCPServer::TCPServer(ipv4& inet) : inet(inet), listener(inet.get_tcp().listen(port)), store() {}

MemoryStore& TCPServer::getStore() {
    return store;
}

void TCPServer::accept() {
    (void) listener.accept().then([this] (net::tcp<ipv4_traits>::connection conn) {
        (new TCPConnection(*this, std::move(conn)))->accept();
        accept();
    });
}

TCPConnection::TCPConnection(
    TCPServer& server,
    net::tcp<ipv4_traits>::connection conn 
) : server(server), conn(std::move(conn)) {}

void TCPConnection::accept() {
    (void) conn.wait_for_data().then([this] {
        packet pkt = conn.read();
        
        if (!pkt.len()) {
            conn.close_write();
            return;
        }

        sstring req = to_sstring(pkt);
        std::smatch matched_args;

        switch (get_op_type(req, matched_args)) {
            case op_type::STORE:
                if (is_valid(matched_args[1])) {
                    (void) server
                        .getStore()
                        .store(matched_args[1], matched_args[2])
                        .then([this] {
                            (void) conn.send(std::move(to_packet(resp_done)));
                        });
                } else {
                    (void) conn.send(std::move(to_packet(resp_invalid_args)));
                }
                break;
            case op_type::LOAD:
                if (is_valid(matched_args[1]) && is_valid(matched_args[2])) {
                    (void) server
                        .getStore()
                        .load(matched_args[1])
                        .then([this] (std::optional<sstring> opt) {
                            if (!opt.has_value()) {
                                (void) conn.send(std::move(to_packet(resp_not_found)));
                            } else {
                                (void) conn.send(std::move(to_packet(make_resp_found(opt.value()))));
                            }
                        });
                } else {
                    (void) conn.send(std::move(to_packet(resp_invalid_args)));
                }
                break;
            default:
                (void) conn.send(std::move(to_packet(resp_invalid_op)));
        }

        accept();
    });
}
