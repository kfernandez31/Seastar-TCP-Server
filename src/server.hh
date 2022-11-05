#pragma once

#include <seastar/net/tcp.hh>
#include <seastar/core/future.hh>
#include <string>
#include "store.hh"

class tcp_server {
    std::vector<seastar::server_socket> _tcp_listeners;
    MemoryStore store;
public:
    MemoryStore& get_store();
    seastar::future<> listen(seastar::ipv4_addr addr);
    seastar::future<> stop();
    void do_accepts(std::vector<seastar::server_socket>& listeners);

    class tcp_connection {
        tcp_server& _server;
        std::string buf;
        seastar::connected_socket _fd;
        seastar::input_stream<char> _read_buf;
        seastar::output_stream<char> _write_buf;
        seastar::future<std::string> read();
        seastar::future<> handle(const std::string& cmd);
        seastar::future<> handle_store(const std::string& key, const std::string& val);
        seastar::future<> handle_load(const std::string& key);
        seastar::future<> handle_unknown_cmd();
        seastar::future<> handle_invalid_args();
        seastar::future<> write_and_flush(const std::string& msg);
    public:
        tcp_connection(
            tcp_server& server, 
            seastar::connected_socket&& fd, 
            seastar::socket_address addr
        );
        seastar::future<> process();        
    };
};
